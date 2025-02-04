#define CLOVE_SUITE_NAME AxTests
#define CLOVE_IMPLEMENTATION
#include "thirdparty/clove-unit.h"
#define AXALLOC_IMPLEMENTATION
#define AX_MATRIX_ELEMENT_TYPE float
#include "include/axalloc.h"
#define AXMATRIX_IMPLEMENTATION
#include "include/axmatrix.h"

float from_np(char *exp){
  FILE *fp;
  float result;
  char cmd[128], buffer[128];

  // Open a process to run Python and capture its output
  sprintf(cmd, "python3 -c \"import numpy as np;%s\"", exp);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    pclose(fp);
    AX_LOG(AX_LOG_FATAL, "popen failed!");
  }

  // Read output from Python
  if (fgets(buffer, sizeof(buffer), fp) != NULL) {
    // Convert string output to integer
    result = (float)atof(buffer);
  } else {
    pclose(fp);
    AX_LOG(AX_LOG_FATAL, "Failed to read output from Python");
  }

  // Close the process
  pclose(fp);

  return result;
}

CLOVE_TEST(AxMatrix) {
  // Create an arena for testing
  Arena* arena = ax_arena_create(4096);
    
  // Create a 5x5 matrix using the arena
  AxMatrix* mat1 = ax_matrix_create(5, 5, arena);
    
  // Create a 5x5 matrix using regular malloc
  AxMatrix* mat2 = ax_matrix_create(5, 5, arena);
    
  // Initialize matrix values
  for (musz i = 0; i < 5; i++) {
    for (musz j = 0; j < 5; j++) {
      AX_MATRIX_AT(*mat1, i, j) = (axm_type)(i * 10 + j);
      AX_MATRIX_AT(*mat2, i, j) = 0.0;
    }
  }

  // Create a slice of mat1 (rows 1-3, columns 2-4)
  AxMatrix slice = AX_MATRIX_SLICE(*mat1, 
                                   AX_RANGE(1, 4), 
                                   AX_RANGE(2, 5));

  // Test Slicing and indexing against numpy
  CLOVE_FLOAT_EQ_P(from_np("M=np.mgrid[0:5,0:5][0]*10+np.mgrid[0:5,0:5][1];S=M[1:4,2:5];print(S[0,0])"), AX_MATRIX_AT(slice, 0, 0), 0.01);
  CLOVE_FLOAT_EQ_P(from_np("M=np.mgrid[0:5,0:5][0]*10+np.mgrid[0:5,0:5][1];S=M[1:4,2:5];print(S[2,2])"), AX_MATRIX_AT(slice, 2, 2), 0.01);
    
  // Modify the slice
  AX_MATRIX_AT(slice, 0, 0) = 99.0;  // This should modify mat1[1,2]
  AX_MATRIX_AT(slice, 2, 2) = 55.0;  // This should modify mat1[3,4]

  // Copy slice to mat2's bottom-right corner
  AxMatrix target = AX_MATRIX_SLICE(*mat2, 
                                    AX_RANGE(2, 5), 
                                    AX_RANGE(2, 5));
  ax_matrix_copy(&target, &slice);

  // Print modified values
  CLOVE_FLOAT_EQ_P(99.0, AX_MATRIX_AT(*mat1, 1, 2), 0.01);
  CLOVE_FLOAT_EQ_P(55.0, AX_MATRIX_AT(*mat1, 3, 4), 0.01);
  CLOVE_FLOAT_EQ_P(24.0, AX_MATRIX_AT(*mat2, 3, 4), 0.01);
  CLOVE_FLOAT_EQ_P(99.0, AX_MATRIX_AT(*mat2, 2, 2), 0.01);

  // Cleanup
  ax_arena_destroy(arena);
}

CLOVE_RUNNER()
