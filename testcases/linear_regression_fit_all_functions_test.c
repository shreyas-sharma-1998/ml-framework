#include<ss.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
    char error_message[4096];
    ss_matrix *matrix_1 = NULL;
    ss_matrix *matrix_2 = NULL;
    ss_matrix *matrix_3 = NULL;
    ss_vector *vector_1 = NULL;
    ss_vector *vector_2 = NULL;
    ss_vector *vector_3 = NULL;
    ss_vector *vector_4 = NULL;
    ss_vector *vector_5 = NULL;
    ss_error *error;
    uint32_t r,c;
    double value;
    error = ss_error_create_new();
    if (error == NULL) {
        printf("Low Memory\n");
        return 0;
    }
    printf("ss_matrix_file test starts\n");
    matrix_1 = ss_matrix_create_new(3, 4, error);
    ss_matrix_fill(matrix_1, 0, 0, 2, 3, 23.3753, error);
    if (ss_has_error(error)) {
        printf("ss_matrix_fill test failed\n");
        goto catch_error;
    }
    ss_matrix_print(stdout, matrix_1, error);
    printf("ss_matrix_file test ends\n");

    printf("ss_matrix_copy test starts\n");
    matrix_2 = ss_matrix_create_new(3, 4, error);
    ss_matrix_copy(matrix_2, 0, 0, matrix_1, 0, 0, 2, 3, error);
    if (ss_has_error(error)) {
        printf("ss_matrix_copy test failed\n");
        goto catch_error;
    }
    ss_matrix_print(stdout, matrix_2, error);
    printf("ss_matrix_copy test ends\n");

    printf("ss_matrix_transpose_create_new test starts\n");
    value = 1.0;
    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 4; ++c) {
            ss_matrix_set(matrix_2, r, c, value++, error);
        }
    }
    matrix_3 = ss_matrix_transpose_create_new(matrix_2, error);
    if (ss_has_error(error)) {
        printf("ss_matrix_transpose_create_new test failed\n");
        goto catch_error;
    }
    ss_matrix_print(stdout, matrix_3, error);
    printf("ss_matrix_transpose_create_new test ends\n");

    printf("ss_vector_fill_random test starts\n");
    vector_1 = ss_vector_create_new(4, error);
    ss_vector_fill_random(vector_1, INT_MIN, INT_MAX, error);
    ss_vector_print(stdout, vector_1, error);
    printf("ss_vector_fill_random test ends\n");

    printf("ss_multiply_matrix_with_vector  test starts\n");
    for (r = 0, value = 10.0; r < 4; ++r, value+=10.0) ss_vector_set(vector_1, r, value, error);
    printf("Left operand matrix\n");
    ss_matrix_print(stdout, matrix_2, error);
    printf("Right operand vector\n");
    ss_vector_print(stdout, vector_1, error);
    vector_2 = ss_vector_create_new(3, error);
    vector_2 = ss_multiply_matrix_with_vector(matrix_2, vector_1, vector_2, error);
    if (ss_has_error(error)) {
        printf("ss_multiply_matrix_with_vector test failed\n");
        goto catch_error;
    }
    printf("Product vector\n");
    ss_vector_print(stdout, vector_2, error);
    printf("ss_multiply_matrix_with_vector test ends\n");

    printf("ss_subtract_vector test starts\n");
    vector_3 = ss_vector_create_new(3, error);
    ss_vector_set(vector_3, 0, 30.33, error);
    ss_vector_set(vector_3, 1, 40.44, error);
    ss_vector_set(vector_3, 2, 50.55, error);
    // vector4 = ss_vector_create_new(3, error);
    vector_4 = ss_subtract_vector(vector_2, vector_3, vector_4, error);
    if (ss_has_error(error)) {
        printf("ss_subtract_vector test failed\n");
        goto catch_error;
    }
    ss_vector_print(stdout, vector_4, error);
    printf("ss_vector_subtract test ends\n");

    printf("ss_vector_transpose test starts\n");
    vector_5 = ss_vector_create_new(3, error);
    vector_5 = ss_vector_transpose(vector_4, vector_5, error);
    if (ss_has_error(error)) {
        printf("ss_vector_transpose test failed\n");
        goto catch_error;
    }
    ss_vector_print(stdout, vector_5, error);
    printf("ss_vector_transpose test ends\n");

    printf("ss_multiply_vector_get_scalar test starts\n");
    for (value = 1.0, r = 0; r < 3; ++r, value += 5) ss_vector_set(vector_4, r, value, error);
    for (value = 10.0, r = 0; r < 3; ++r, value += 3) ss_vector_set(vector_5, r, value, error);
    printf("Row vector \n");
    ss_vector_print(stdout, vector_4, error);
    printf("Column vector \n");
    ss_vector_print(stdout, vector_5, error);
    value = ss_multiply_vector_get_scalar(vector_4, vector_5, error);
    if (ss_has_error(error)) {
        printf("ss_multiply_vector_get_scalar test failed\n");
        goto catch_error;
    }
    printf("Product : %f\n", value);
    printf("ss_multiply_vector_get_scalar test ends\n");

    printf("ss_multiply_vector_with_scalar test starts\n");
    vector_5 = ss_multiply_vector_with_scalar(vector_4, 100, vector_5, error);
    if (ss_has_error(error)) {
        printf("ss_multiply_vector_with_scalar test failed\n");
        goto catch_error;
    }
    ss_vector_print(stdout, vector_5, error);
    printf("ss_multiply_vector_with_scalar test ends\n");
    goto prepare_for_exit;
    catch_error:
        ss_get_error(error_message, 4096, error);
        printf("Error : %s\n", error_message);
    prepare_for_exit:
        ss_matrix_destroy(matrix_1, error);
        ss_matrix_destroy(matrix_2, error);
        ss_matrix_destroy(matrix_3, error);
        ss_vector_destroy(vector_1, error);
        ss_vector_destroy(vector_2, error);
        ss_vector_destroy(vector_3, error);
        ss_vector_destroy(vector_4, error);
        ss_vector_destroy(vector_5, error);
        ss_error_destroy(error);
    return 0;
}