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
    matrix = ss_matrix_read_csv("matrix.csv", 1, error);

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