#include<ss.h>
#include<private_ss.h>
#include<stdio.h>
#include<math.h>

double ss_mean_squared_error(ss_vector *left_vector, ss_vector *right_vector, ss_error *error) {
    double mean_squared_error;
    double *data, *left_data, *right_data;
    uint32_t i;
    ss_matrix *left_matrix = NULL;
    ss_matrix *right_matrix = NULL;
    ss_matrix *product_matrix = NULL;
    ss_matrix *matrix = NULL;
    ss_vector *difference = NULL;
    char error_message[4096];
    uint32_t left_size, right_size;
    if (error) ss_clear_error(error);
    if (left_vector == NULL) {
        ss_set_error(error, "Null pointer argument(1)", SS_NULL_POINTER);
        return 0.0;
    }
    if (right_vector == NULL) {
        ss_set_error(error, "Null pointer argument(2)", SS_NULL_POINTER);
        return 0.0;
    }
    left_size = ss_vector_get_size(left_vector, error);
    right_size = ss_vector_get_size(right_vector, error);
    if (left_size != right_size) {
        sprintf(error_message, "Unable to compute mean square error with vector of size %u and %u", left_size, right_size);
        ss_set_error(error, error_message, SS_VECTOR_SIZE_MISMATCH);
        return 0.0;
    }
    difference = ss_subtract_vector(left_vector, right_vector, NULL, error);
    if (difference == NULL) {
        mean_squared_error = 0.0;
        left_matrix = ss_vector_get_matrix(left_vector, error);
        right_matrix = ss_vector_get_matrix(right_vector, error);
        left_data = ss_matrix_get_data(left_matrix, error);
        right_data = ss_matrix_get_data(right_matrix, error);
        for (i = 0; i < left_size; ++i) {
            mean_squared_error += pow((left_data[i] - right_data[i]), 2.0);
        }
        return mean_squared_error/left_size;
    }
    right_matrix = ss_vector_get_matrix(difference, error);
    left_matrix = ss_matrix_transpose_create_new(right_matrix, error);
    if (left_matrix == NULL) {
        goto fallback;
    }
    product_matrix = ss_multiply_matrix(left_matrix, right_matrix, NULL, error);
    if (product_matrix == NULL) {
        goto fallback;
    }
    mean_squared_error = ss_matrix_get(product_matrix, 0, 0, error)/left_size;
    ss_vector_destroy(difference, error);
    ss_matrix_destroy(product_matrix, error);
    ss_matrix_destroy(left_matrix, error);
    return mean_squared_error;
   fallback:
        matrix = ss_vector_get_matrix(difference, error);
        data = ss_matrix_get_data(matrix, error);
        mean_squared_error = 0.0;
        for (i = 0; i < left_size; ++i) {
            mean_squared_error += pow(data[i], 2.0);
        }
        ss_vector_destroy(difference, error);
        ss_matrix_destroy(product_matrix, error);
        ss_matrix_destroy(left_matrix, error);
        return mean_squared_error / left_size;
}

double ss_r2_score(const ss_vector *y_vector, const ss_vector *y_hat_vector, ss_error *error) {

    char error_message[4096];
    uint32_t y_hat_vector_size, y_vector_size;
    ss_vector *difference_vector;
    ss_vector *difference_vector_T;
    double n, d, mean;

    if (error) ss_clear_error(error);
    if (y_vector == NULL) {
        ss_set_error(error, "Null pointer argument(1)", SS_NULL_POINTER);
        return 0.0;
    }
    if (y_hat_vector == NULL) {
        ss_set_error(error, "Null pointer argument(2)", SS_NULL_POINTER);
        return 0.0;
    }
    y_vector_size = ss_vector_get_size(y_vector, error);
    y_hat_vector_size = ss_vector_get_size(y_hat_vector, error);
    if (y_vector_size != y_hat_vector_size) {
        sprintf(error_message, "Unable to compute R2Score with vector of size %u and %u", y_vector_size, y_hat_vector_size);
        ss_set_error(error, error_message, SS_VECTOR_SIZE_MISMATCH);
        return 0.0;
    }
    difference_vector = ss_subtract_vector(y_vector, y_hat_vector, NULL, error);
    if (ss_has_error(error)) return 0.0;
    difference_vector_T = ss_vector_transpose(difference_vector, NULL, error);
    if (ss_has_error(error)) return 0.0;
    n = ss_multiply_vector_get_scalar(difference_vector_T, difference_vector, error);
    mean = ss_mean_vector(y_vector, error);
    difference_vector = ss_subtract_scalar_from_vector(y_vector, mean, difference_vector, error);
    difference_vector_T = ss_vector_transpose(difference_vector, difference_vector_T, error);
    d = ss_multiply_vector_get_scalar(difference_vector_T, difference_vector, error);
    ss_vector_destroy(difference_vector, error);
    ss_vector_destroy(difference_vector_T, error);
    return 1.0 - (n/d);

}