#include<ss.h>
#include<stdlib.h>
#include<private_ss.h>
#include<time.h>
#include<string.h>

typedef struct __ss_vector {
    ss_matrix *matrix;
} ss_vector;

ss_vector * ss_vector_create_new(uint32_t size, ss_error *error) {
    if (error) ss_clear_error(error);
    ss_vector *vector;
    if (size == 0) {
        ss_set_error(error, "Invalid size(0)", SS_INVALID_SIZE);
        return NULL;
    }
    vector = (ss_vector *)malloc(sizeof(ss_vector));
    if (vector == NULL) {
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        return NULL;
    }
    vector -> matrix = ss_matrix_create_new(size, 1, error);
    if (vector -> matrix == NULL) {
        free(vector);
        return NULL;
    }
    return vector;
}

void __ss_vector_destroy(ss_vector **vector, ss_error *error) {
    if (error) ss_clear_error(error);
    if (*vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
    }
    ss_matrix_destroy((*vector) -> matrix, error);
    free(*vector);
    *vector = NULL;
}

void ss_vector_set(ss_vector *vector, uint32_t index, double value, ss_error *error) {
    if (error) ss_clear_error(error);
   char error_message[4096];
   if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
   }
   if (index >= ss_matrix_get_row_count(vector -> matrix, error)) {
        sprintf(error_message, "Invalid index (%u)", index);
        ss_set_error(error, error_message, SS_INVALID_INDEX);
        return;
   }
   ss_matrix_set(vector -> matrix, index, 0, value, error);
}

double ss_vector_get(const ss_vector *vector, uint32_t index, ss_error *error) {
    if (error) ss_clear_error(error);
   char error_message[4096];
   if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return 0.0;
   }
   if (index >= ss_matrix_get_row_count(vector -> matrix, error)) {
        sprintf(error_message, "Invalid index (%u)", index);
        ss_set_error(error, error_message, SS_INVALID_INDEX);
        return 0.0;
   }
   return ss_matrix_get(vector -> matrix, index, 0, error);
}

uint32_t ss_vector_get_size(const ss_vector *vector, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return 0;
    }
    return ss_matrix_get_row_count(vector -> matrix, error);
}

void ss_vector_write(const ss_vector *vector, const char * filename, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer argument(1)", SS_NULL_POINTER);
        return;
    }
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer argument(2)", SS_NULL_POINTER);
        return;
    }
    ss_matrix_write(vector -> matrix, filename, error);
}

ss_vector * ss_vector_read(const char * filename, ss_error *error) {
    uint16_t error_code;
    ss_vector *vector;
    ss_matrix *matrix;
    uint32_t rows, columns;
    if (error) ss_clear_error(error);
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return NULL;
    }
    matrix = ss_matrix_read(filename, error);
    if (matrix == NULL) {
        if (error) {
            error_code = ss_get_error_code(error);
            if (error_code == SS_INVALID_DIMENSIONS  || error_code == SS_INVALID_ROW_COUNT || error_code == SS_INVALID_COLUMN_COUNT) {
                ss_clear_error(error);
                ss_set_error(error, "Invalid file format", SS_FILE_FORMAT_ERROR);
            }
        }
        return NULL;
    }
    ss_matrix_get_dimensions(matrix, &rows, &columns, error);
    if (rows > 1 && columns > 1) {
        ss_matrix_destroy(matrix, error);
        ss_set_error(error, "File contains matrix", SS_FILE_CONTAINS_MATRIX);
        return NULL;
    }
    vector = (ss_vector *)malloc(sizeof(ss_vector));
    if (vector == NULL) {
        ss_matrix_destroy(matrix, error);
        ss_set_error(error, "Low memory", SS_LOW_MEMORY);
        return NULL;
    }
    if (columns == 1) {
        vector -> matrix = matrix;
    } else {
        ss_matrix_transpose(matrix, error);
        vector -> matrix = matrix;
    }
    return vector;
}

ss_vector * ss_vector_read_csv(const char * filename, uint8_t skip_lines, ss_error *error) {
    uint16_t error_code;
    ss_vector *vector;
    ss_matrix *matrix;
    uint32_t rows, columns;
    if (error) ss_clear_error(error);
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return NULL;
    }
    matrix = ss_matrix_read_csv(filename, skip_lines, error);
    if (matrix == NULL) {
        if (error) {
            error_code = ss_get_error_code(error);
            if (error_code == SS_INVALID_DIMENSIONS  || error_code == SS_INVALID_ROW_COUNT || error_code == SS_INVALID_COLUMN_COUNT) {
                ss_clear_error(error);
                ss_set_error(error, "Invalid file format", SS_FILE_FORMAT_ERROR);
            }
        }
        return NULL;
    }
    ss_matrix_get_dimensions(matrix, &rows, &columns, error);
    if (rows > 1 && columns > 1) {
        ss_matrix_destroy(matrix, error);
        ss_set_error(error, "File contains matrix", SS_FILE_CONTAINS_MATRIX);
        return NULL;
    }
    vector = (ss_vector *)malloc(sizeof(ss_vector));
    if (vector == NULL) {
        ss_matrix_destroy(matrix, error);
        ss_set_error(error, "Low memory", SS_LOW_MEMORY);
        return NULL;
    }
    if (columns == 1) {
        vector -> matrix = matrix;
    } else {
        ss_matrix_transpose(matrix, error);
        vector -> matrix = matrix;
    }
    return vector;
}

void ss_vector_write_csv(const ss_vector *vector, const char * filename, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return;
    }
    if (filename == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return;
    }
    ss_matrix_write_csv(vector -> matrix, filename, error);
}

void ss_vector_fill_random(ss_vector *vector, double min, double max, ss_error *error) {
    uint32_t i;
    double value;
    uint32_t size;
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
    }
    srand(time(0));
    size = ss_vector_get_size(vector, error);
    for (i = 0; i < size; ++i) {
        value = min + ((double)rand() / RAND_MAX) * (max - min);
        ss_matrix_set(vector -> matrix, i, 0, value, error);
    }
}

ss_matrix * ss_vector_get_matrix(const ss_vector *vector, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return NULL;
    }
    return vector -> matrix;
}

void ss_vector_set_matrix(ss_vector *vector, ss_matrix *matrix, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer argument(1)", SS_NULL_POINTER);
        return;
    }
    if (matrix == NULL) {
        ss_set_error(error, "Null Pointer argument(2)", SS_NULL_POINTER);
        return;
    }
    if (vector -> matrix == matrix) return;
    ss_matrix_destroy(vector -> matrix, error);
    vector -> matrix = matrix;
}

ss_vector * ss_vector_transpose(const ss_vector *vector, ss_vector *transposed_vector_container, ss_error *error) {
    ss_vector *transposed_vector = NULL;
    uint32_t sz;
    char error_message[4096];
    uint32_t transposed_vector_container_size;

    if (error) ss_clear_error(error);

    if (!vector) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return NULL;
    }
    if (vector == transposed_vector_container) return transposed_vector_container;
    sz = ss_vector_get_size(vector, error);
    if (transposed_vector_container == NULL) {
        transposed_vector = ss_vector_create_new(sz, error);
        if (transposed_vector == NULL) {
            ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
            return NULL;
        }
    } else {
        transposed_vector_container_size = ss_vector_get_size(transposed_vector_container, error);
        if (transposed_vector_container_size != sz) {
            sprintf(error_message, "Invalid container size (%u)", transposed_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        transposed_vector = transposed_vector_container;
    }
    memcpy(ss_matrix_get_data(transposed_vector -> matrix, error), ss_matrix_get_data(vector -> matrix, error), sizeof(double)*sz);
    return transposed_vector;
}

void ss_vector_print(FILE *f, const ss_vector *vector, ss_error *error) {
    if (error) ss_clear_error(error);
    if (vector == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
    }
    ss_matrix_print(f, vector -> matrix, error);
}
