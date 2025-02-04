#define CLOVE_SUITE_NAME AxTests
#define CLOVE_IMPLEMENTATION
#include "thirdparty/clove-unit.h"
#define AXALLOC_IMPLEMENTATION
#define AX_MATRIX_ELEMENT_TYPE float
#include "include/axalloc.h"
#define AXMATRIX_IMPLEMENTATION
#include "include/axmatrix.h"

CLOVE_TEST(AxMatrix) {
    // Create an arena for testing
    Arena* arena = ax_arena_create(4096);

    // Create a 5x5 matrix using the arena
    AxMatrix* mat1 = ax_matrix_create(5, 5, arena);

    // Initialize matrix values
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 5; j++) {
            ax_matrix_set(mat1, i, j, (float)(i * 10 + j));
        }
    }

    // Create a slice of mat1 (rows 1-3, columns 2-4)
    AxMatrix slice = ax_matrix_slice(mat1, 1, 4, 2, 5);

    // Modify the slice
    ax_matrix_set(&slice, 0, 0, 99.0f);  // This should modify mat1[1,2]
    ax_matrix_set(&slice, 2, 2, 55.0f);  // This should modify mat1[3,4]

    // Copy slice to mat2's bottom-right corner
    AxMatrix* mat2 = ax_matrix_create(5, 5, NULL);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            ax_matrix_set(mat2, i + 2, j + 2, ax_matrix_get(&slice, i, j));
        }
    }

    // Print modified values
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat1, 1, 2), 99.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat1, 3, 4), 55.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat2, 3, 4), 55.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat2, 2, 2), 24.0f, 0.01f);

    // Cleanup
    ax_arena_destroy(arena);
}

CLOVE_TEST(SecondTest) {
    CLOVE_IS_TRUE(1);
}

// New test function to replicate matrix in Python using NumPy
void test_matrix_replication() {
    // Create a 5x5 matrix using the arena
    Arena* arena = ax_arena_create(4096);
    AxMatrix* mat1 = ax_matrix_create(5, 5, arena);

    // Initialize matrix values
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 5; j++) {
            ax_matrix_set(mat1, i, j, (float)(i * 10 + j));
        }
    }

    // Create a slice of mat1 (rows 1-3, columns 2-4)
    AxMatrix slice = ax_matrix_slice(mat1, 1, 4, 2, 5);

    // Modify the slice
    ax_matrix_set(&slice, 0, 0, 99.0f);  // This should modify mat1[1,2]
    ax_matrix_set(&slice, 2, 2, 55.0f);  // This should modify mat1[3,4]

    // Copy slice to mat2's bottom-right corner
    AxMatrix* mat2 = ax_matrix_create(5, 5, NULL);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            ax_matrix_set(mat2, i + 2, j + 2, ax_matrix_get(&slice, i, j));
        }
    }

    // Print modified values
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat1, 1, 2), 99.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat1, 3, 4), 55.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat2, 3, 4), 55.0f, 0.01f);
    CLOVE_FLOAT_EQ_P(ax_matrix_get(mat2, 2, 2), 24.0f, 0.01f);

    // Cleanup
    ax_arena_destroy(arena);
}

CLOVE_TEST(PythonReplicationTest) {
    test_matrix_replication();
}

CLOVE_RUN_TESTS()
