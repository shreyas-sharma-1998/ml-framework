#include<ss.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main() {
    ss_matrix *left_matrix, *right_matrix, *product_matrix;
    ss_error *error;
    uint32_t r,c, rows, columns;
    error = ss_error_create_new();
    srand(time(0));
    left_matrix = ss_matrix_create_new(5, 3, error);
    right_matrix = ss_matrix_create_new(3, 5, error);
    for (r = 0; r < 5; ++r) {
        for (c = 0; c < 3; ++c) {
            double value = rand() % 10;
            ss_matrix_set(left_matrix, r, c, value, error);
        }
    }
    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 5; ++c) {
            double value = rand() % 10;
            ss_matrix_set(right_matrix, r, c, value, error);
        }
    }
    product_matrix = ss_multiply_matrix(left_matrix, right_matrix, error);
    printf("Left Matrix \n");
    ss_matrix_print(stdout, left_matrix, error);
    printf("Right Matrix \n");
    ss_matrix_print(stdout, right_matrix, error);
    printf("Product Matrix \n");
    ss_matrix_print(stdout, product_matrix, error);
    return 0;
}