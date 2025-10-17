#include<ss.h>
#include<private_ss.h>
#include<stdio.h>
#include<stdlib.h>

ss_vector * ss_linear_regression_predict(const ss_matrix *x_matrix, const ss_vector *model_vector, ss_error *error) {
    ss_matrix *X_matrix = NULL;
    ss_vector *Y_HAT_vector = NULL;
    char error_message[4096];
    uint32_t x_matrix_rows, x_matrix_columns;
    uint32_t model_vector_size;

    if (error) ss_clear_error(error);

    if (x_matrix == NULL) {
        ss_set_error(error, "Null Pointer argument (1)", SS_NULL_POINTER);
        goto catch_error;
    }
    if (model_vector == NULL) {
        ss_set_error(error, "Null Pointer argument (2)", SS_NULL_POINTER);
        goto catch_error;
    }

    ss_matrix_get_dimensions(x_matrix, &x_matrix_rows, &x_matrix_columns, error);
    model_vector_size = ss_vector_get_size(model_vector, error);
    if (x_matrix_columns + 1 != model_vector_size) {
        sprintf(error_message, "Invalid model size (%u) required (%u)", model_vector_size, x_matrix_columns + 1);
        ss_set_error(error, error_message, SS_INVALID_MODEL_SIZE);
        goto catch_error;
    }
    X_matrix = ss_matrix_create_new(x_matrix_rows, x_matrix_columns + 1, error);
    if (X_matrix == NULL) goto catch_error;
    ss_matrix_fill(X_matrix, 0, 0, x_matrix_rows - 1, 0, 1.0, NULL);
    ss_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);
    Y_HAT_vector = ss_multiply_matrix_with_vector(X_matrix, model_vector, NULL, error);
    if (Y_HAT_vector == NULL) goto catch_error;
    goto prepare_for_exit;
    catch_error:
        // nothing to do
    prepare_for_exit:
        ss_matrix_destroy(X_matrix, NULL);
    return Y_HAT_vector;
}

// the creator of the main (as of date) is writing it to test the accuracy
int main(int c, char *v[]) {
    char error_message[4096];
    ss_error *error;
    ss_matrix *dataset = NULL;
    uint32_t dataset_rows, dataset_columns;
    ss_matrix *x = NULL;
    uint32_t x_rows, x_columns;
    ss_vector *model = NULL;
    ss_vector *y = NULL;
    ss_vector *y_hat=NULL;

    double r2score;

    char *dataset_filename;
    char *model_filename = NULL;

    if (c != 3) {
        printf("Usage : %s dataset_name model_name\n", v[0]);
        return 0;
    }

    dataset_filename = v[1];
    model_filename = v[4];

    //init error
    error = ss_error_create_new();
    if (error == NULL) {
        printf("Low Memory\n");
        return 0;
    }

    // load dataset into a matrix
    dataset = ss_matrix_read_csv(dataset_filename, 0, error);
    ss_on_error_throw(error, err);

    model = ss_vector_read_csv(model_filename, 0, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(dataset, &dataset_rows, &dataset_columns, error);

    x = ss_matrix_create_new(dataset_rows, dataset_columns - 1, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(x, &x_rows, &x_columns, error);

    ss_matrix_copy(x,0,0,dataset,0,0,dataset_rows-1,dataset_columns-2,error);

    y = ss_matrix_get_vector(dataset, 0 , dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);
    ss_matrix_destroy(dataset, error);
    y_hat = ss_linear_regression_predict(x, model, error);
    ss_on_error_throw(error, err);

    r2score = ss_r2_score(y, y_hat, error);
    printf("R2Score : %lf\n", r2score);

    goto exit_program;
    err:
        ss_get_error(error_message, 4096, error);
        printf("Error : %s\n", error_message);
    exit_program:
        ss_matrix_destroy(dataset, error);
        ss_matrix_destroy(x, error);
        ss_vector_destroy(y, error);
        ss_vector_destroy(model, error);
        ss_vector_destroy(y_hat, error);
        ss_error_destroy(error);
        return 0;
}