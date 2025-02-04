/*
  ================================================================================
  AxAlloc: Arena Allocator (STB-Style Single-Header Library)
  ================================================================================
  - Provides a simple arena allocator using a linked list of memory blocks.
  - Allocations are O(1) and come from the "current" block; new blocks are
  added on-demand.
  - All allocated memory is freed at once when the arena is destroyed.
  - The default block size is customizable; if 0 is passed, a fallback
  (4096 bytes by default) is used.
  - Dependencies:
  - "axtypes.h"     (defines mu8, musz, etc.)
  - "axlog.h"       (for AX_LOG(...) macros)
  - <stdlib.h>      (malloc, free)
  - <stdalign.h>    (alignof, max_align_t)
  ================================================================================
  USAGE:
  1) In **one** C or C++ file where you want the implementation, do:
  #define AXALLOC_IMPLEMENTATION
  #include "axalloc.h"

  2) In any other files that need to use the library, just include "axalloc.h"
  without defining AXALLOC_IMPLEMENTATION.
  ================================================================================
*/

#ifndef AXALLOC_H_
#define AXALLOC_H_

#include "axtypes.h"
#include "axlog.h"
#include <stdlib.h>     // for malloc/free
#include <stdalign.h>   // for alignof, max_align_t

#ifdef __cplusplus
extern "C" {
#endif

  /*
    --------------------------------------------------------------------------------
    Data Structures
    --------------------------------------------------------------------------------
  */

  /**
   * @brief A single block of memory within the arena.
   *
   * @note Internal use only.
   */
  typedef struct ArenaBlock {
    mu8*            memory;   /**< Pointer to the allocated memory chunk */
    musz            size;     /**< Total size of the memory chunk */
    musz            used;     /**< Number of bytes currently in use */
    struct ArenaBlock* next;  /**< Pointer to the next block in the chain */
  } ArenaBlock;

  /**
   * @brief The main Arena structure that manages a linked list of ArenaBlock.
   *
   * Use ax_arena_create() and ax_arena_destroy() to manage this.
   */
  typedef struct Arena {
    ArenaBlock* head;               /**< The first block in the chain */
    ArenaBlock* current;            /**< The block currently being allocated from */
    musz        default_block_size; /**< The default block size for new blocks */
  } Arena;

  /*
    --------------------------------------------------------------------------------
    Function Declarations
    --------------------------------------------------------------------------------
  */

  /**
   * @brief Creates a new arena with a given default block size.
   *
   * @param default_block_size The size (in bytes) for each new block. If 0 is 
   *        provided, a default of 4096 bytes is used.
   * @return A pointer to the newly created Arena, or NULL on failure.
   *
   * Usage:
   * @code
   *   Arena* my_arena = ax_arena_create(1024); // Creates an arena with 1KB blocks
   *   // ...
   *   ax_arena_destroy(my_arena);
   * @endcode
   */
  Arena* ax_arena_create(musz default_block_size);

  /**
   * @brief Destroys an arena, freeing all memory used by its blocks.
   *
   * @param arena Pointer to the Arena to destroy. If NULL, this does nothing.
   *
   * Usage:
   * @code
   *   ax_arena_destroy(my_arena);
   *   my_arena = NULL; // Always good practice to set the pointer to NULL
   * @endcode
   */
  void ax_arena_destroy(Arena* arena);

  /**
   * @brief Allocates `size` bytes from the arena.
   *
   * @param arena Pointer to the Arena from which to allocate.
   * @param size  Number of bytes to allocate.
   * @return Pointer to the allocated memory, or NULL if allocation fails or if
   *         `size` is 0.
   *
   * @note The allocated memory is aligned to `max_align_t`. 
   *       If the current block doesn’t have enough space, a new block is allocated.
   */
  void* ax_alloc(Arena* arena, musz size);

#ifdef __cplusplus
}
#endif

/* 
   ------------------------------------------------------------------------------
   Implementation
   ------------------------------------------------------------------------------
*/
#ifdef AXALLOC_IMPLEMENTATION

/*--------------------------------------------------------------------------
  Internal helper: Aligns `size` up to the given `alignment`.
  Example: ax_align_up(13, 8) => 16
  --------------------------------------------------------------------------*/
static musz ax_align_up(musz size, musz alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

/*--------------------------------------------------------------------------
  Creates a new arena with an initial block.
  --------------------------------------------------------------------------*/
Arena* ax_arena_create(musz default_block_size) {
  /* Allocate the Arena struct */
  Arena* arena = (Arena*)malloc(sizeof(Arena));
  if (!arena) {
    AX_LOG(AX_LOG_FATAL, "Failed to allocate Arena struct");
    return NULL;
  }

  /* Determine the block size we will use */
  if (default_block_size > 0) {
    arena->default_block_size = default_block_size;
  } else {
    arena->default_block_size = 4096; /* fallback */
  }

  /* Allocate the first ArenaBlock */
  ArenaBlock* block = (ArenaBlock*)malloc(sizeof(ArenaBlock));
  if (!block) {
    AX_LOG(AX_LOG_FATAL, "Failed to allocate ArenaBlock");
    free(arena);
    return NULL;
  }

  /* Allocate the block's memory */
  block->memory = (mu8*)malloc(arena->default_block_size);
  if (!block->memory) {
    AX_LOG(AX_LOG_FATAL, "Failed to allocate %zu bytes for ArenaBlock",
           arena->default_block_size);
    free(block);
    free(arena);
    return NULL;
  }

  block->size = arena->default_block_size;
  block->used = 0;
  block->next = NULL;

  arena->head = block;
  arena->current = block;

  return arena;
}

/*--------------------------------------------------------------------------
  Destroys the arena and frees all memory blocks in its chain.
  --------------------------------------------------------------------------*/
void ax_arena_destroy(Arena* arena) {
  if (!arena) {
    return;
  }

  ArenaBlock* block = arena->head;
  while (block) {
    ArenaBlock* next = block->next;
    free(block->memory);
    free(block);
    block = next;
  }

  free(arena);
}

/*--------------------------------------------------------------------------
  Allocates `size` bytes from the arena, aligned to `max_align_t`.
  If the current block doesn't have enough space, a new one is created.
  --------------------------------------------------------------------------*/
void* ax_alloc(Arena* arena, musz size) {
  if (!arena) {
    AX_LOG(AX_LOG_FATAL, "ax_alloc: NULL arena passed");
    return NULL;
  }
  if (size == 0) {
    AX_LOG(AX_LOG_WARN, "ax_alloc: requested 0 bytes (ignored)");
    return NULL;
  }

  /* Ensure alignment */
  usz alignment    = alignof(max_align_t);
  usz aligned_size = ax_align_up(size, alignment);

  /* Get the current block and find aligned offset */
  ArenaBlock* current = arena->current;
  usz aligned_used   = ax_align_up(current->used, alignment);

  /* If enough space remains in the current block, use it */
  if (aligned_used + aligned_size <= current->size) {
    void* ptr     = current->memory + aligned_used;
    current->used = aligned_used + aligned_size;
    return ptr;
  }

  /* Otherwise, allocate a new block */
  usz new_block_size = (aligned_size > arena->default_block_size)
    ? aligned_size
    : arena->default_block_size;

  ArenaBlock* new_block = (ArenaBlock*)malloc(sizeof(ArenaBlock));
  if (!new_block) {
    AX_LOG(AX_LOG_FATAL, "ax_alloc: failed to allocate new ArenaBlock");
    return NULL;
  }

  new_block->memory = (mu8*)malloc(new_block_size);
  if (!new_block->memory) {
    AX_LOG(AX_LOG_FATAL, "ax_alloc: failed to allocate %zu bytes for new block",
           new_block_size);
    free(new_block);
    return NULL;
  }

  new_block->size = new_block_size;
  new_block->used = aligned_size;
  new_block->next = NULL;

  current->next   = new_block;
  arena->current  = new_block;

  /* First allocation in this new block starts at offset 0 */
  return new_block->memory;
}

#endif /* AXALLOC_IMPLEMENTATION */

#endif /* AXALLOC_H_ */
