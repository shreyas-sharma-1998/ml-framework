#include<ss.h>
#include<stdio.h>

int main() {

    ss_error *error;
    ss_matrix *matrix;
    char error_message[4096];
    uint32_t row, column;
    uint32_t rows, columns;
    double value;
    error = ss_error_create_new();
    matrix = ss_matrix_create_new(3, 5, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error : %s \n", error_message);
        return 0;
    }

    ss_matrix_set(matrix, 0, 0, 10.0, error);
    ss_matrix_set(matrix, 0, 1, 20.0, error);
    ss_matrix_set(matrix, 0, 2, 30.0, error);
    ss_matrix_set(matrix, 0, 3, 40.0, error);
    ss_matrix_set(matrix, 0, 4, 50.0, error);

    ss_matrix_set(matrix, 1, 0, 60.0, error);
    ss_matrix_set(matrix, 1, 1, 70.0, error);
    ss_matrix_set(matrix, 1, 2, 80.0, error);
    ss_matrix_set(matrix, 1, 3, 90.0, error);
    ss_matrix_set(matrix, 1, 4, 100.0, error);

    ss_matrix_set(matrix, 2, 0, 110.0, error);
    ss_matrix_set(matrix, 2, 1, 120.0, error);
    ss_matrix_set(matrix, 2, 2, 130.0, error);
    ss_matrix_set(matrix, 2, 3, 140.0, error);
    ss_matrix_set(matrix, 2, 4, 150.0, error);

    ss_matrix_get_dimensions(matrix, &rows, &columns, error);

    for (row =0; row < rows; ++row) {
        for (column = 0; column < columns; ++column) {
            value = ss_matrix_get(matrix, row, column, error);
            printf("%5.1f ", value);
        }
        printf("\n");
    }

    ss_matrix_destroy(matrix, error);
    ss_error_destroy(error);
    return 0;
}