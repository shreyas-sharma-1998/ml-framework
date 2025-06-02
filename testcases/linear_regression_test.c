#include<ppp.h>
#include<stdio.h>
#include<stdlib.h>

int main(int c, char *v[]) {
    char error_message[4096];
    ss_error *error;
    ss_matrix *dataset = NULL;
    uint32_t dataset_rows, dataset_columns;
    ss_matrix *input_matrix = NULL;
    uint32_t input_matrix_rows, input_matrix_columns;
    ss_vector *model = NULL;
    ss_vector *y = NULL;
    ss_vector *y_hat = NULL;
    double mean_squared_error;
    uint32_t model_size;
    uint64_t number_of_iterations;
    uint64_t i;
    char *filename;

    filename = v[1];
    number_of_iterations = atol(v[2]);

    //init error
    error = ss_error_create_new();
    if (error == NULL) {
        printf("Low Memory\n");
        return 0;
    }
    // load dataset into a matrix
    dataset = ss_matrix_read_csv(filename, 0, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(dataset, &dataset_rows, &dataset_columns, error);

    input_matrix = ss_matrix_create_new(dataset_rows, dataset_columns, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(input_matrix, &input_matrix_rows, &input_matrix_columns, error);
    ss_matrix_fill(input_matrix, 0, 0, input_matrix_rows - 1, 0, 1.0, error);
    ss_matrix_copy(input_matrix,0,1,dataset,0,0,dataset_rows-1,dataset_columns-2,error);
    y = ss_matrix_get_vector(dataset, 0 , dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);
    ss_matrix_destroy(dataset, error);

    // create model vector
    model_size = input_matrix_columns;
    model = ss_vector_create_new(model_size, error);
    ss_on_error_throw(error, err);
    ss_vector_fill_random(model, error);
    // training starts (logic to fit line)
    for (i=1; i <= number_of_iterations; ++i) {
        y_hat = ss_multiply_matrix_with_vector(input_matrix, model, error);
        mean_squared_error = ss_mean_squared_error(y, y_hat, error);
        printf("MSE : %20.12lf\n", mean_squared_error);
        ss_vector_destroy(y_hat, error);
        //update model
    }
    ss_vector_write_csv(model, "model.csv", error);
    goto exit_program;
    err:
        ss_get_error(error_message, 4096, error);
        printf("Error : %s\n", error_message);
    exit_program:
        ss_matrix_destroy(dataset, error);
        ss_matrix_destroy(input_matrix, error);
        ss_vector_destroy(y, error);
        ss_vector_destroy(y_hat, error);
        ss_vector_destroy(model, error);
        ss_error_destroy(error);
        return 0;
}