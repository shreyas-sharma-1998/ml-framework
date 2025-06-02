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

    int size = ss_vector_get_size(vector, error);
    for (int i = 0; i < size; ++i) {
        printf("%f\n", ss_vector_get(vector, i, error));
    }
    return 0;
}