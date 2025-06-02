#include<ss.h>
#include<stdio.h>

int main() {
    ss_vector *vector;
    ss_error *error;
    error = ss_error_create_new();
    vector = ss_vector_create_new(5, error);

    ss_vector_set(vector, 0, 10.0, error);
    ss_vector_set(vector, 1, 20.0, error);
    ss_vector_set(vector, 2, 30.0, error);
    ss_vector_set(vector, 3, 40.0, error);
    ss_vector_set(vector, 4, 50.0, error);

    ss_vector_write(vector, "vector.data", error);
    ss_vector_destroy(vector, error);
    vector = ss_vector_read("vector.data", error);

    int size = ss_vector_get_size(vector, error);
    for (int i = 0; i < size; ++i) {
        printf("%f\n", ss_vector_get(vector, i, error));
    }
    ss_vector_destroy(vector, error);
    ss_error_destroy(error);
    return 0;
}