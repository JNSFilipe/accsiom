/* #include <stdio.h> */
/* #include "include/axlog.h" */
/* #include "include/axtypes.h" */
/* #define AXALLOC_IMPLEMENTATION */
/* #include "include/axalloc.h" */
/* #define AXMATRIX_IMPLEMENTATION */
/* #include "include/axmatrix.h" */


/* mi32 main(void) { */

/*   Arena* arena = ax_arena_create(4096); // Create arena */

/*   mi32* arr = (mi32*)ax_alloc(arena, 100 * sizeof(mi32)); // Allocate array */

/*   printf("Hello WOrld %d \n",arr[9]); */
/*   ax_arena_destroy(arena); // Free all memory */

/*   return 0; */
/* } */
#define AXALLOC_IMPLEMENTATION
#include "include/axalloc.h"
#define AXMATRIX_IMPLEMENTATION
#include "include/axmatrix.h"
#include <stdio.h>

int main(void) {
  // Create an arena for testing
  Arena* arena = ax_arena_create(4096);
    
  // Create a 5x5 matrix using the arena
  AxMatrix* mat1 = ax_matrix_create(5, 5, arena);
    
  // Create a 5x5 matrix using regular malloc
  AxMatrix* mat2 = ax_matrix_create(5, 5, NULL);
    
  // Initialize matrix values
  for (musz i = 0; i < 5; i++) {
    for (musz j = 0; j < 5; j++) {
      AX_MATRIX_AT(*mat1, i, j) = (axm_type)(i * 10 + j);
      AX_MATRIX_AT(*mat2, i, j) = 0.0;
    }
  }

  printf("\nmat1:\n");
  ax_matrix_print(mat1, "%.1f");
  printf("\nmat2:\n");
  ax_matrix_print(mat2, "%.1f");
  printf("\n\n");

  // Print some initial values
  printf("Original mat1[2,3]: %.1f\n", AX_MATRIX_AT(*mat1, 2, 3));
  printf("Original mat2[3,4]: %.1f\n", AX_MATRIX_AT(*mat2, 3, 4));

  // Create a slice of mat1 (rows 1-3, columns 2-4)
  AxMatrix slice = AX_MATRIX_SLICE(*mat1, 
                                   AX_RANGE(1, 4), 
                                   AX_RANGE(2, 5));

  printf("\nslice:\n");
  ax_matrix_print(&slice, "%.1f");
  printf("\n\n");
    
  // Modify the slice
  AX_MATRIX_AT(slice, 0, 0) = 99.0;  // This should modify mat1[1,2]
  AX_MATRIX_AT(slice, 2, 2) = 55.0;  // This should modify mat1[3,4]

  // Copy slice to mat2's bottom-right corner
  AxMatrix target = AX_MATRIX_SLICE(*mat2, 
                                    AX_RANGE(2, 5), 
                                    AX_RANGE(2, 5));
  ax_matrix_copy(&target, &slice);

  printf("\ntarget:\n");
  ax_matrix_print(&target, "%.1f");
  printf("\n\n");

  printf("\nmat1:\n");
  ax_matrix_print(mat1, "%.1f");
  printf("\nmat2:\n");
  ax_matrix_print(mat2, "%.1f");
  printf("\n\n");

  // Print modified values
  printf("\nAfter modification:\n");
  printf("mat1[1,2]: %.1f (should be 99.0)\n", AX_MATRIX_AT(*mat1, 1, 2));
  printf("mat1[3,4]: %.1f (should be 55.0)\n", AX_MATRIX_AT(*mat1, 3, 4));
  printf("mat2[3,4]: %.1f (should be 55.0)\n", AX_MATRIX_AT(*mat2, 3, 4));
  printf("mat2[2,2]: %.1f (should be 99.0)\n", AX_MATRIX_AT(*mat2, 2, 2));

  // Cleanup
  ax_arena_destroy(arena);
  ax_matrix_destroy(mat2);

  return 0;

}
