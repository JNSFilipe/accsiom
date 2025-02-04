#ifndef AXMATRIX_H_
#define AXMATRIX_H_

#include "axlog.h"
#include "axalloc.h"
#include "axtypes.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AX_MATRIX_ELEMENT_TYPE
#define AX_MATRIX_ELEMENT_TYPE double
#endif

typedef AX_MATRIX_ELEMENT_TYPE axm_type;

typedef struct AxRange {
    musz start;
    musz end; // exclusive
} AxRange;

#define AX_RANGE(start, end) (AxRange){ (start), (end) }

typedef struct AxMatrix {
    musz rows;
    musz cols;
    musz stride; // Number of elements between rows
    axm_type* data;
    bool data_owner; // Whether to free data on destroy
} AxMatrix;

// Matrix initialization and creation
bool ax_matrix_init(AxMatrix* mat, musz rows, musz cols, Arena* arena);
AxMatrix* ax_matrix_create(musz rows, musz cols, Arena* arena);
void ax_matrix_destroy(AxMatrix* mat);

// Element access
#define AX_MATRIX_AT(mat, i, j) ((mat).data[(i) * (mat).stride + (j)])

// Slicing
#define AX_MATRIX_SLICE(mat, row_range, col_range) \
    ((AxMatrix){ \
        .rows = (row_range.end) - (row_range.start), \
        .cols = (col_range.end) - (col_range.start), \
        .stride = (mat).stride, \
        .data = &(mat).data[(row_range.start) * (mat).stride + (col_range.start)], \
        .data_owner = false \
    })

// Copy data from src to dest (must have same dimensions)
bool ax_matrix_copy(AxMatrix* dest, const AxMatrix* src);

// Print matrix
void ax_matrix_print(const AxMatrix *mat, const char* fmt);

#ifdef __cplusplus
}
#endif

#endif /* AXMATRIX_H_ */

#ifdef AXMATRIX_IMPLEMENTATION

#include <stdlib.h>

bool ax_matrix_init(AxMatrix* mat, musz rows, musz cols, Arena* arena) {
    musz nelem = rows * cols;
    musz size = nelem * sizeof(axm_type);
    if (arena) {
        mat->data = (axm_type*) ax_alloc(arena, size);
    } else {
        AX_LOG(AX_LOG_INFO, "Arena is NULL, using malloc");
        AX_LOG(AX_LOG_WARN, "DO NOT FORGET TO CALL free");
        mat->data = (axm_type*) malloc(size);
    }
    if (!mat->data) {
        AX_LOG(AX_LOG_FATAL, "ax_matrix_init: failed to allocate data");
        return false;
    }
    mat->rows = rows;
    mat->cols = cols;
    mat->stride = cols;
    mat->data_owner = (arena == NULL);
    return true;
}

AxMatrix* ax_matrix_create(musz rows, musz cols, Arena* arena) {
    AxMatrix* mat;
    if (arena) {
        mat = (AxMatrix*) ax_alloc(arena, sizeof(AxMatrix));
    } else {
        AX_LOG(AX_LOG_INFO, "Arena is NULL, using malloc");
        AX_LOG(AX_LOG_WARN, "DO NOT FORGET TO CALL ax_matrix_destroy");
        mat = (AxMatrix*) malloc(sizeof(AxMatrix));
    }
    if (!mat) {
        AX_LOG(AX_LOG_FATAL, "ax_matrix_create: failed to allocate matrix struct");
        return NULL;
    }
    if (!ax_matrix_init(mat, rows, cols, arena)) {
        if (!arena) {
            free(mat);
        }
        return NULL;
    }
    return mat;
}

void ax_matrix_destroy(AxMatrix* mat) {
    if (!mat) return;
    if (mat->data_owner) {
        free(mat->data);
    }
    mat->data = NULL;
    mat->rows = 0;
    mat->cols = 0;
    mat->stride = 0;
}

bool ax_matrix_copy(AxMatrix* dest, const AxMatrix* src) {
    if (!dest || !src) {
        AX_LOG(AX_LOG_FATAL, "ax_matrix_copy: null matrix");
        return false;
    }
    if (dest->rows != src->rows || dest->cols != src->cols) {
        AX_LOG(AX_LOG_FATAL, "ax_matrix_copy: dimension mismatch");
        return false;
    }
    for (musz i = 0; i < dest->rows; i++) {
        for (musz j = 0; j < dest->cols; j++) {
            AX_MATRIX_AT(*dest, i, j) = AX_MATRIX_AT(*src, i, j);
        }
    }
    return true;
}

void ax_matrix_print(const AxMatrix* mat, const char* fmt) {
    if (!mat || !mat->data) {
        AX_LOG(AX_LOG_FATAL, "ax_matrix_print: invalid matrix");
        return;
    }
    if (!fmt) fmt = "% .6g";  // Default format with space for alignment
    
    // First pass to find maximum width per column
    musz *widths = (musz*)calloc(mat->cols, sizeof(musz));
    for (musz j = 0; j < mat->cols; j++) {
        for (musz i = 0; i < mat->rows; i++) {
            char buf[256];
            musz len = (musz)snprintf(buf, sizeof(buf), fmt, AX_MATRIX_AT(*mat, i, j));
            if (len > widths[j]) widths[j] = len;
        }
    }
    
    // Second pass to print with column alignment
    for (musz i = 0; i < mat->rows; i++) {
        for (musz j = 0; j < mat->cols; j++) {
            char buf[256];
            snprintf(buf, sizeof(buf), fmt, AX_MATRIX_AT(*mat, i, j));
            printf("%*s", (int)widths[j], buf);
            if (j < mat->cols - 1) printf("  ");  // 2 spaces between columns
        }
        printf("\n");
    }
    
    free(widths);
}

#endif /* AXMATRIX_IMPLEMENTATION */
