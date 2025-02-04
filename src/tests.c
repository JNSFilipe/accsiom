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
    
    // Create a 5x5 matrix using regular malloc
    AxMatrix* mat2 = ax_matrix_create(5, 5, NULL);
    
    // Initialize matrix values
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 5; j++) {
            AX_MATRIX_AT(*mat1, i, j) = (axm_type)(i * 10 + j);
            AX_MATRIX_AT(*mat2, i, j) = 0.0;
        }
    }

    // Create a slice of mat1 (rows 1-3, columns 2-4)
    AxMatrix slice = AX_MATRIX_SLICE(*mat1, 
                                   AX_RANGE(1, 4), 
                                   AX_RANGE(2, 5));
    
    // Modify the slice
    AX_MATRIX_AT(slice, 0, 0) = 99.0;  // This should modify mat1[1,2]
    AX_MATRIX_AT(slice, 2, 2) = 55.0;  // This should modify mat1[3,4]

    // Copy slice to mat2's bottom-right corner
    AxMatrix target = AX_MATRIX_SLICE(*mat2, 
                                    AX_RANGE(2, 5), 
                                    AX_RANGE(2, 5));
    ax_matrix_copy(&target, &slice);

    // Print modified values
    CLOVE_FLOAT_EQ_P(AX_MATRIX_AT(*mat1, 1, 2), 99.0, 0.01);
    CLOVE_FLOAT_EQ_P(AX_MATRIX_AT(*mat1, 3, 4), 55.0, 0.01);
    CLOVE_FLOAT_EQ_P(AX_MATRIX_AT(*mat2, 3, 4), 55.0, 0.01);
    CLOVE_FLOAT_EQ_P(AX_MATRIX_AT(*mat2, 2, 2), 24.0, 0.01);

    // Cleanup
    ax_arena_destroy(arena);
}

CLOVE_TEST(SecondTest) {
    CLOVE_IS_TRUE(1);
}

CLOVE_RUNNER()
