#include <ss.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

uint8_t callback_function(uint32_t i, double error, ss_vector *model) {
    printf("Iteration %u, Cross-Entropy Loss = %f\n", i, error);
    printf("Model\n");
    ss_vector_print(stdout, model, NULL);
    return 1; // continue training
}

int main(int c, char *v[]) {
    char error_message[4096];
    ss_error *error = NULL;
    ss_matrix *dataset = NULL;
    ss_matrix *x = NULL;
    ss_vector *y = NULL;
    ss_vector *model = NULL;
    ss_vector *model_vector = NULL;
    uint32_t dataset_rows, dataset_columns;
    uint64_t number_of_iterations;
    double learning_rate;

    char *filename = NULL;
    char *model_filename = NULL;

    if (c != 4 && c != 5) {
        printf("Usage : %s datafile_name number_of_iterations learning_rate [model_file]\n", v[0]);
        return 0;
    }

    // Parse command line args
    filename = v[1];
    number_of_iterations = atol(v[2]);
    learning_rate = atof(v[3]);
    if (c == 5) model_filename = v[4];

    // Initialize error handler
    error = ss_error_create_new();
    if (!error) {
        printf("Low Memory\n");
        return 0;
    }

    // Optional model file
    if (model_filename != NULL) {
        model_vector = ss_vector_read_csv(model_filename, 0, error);
        printf("Read model vector of size: %u\n", ss_vector_get_size(model_vector, NULL));
    }

    // Load CSV dataset
    dataset = ss_matrix_read_csv(filename, 0, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(dataset, &dataset_rows, &dataset_columns, error);

    x = ss_matrix_create_new(dataset_rows, dataset_columns - 1, error);
    ss_matrix_copy(x, 0, 0, dataset, 0, 0, dataset_rows - 1, dataset_columns - 2, error);

    y = ss_matrix_get_vector(dataset, 0, dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);
    ss_matrix_destroy(dataset, NULL);

    model = ss_logistic_regression_batch_gradient_descent(
        x, y, model_vector, number_of_iterations, learning_rate, callback_function, error
    );
    ss_on_error_throw(error, err);

    // Save model to CSV
    ss_vector_write_csv(model, "logistic_model.csv", error);
    goto exit_program;

err:
    ss_get_error(error_message, 4096, error);
    printf("ERROR: %s\n", error_message);

exit_program:
    ss_matrix_destroy(x, NULL);
    ss_vector_destroy(y, NULL);
    ss_vector_destroy(model, NULL);
    ss_error_destroy(error);

    return 0;
}