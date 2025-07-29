#include<ss.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main() {
    ss_matrix *matrix;
    ss_vector *vector, *product;
    ss_error *error;
    uint32_t r,c, rows, columns;
    error = ss_error_create_new();
    srand(time(0));
    matrix = ss_matrix_create_new(5, 3, error);
    for (r = 0; r < 5; ++r) {
        for (c = 0; c < 3; ++c) {
            double value = rand() % 10;
            ss_matrix_set(matrix, r, c, value, error);
        }
    }
    vector = ss_vector_create_new(3, error);
    for (r = 0; r < 3; ++r) {
            double value = rand() % 10;
            ss_vector_set(vector, r, value, error);
    }
    product = ss_multiply_matrix_with_vector(matrix, vector, error);
    printf("Matrix \n");
    ss_matrix_print(stdout, matrix, error);
    printf("Vector \n");
    ss_vector_print(stdout, vector, error);
    printf("Product \n");
    ss_vector_print(stdout, product, error);
    return 0;
}