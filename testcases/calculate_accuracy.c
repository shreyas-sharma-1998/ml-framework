#include<stdio.h>
#include<ss.h>

double getR2Score(ss_matrix *input_matrix, ss_vector *y_actual, ss_vector *model, ss_error *error) {
    char error_message[4096];
    ss_vector *y_hat;
    y_hat = ss_multiply_matrix_with_vector(input_matrix, model, NULL, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error while multiplying input with model : %s", error_message);
        return 0.0;
    }
    ss_vector *diff1, *diff2, *diff1t, *diff2t;
    diff1 = ss_subtract_vector(y_actual, y_hat, NULL, error);
    diff1t = ss_vector_transpose(diff1, NULL, error);
    double n = ss_multiply_vector_get_scalar(diff1t, diff1, error);
    double y_mean = ss_mean_vector(y_actual, error);
    diff2 = ss_subtract_scalar_from_vector(y_actual, y_mean, NULL, error);
    diff2t = ss_vector_transpose(diff2, NULL, error);
    double d = ss_multiply_vector_get_scalar(diff2t, diff2, error);
    return (double)1 - (n/d);
}

int main(int c, char *vv[]) {

    char *test_file, *model_file;
    ss_error *error;
    char error_message[4096];
    ss_matrix *test_dataset = NULL, *input_matrix = NULL;
    ss_vector *model_vector = NULL;
    ss_vector *y = NULL;
    uint32_t test_dataset_rows, test_dataset_columns;
    if (c != 3) {
        printf("Usage:  calculate_accuracy test_file_name model_file_name\n");
        return 0;
    }
    test_file = vv[1];
    model_file = vv[2];
    error = ss_error_create_new();
    test_dataset = ss_matrix_read_csv(test_file, 0, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error while reading test file : %s", error_message);
        return 0;
    }
    ss_matrix_get_dimensions(test_dataset, &test_dataset_rows, &test_dataset_columns, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error : %s", error_message);
        return 0;
    }
    input_matrix = ss_matrix_create_new(test_dataset_rows, test_dataset_columns, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error : %s", error_message);
        return 0;
    }
    ss_matrix_fill(input_matrix,0, 0, test_dataset_rows - 1, 0, 1.0, error);
    ss_matrix_copy(input_matrix, 0, 1, test_dataset, 0, 0, test_dataset_rows - 1, test_dataset_columns - 2, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error : %s", error_message);
        return 0;
    }
    y = ss_matrix_get_vector(test_dataset, 0, test_dataset_columns - 1, test_dataset_rows - 1, test_dataset_columns - 1, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error : %s", error_message);
        return 0;
    }
    model_vector = ss_vector_read_csv(model_file, 0, error);
    if (ss_has_error(error)) {
        ss_get_error(error_message, 4096, error);
        printf("Error while reading model file : %s", error_message);
        return 0;
    }
    double accuracy = getR2Score(input_matrix, y, model_vector, error);
    printf("Accuracy : %f\n", accuracy);
    return 0;
}