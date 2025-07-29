#include<ss.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

int main(int cc, char *vv[]) {
    char *filename;
    uint32_t r,c;
    uint32_t rows, columns;
    ss_matrix *matrix;
    int n, d;
    char error_message[4096];
    double value;
    ss_error *error;
    if (cc!=4) {
        printf("Usage : dataset_generator filename rows columns\n");
        return 0;
    }
    filename = vv[1];
    rows = atol(vv[2]);
    columns = atol(vv[3]);
    error = ss_error_create_new();
    matrix = ss_matrix_create_new(rows, columns, error);
    if (matrix == NULL) {
        printf("Low memory, could not create %s\n", filename);
        return 0;
    }
    srand(time(0));
    for (r = 0; r < rows; ++r) {
        for (c = 0; c < columns; ++c) {
            n = rand();
            d = rand();
            if (d == 0) d = 3;
            value = (double)(n/d);
            ss_matrix_set(matrix, r,c, value,error);
        }
    }
    ss_matrix_write_csv(matrix, filename, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Unable to create %s\n", error_message);
    } else {
        printf("%s created\n", filename);
    }
    ss_matrix_destroy(matrix, error);
    ss_error_destroy(error);
    return 0;
}