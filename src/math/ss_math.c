#include<ss.h>
#include<stdlib.h>
#include<private_ss.h>
#include<math.h>

ss_matrix * ss_multiply_matrix(const ss_matrix *left_matrix, const ss_matrix *right_matrix, ss_matrix *product_matrix_container, ss_error *error) {
    uint32_t left_matrix_rows, left_matrix_columns;
    uint32_t right_matrix_rows, right_matrix_columns;
    uint32_t product_matrix_rows, product_matrix_columns;
    uint32_t product_matrix_container_rows, product_matrix_container_columns;
    uint32_t r,c,j;
    double *left_matrix_data;
    double *right_matrix_data;
    double *product_matrix_data;

    double value;
    ss_matrix *product_matrix;
    char error_message[4096];

    if (error) ss_clear_error(error);
    if (left_matrix == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return NULL;
    }
    if (right_matrix == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return NULL;
    }
    ss_matrix_get_dimensions(left_matrix, &left_matrix_rows, &left_matrix_columns, error);
    ss_matrix_get_dimensions(right_matrix, &right_matrix_rows, &right_matrix_columns, error);
    if (left_matrix_columns != right_matrix_rows) {
        sprintf(error_message, "Cannot multiply matrix [%u,%u]x[%u,%u]", left_matrix_rows, left_matrix_columns, right_matrix_rows, right_matrix_columns);
        ss_set_error(error, error_message, SS_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION);
        return NULL;
    }
    product_matrix_rows = left_matrix_rows;
    product_matrix_columns = right_matrix_columns;
    if (product_matrix_container == NULL) {
        product_matrix = ss_matrix_create_new(product_matrix_rows, product_matrix_columns, error);
        if (product_matrix == NULL) return NULL;
    } else {
        ss_matrix_get_dimensions(product_matrix_container, &product_matrix_container_rows, &product_matrix_container_columns, error);
        if (product_matrix_container_columns != product_matrix_columns || product_matrix_container_rows != product_matrix_rows) {
            sprintf(error_message, "Invalid container size (%ux%u)", product_matrix_container_rows, product_matrix_container_columns);
            ss_set_error(error, error_message, SS_INVALID_MATRIX_CONTAINER_DIMENSIONS);
            return NULL;
        }
        product_matrix = product_matrix_container;
    }
    left_matrix_data = ss_matrix_get_data(left_matrix, error);
    right_matrix_data = ss_matrix_get_data(right_matrix, error);
    product_matrix_data = ss_matrix_get_data(product_matrix, error);

    for (r = 0; r < product_matrix_rows; ++r) {
        for (c = 0; c <product_matrix_columns; ++c) {
            value = 0;
            for (j = 0; j < left_matrix_columns; ++j) {
                value += (left_matrix_data[r*left_matrix_columns + j]*right_matrix_data[j*right_matrix_columns+c]);
            }
            product_matrix_data[r*product_matrix_columns + c] = value;
        }
    }
    return product_matrix;
}

ss_vector * ss_multiply_matrix_with_vector(const ss_matrix *matrix, const ss_vector *vector, ss_vector *product_vector_container, ss_error *error) {
    char error_message[4096];
    uint32_t matrix_rows;
    uint32_t matrix_columns;
    uint32_t vector_size;
    ss_vector *product_vector;
    ss_matrix *product_matrix;
    uint32_t product_vector_container_size;
    if (error) ss_clear_error(error);
    if (matrix == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return NULL;
    }
    if (vector == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return NULL;
    }
    ss_matrix_get_dimensions(matrix, &matrix_rows, &matrix_columns, error);
    vector_size = ss_vector_get_size(vector, error);
    if (matrix_columns != vector_size) {
        sprintf(error_message, "Cannot multiply matrix [%u,%u] with vector [%u]", matrix_rows, matrix_columns, vector_size);
        ss_set_error(error, error_message, SS_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION);
        return NULL;
    }
    if (product_vector_container == NULL) {
        product_vector = ss_vector_create_new(1, error);
        if (product_vector == NULL) return NULL;
        product_matrix = NULL;
    } else {
        product_vector_container_size = ss_vector_get_size(product_vector_container, error);
        if (product_vector_container_size != matrix_rows) {
            sprintf(error_message, "Invalid container size : %u)", product_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        product_matrix = ss_vector_get_matrix(product_vector_container, error);
        product_vector = product_vector_container;
    }
    product_matrix = ss_multiply_matrix(matrix, ss_vector_get_matrix(vector, error), product_matrix, error);
    if (product_matrix == NULL) return NULL;
    ss_vector_set_matrix(product_vector, product_matrix, error);
    return product_vector;
}

ss_vector * ss_subtract_vector(const ss_vector *left_vector, const ss_vector *right_vector, ss_vector *difference_vector_container, ss_error *error) {
    char error_message[4096];
    uint32_t i;
    ss_vector *result_vector;
    uint32_t left_vector_size, right_vector_size, result_vector_size;
    uint32_t difference_vector_container_size;
    double *left_data, *right_data, *result_data;
    if (error) ss_clear_error(error);
    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return NULL;
    }
    if (right_vector == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return NULL;
    }
    left_vector_size = ss_vector_get_size(left_vector, error);
    right_vector_size = ss_vector_get_size(right_vector, error);
    if (left_vector_size != right_vector_size) {
        sprintf(error_message, "Cannot subtract vector of size %u from vector of size %u", right_vector_size, left_vector_size);
        ss_set_error(error, error_message, SS_VECTOR_SUBTRACT_SIZE_ERROR);
        return NULL;
    }
    result_vector_size = left_vector_size;
    if (difference_vector_container == NULL) {
        result_vector = ss_vector_create_new(result_vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        difference_vector_container_size = ss_vector_get_size(difference_vector_container, error);
        if (difference_vector_container_size != result_vector_size) {
            sprintf(error_message, "Invalid container size : %u)", difference_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = difference_vector_container;
    }
    left_data = ss_matrix_get_data(ss_vector_get_matrix(left_vector, error), error);
    right_data = ss_matrix_get_data(ss_vector_get_matrix(right_vector, error), error);
    result_data = ss_matrix_get_data(ss_vector_get_matrix(result_vector, error), error);
    for (i = 0; i < result_vector_size; ++i) {
        // [i*1 + 0] can be written as [i]
        result_data[i] = left_data[i] - right_data[i];
    }
    return result_vector;
}

double ss_multiply_vector_get_scalar(const ss_vector *left_vector, const ss_vector *right_vector, ss_error *error) {
    uint32_t i;
    double product;
    double *left_data, *right_data;
    char error_message[4096];
    uint32_t left_vector_size;
    uint32_t right_vector_size;
    if (error) ss_clear_error(error);

    if (!left_vector) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return 0.0;
    }
    if (!right_vector) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return 0.0;
    }

    left_vector_size = ss_vector_get_size(left_vector, error);
    right_vector_size = ss_vector_get_size(right_vector, error);

    if (left_vector_size != right_vector_size) {
        sprintf(error_message, "Cannot multiply vector of size %u with vector of size %u and produce a scalar value", left_vector_size, right_vector_size);
        ss_set_error(error, error_message, SS_INVALID_SIZE_FOR_VECTOR_MULTIPLICATION);
        return 0.0;
    }
    left_data = ss_matrix_get_data(ss_vector_get_matrix(left_vector, error), error);
    right_data = ss_matrix_get_data(ss_vector_get_matrix(right_vector, error), error);
    for (i = 0, product = 0.0; i < left_vector_size; ++i) product += (left_data[i] * right_data[i]);
    return product;
}

ss_vector * ss_multiply_vector_with_scalar(const ss_vector *left_vector, double right_value, ss_vector *product_vector_container, ss_error *error) {
    char error_message[4096];
    double value;
    uint32_t i;
    ss_vector *product_vector;
    uint32_t left_vector_size;
    uint32_t product_vector_container_size;

    if (error) ss_clear_error(error);

    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return NULL;
    }
    left_vector_size = ss_vector_get_size(left_vector, error);
    if (product_vector_container == NULL) {
        product_vector = ss_vector_create_new(left_vector_size, error);
        if (product_vector == NULL) return NULL;
    } else {
        product_vector_container_size = ss_vector_get_size(product_vector_container, error);
        if (left_vector_size != product_vector_container_size) {
            sprintf(error_message, "Invalid container size (%u)", product_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        product_vector = product_vector_container;
    }
    for (i = 0; i < left_vector_size; ++i) {
        value = ss_vector_get(left_vector, i, error);
        value *= right_value;
        ss_vector_set(product_vector, i, value, error);
    }
    return product_vector;
}

double ss_sum_vector(const ss_vector *vector, ss_error *error) {
    double sum = 0.0;
    uint32_t vector_size, i;

    if (error) ss_clear_error(error);

    if (vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return sum;
    }
    vector_size = ss_vector_get_size(vector, error);
    for (i = 0; i < vector_size; ++i) {
        sum += ss_vector_get(vector, i, error);
    }
    return sum;
}

double ss_mean_vector(const ss_vector *vector, ss_error *error) {
    double sum = 0.0;
    uint32_t vector_size;

    if (error) ss_clear_error(error);

    if (vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return sum;
    }
    vector_size = ss_vector_get_size(vector, error);
    sum = ss_sum_vector(vector, error);
    return sum/vector_size;
}

ss_vector * ss_subtract_scalar_from_vector(const ss_vector *left_vector, double right_value, ss_vector *subtract_vector_container, ss_error *error) {
    char error_message[4096];
    ss_vector *result_vector;
    uint32_t left_vector_size, i, subtract_vector_container_size;
    double value;

    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return NULL;
    }
    left_vector_size = ss_vector_get_size(left_vector, error);
    if (subtract_vector_container == NULL) {
        result_vector = ss_vector_create_new(left_vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        subtract_vector_container_size = ss_vector_get_size(subtract_vector_container, error);
        if (subtract_vector_container_size != left_vector_size) {
            sprintf(error_message, "Invalid container size (%u)", subtract_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = subtract_vector_container;
    }
    for (i = 0; i < left_vector_size; ++i) {
        value = ss_vector_get(left_vector, i, error);
        value -= right_value;
        ss_vector_set(result_vector, i, value, error);
    }
    return result_vector;
}

ss_vector * ss_sigmoid_vector(const ss_vector *vector, ss_vector *result_vector_container, ss_error *error) {
    char error_message[4096];
    ss_vector *result_vector;
    double value, sigmoid_value;
    uint32_t vector_size, result_vector_container_size, i;

    if (vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return NULL;
    }
    vector_size = ss_vector_get_size(vector, error);
    if (result_vector_container == NULL) {
        result_vector = ss_vector_create_new(vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        result_vector_container_size = ss_vector_get_size(result_vector_container, error);
        if (result_vector_container_size != vector_size) {
            sprintf(error_message, "Invalid container size (%u)", result_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = result_vector_container;
    }
    for (i = 0; i < vector_size; ++i) {
        value = ss_vector_get(vector, i, error);
        sigmoid_value = 1.0 / (1.0 + exp(-value));
        ss_vector_set(result_vector, i, sigmoid_value, error);
    }
    return result_vector;
}

ss_vector * ss_log_vector(const ss_vector *vector, ss_vector *result_vector_container, ss_error *error) {
    char error_message[4096];
    ss_vector *result_vector;
    double value, log_value;
    uint32_t vector_size, result_vector_container_size, i;

    if (vector == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return NULL;
    }
    vector_size = ss_vector_get_size(vector, error);
    if (result_vector_container == NULL) {
        result_vector = ss_vector_create_new(vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        result_vector_container_size = ss_vector_get_size(result_vector_container, error);
        if (result_vector_container_size != vector_size) {
            sprintf(error_message, "Invalid container size (%u)", result_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = result_vector_container;
    }
    for (i = 0; i < vector_size; ++i) {
        value = ss_vector_get(vector, i, error);
        double safe_value = fmin(fmax(value, 1e-15), 1.0 - 1e-15);
        log_value = log(safe_value);
        ss_vector_set(result_vector, i, log_value, error);
    }
    return result_vector;
}

ss_vector * ss_multiply_vector_element_wise(const ss_vector *left_vector, const ss_vector *right_vector, ss_vector *result_vector_container, ss_error *error) {

    char error_message[4096];
    ss_vector *result_vector;
    double *left_data, *right_data;
    uint32_t left_vector_size, right_vector_size, result_vector_container_size, i;
    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return NULL;
    }
    if (right_vector == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return NULL;
    }

    left_vector_size = ss_vector_get_size(left_vector, error);
    right_vector_size = ss_vector_get_size(right_vector, error);

    if (left_vector_size != right_vector_size) {
        sprintf(error_message, "Cannot multiply vector of size %u with vector of size %u element wise", left_vector_size, right_vector_size);
        ss_set_error(error, error_message, SS_INVALID_SIZE_FOR_VECTOR_MULTIPLICATION);
        return NULL;
    }

    if (result_vector_container == NULL) {
        result_vector = ss_vector_create_new(left_vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        result_vector_container_size = ss_vector_get_size(result_vector_container, error);
        if (result_vector_container_size != left_vector_size) {
            sprintf(error_message, "Invalid container size (%u)", result_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = result_vector_container;
    }
    left_data = ss_matrix_get_data(ss_vector_get_matrix(left_vector, error), error);
    right_data = ss_matrix_get_data(ss_vector_get_matrix(right_vector, error), error);
    for (i = 0; i < left_vector_size; ++i) {
        ss_vector_set(result_vector, i, left_data[i]*right_data[i], error);
    }
    return result_vector;
}

ss_vector * ss_subtract_vector_from_scalar(double left_value, const ss_vector *right_vector, ss_vector *result_vector_container, ss_error *error) {
    char error_message[4096];
    ss_vector *result_vector;
    double *right_data;
    uint32_t right_vector_size, result_vector_container_size, i;
    if (right_vector == NULL) {
        ss_set_error(error, "Null pointer argument", SS_NULL_POINTER);
        return NULL;
    }

    right_vector_size = ss_vector_get_size(right_vector, error);

    if (result_vector_container == NULL) {
        result_vector = ss_vector_create_new(right_vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        result_vector_container_size = ss_vector_get_size(result_vector_container, error);
        if (result_vector_container_size != right_vector_size) {
            sprintf(error_message, "Invalid container size (%u)", result_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = result_vector_container;
    }

    right_data = ss_matrix_get_data(ss_vector_get_matrix(right_vector, error), error);

    for (i = 0; i < right_vector_size; ++i) {
        ss_vector_set(result_vector, i, 1.0 - right_data[i], error);
    }
    return result_vector;
}

ss_vector * ss_add_vector_element_wise(const ss_vector *left_vector, const ss_vector *right_vector, ss_vector *result_vector_container, ss_error *error) {
    char error_message[4096];
    ss_vector *result_vector;
    double *left_data, *right_data;
    uint32_t left_vector_size, right_vector_size, result_vector_container_size, i;
    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer argument (1)", SS_NULL_POINTER);
        return NULL;
    }
    if (right_vector == NULL) {
        ss_set_error(error, "Null pointer argument (2)", SS_NULL_POINTER);
        return NULL;
    }

    left_vector_size = ss_vector_get_size(left_vector, error);
    right_vector_size = ss_vector_get_size(right_vector, error);

    if (left_vector_size != right_vector_size) {
        sprintf(error_message, "Cannot multiply vector of size %u with vector of size %u element wise", left_vector_size, right_vector_size);
        ss_set_error(error, error_message, SS_INVALID_SIZE_FOR_VECTOR_MULTIPLICATION);
        return NULL;
    }

    if (result_vector_container == NULL) {
        result_vector = ss_vector_create_new(left_vector_size, error);
        if (result_vector == NULL) return NULL;
    } else {
        result_vector_container_size = ss_vector_get_size(result_vector_container, error);
        if (result_vector_container_size != left_vector_size) {
            sprintf(error_message, "Invalid container size (%u)", result_vector_container_size);
            ss_set_error(error, error_message, SS_INVALID_VECTOR_CONTAINER_SIZE);
            return NULL;
        }
        result_vector = result_vector_container;
    }
    left_data = ss_matrix_get_data(ss_vector_get_matrix(left_vector, error), error);
    right_data = ss_matrix_get_data(ss_vector_get_matrix(right_vector, error), error);
    for (i = 0; i < left_vector_size; ++i) {
        ss_vector_set(result_vector, i, left_data[i] + right_data[i], error);
    }
    return result_vector;
}

