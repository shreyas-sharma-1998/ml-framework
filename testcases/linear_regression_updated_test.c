#include<ss.h>
#include<private_ss.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <unistd.h>
#include <stdint.h>

ss_vector * ss_linear_regression_fit(const ss_matrix *x_matrix, const ss_vector *y_vector, const ss_vector *model_vector_start, uint64_t iterations, double learning_rate, uint8_t (*on_iteration_completed)(uint32_t, double , ss_vector *), ss_error *error) {
    ss_matrix *X_matrix = NULL;
    ss_matrix *XT_matrix = NULL;
    ss_vector *Y_HAT_vector = NULL;
    ss_vector *E_vector = NULL;
    ss_vector *model_vector = NULL;
    ss_vector *ET_vector = NULL;
    ss_vector *ETE_vector = NULL;
    ss_vector *XTE_vector = NULL;
    double error_value;
    uint64_t i;
    char error_message[4096];
    uint32_t x_matrix_rows, x_matrix_columns;
    uint32_t X_matrix_rows, X_matrix_columns;
    uint32_t y_vector_size;
    uint32_t model_vector_size;

    if (error) ss_clear_error(error);

    if (x_matrix == NULL) {
        ss_set_error(error, "Null Pointer argument (1)", SS_NULL_POINTER);
        goto catch_error;
    }
    if (y_vector == NULL) {
        ss_set_error(error, "Null Pointer argument (2)", SS_NULL_POINTER);
        goto catch_error;
    }
    if (iterations == 0) {
        ss_set_error(error, "Cannot generate model with 0 iterations", SS_INVALID_NUMBER_OF_ITERATIONS);
        goto catch_error;
    }
    ss_matrix_get_dimensions(x_matrix, &x_matrix_rows, &x_matrix_columns, error);
    y_vector_size = ss_vector_get_size(y_vector, error);
    if (y_vector_size != x_matrix_rows) {
        sprintf(error_message, "y size (%u) does not match x rows (%u)", y_vector_size, x_matrix_rows);
        ss_set_error(error, error_message, SS_INPUT_AND_TARGET_OUTPUT_SIZE_MISMATCH);
        goto catch_error;
    }
    X_matrix_rows = x_matrix_rows;
    X_matrix_columns = x_matrix_columns + 1;
    X_matrix = ss_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
    if (X_matrix == NULL) goto catch_error;
    ss_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);
    ss_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);
    XT_matrix = ss_matrix_transpose_create_new(X_matrix, error);
    if (XT_matrix == NULL) goto catch_error;
    Y_HAT_vector = ss_vector_create_new(y_vector_size, error);
    if (Y_HAT_vector == NULL) goto catch_error;
    E_vector = ss_vector_create_new(y_vector_size, error);
    if (E_vector == NULL) goto catch_error;
    ET_vector = ss_vector_create_new(y_vector_size, error);
    if (ET_vector == NULL) goto catch_error;
    ETE_vector = ss_vector_create_new(1, error);
    if (ETE_vector == NULL) goto catch_error;
    model_vector_size = X_matrix_columns;
    if (model_vector_start != NULL) {
        model_vector = ss_vector_transpose(model_vector_start, NULL, error);
        if (model_vector == NULL) goto catch_error;
    } else {
        model_vector = ss_vector_create_new(model_vector_size, error);
        if (model_vector == NULL) goto catch_error;
        ss_vector_fill_random(model_vector, error);
    }
    printf("Starting Model : \n");
    for (int i = 0; i < model_vector_size; ++i) {
        printf("%f ", ss_vector_get(model_vector, i, error));
    }
    printf("\n");
    XTE_vector = ss_vector_create_new(model_vector_size, error);
    if (XTE_vector == NULL) goto catch_error;
    for ( i = 1; i <= iterations; ++i) {
        Y_HAT_vector = ss_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);
        E_vector = ss_subtract_vector(Y_HAT_vector, y_vector, E_vector, error);
        ET_vector = ss_vector_transpose(E_vector, ET_vector, error);
        error_value = ss_multiply_vector_get_scalar(ET_vector, E_vector, error);
        error_value = error_value / (2*X_matrix_rows);

        if (on_iteration_completed && !on_iteration_completed(i, error_value, model_vector)) {
            break;
        }

        XTE_vector = ss_multiply_matrix_with_vector(XT_matrix, E_vector, XTE_vector, error);
        XTE_vector = ss_multiply_vector_with_scalar(XTE_vector, (1.0 / X_matrix_rows)*learning_rate, XTE_vector, error);
        model_vector = ss_subtract_vector(model_vector, XTE_vector, model_vector, error);
    }
    goto prepare_for_exit;
    catch_error:
        ss_vector_destroy(model_vector, NULL);
    prepare_for_exit:
        ss_matrix_destroy(X_matrix, NULL);
        ss_matrix_destroy(XT_matrix, NULL);
        ss_vector_destroy(Y_HAT_vector, NULL);
        ss_vector_destroy(E_vector, NULL);
        ss_vector_destroy(ET_vector, NULL);
        ss_vector_destroy(XTE_vector, NULL);
    return model_vector;
}

static FILE *data_file = NULL;
static FILE *gnuplot = NULL;

uint8_t callback_function(uint32_t i, double error, ss_vector *model) {
    printf("Iteration %u, error %40.20f\n", i, error);
    return 1;
    if (i == 1) {
        // Open the data file for writing error values
        data_file = fopen("error_data.txt", "w");
        if (!data_file) {
            perror("Failed to open error_data.txt");
            return 0;
        }
        // Open gnuplot pipe
        gnuplot = popen("gnuplot -persistent", "w");
        if (!gnuplot) {
            perror("Failed to open gnuplot");
            return 0;
        }
        // Set gnuplot settings
        fprintf(gnuplot, "set title 'Training Error over Iterations'\n");
        fprintf(gnuplot, "set xlabel 'Iteration'\n");
        fprintf(gnuplot, "set ylabel 'Error'\n");
        fprintf(gnuplot, "set logscale y\n");
        fprintf(gnuplot, "set grid\n");
        fprintf(gnuplot, "plot 'error_data.txt' using 1:2 with lines title 'Error'\n");
        fflush(gnuplot);
    }
    // Append current iteration and error to data file
    if (i == 1 || i % 10000 == 0) {
        fprintf(data_file, "%u %f\n", i, error);
        fflush(data_file); // ensure it's written immediately
    }
    // Refresh plot
    if (i % 100000 == 0) {
        printf("Iteration %u, error %40.20f\n", i, error);
        fprintf(gnuplot, "replot\n");
        fflush(gnuplot);   // flush the command to gnuplot
    }
    return 1;
}

int main(int c, char *v[]) {
    time_t start_time, end_time;
    double elapsed;
    char error_message[4096];
    ss_error *error;
    ss_matrix *dataset = NULL;
    uint32_t dataset_rows, dataset_columns;
    ss_matrix *x = NULL;
    uint32_t x_rows, x_columns;
    ss_vector *model = NULL;
    ss_vector *y = NULL;
    uint64_t number_of_iterations;
    double learning_rate;

    char *filename;
    char *model_filename = NULL;
    ss_vector *model_vector = NULL;

    if (c != 4 && c != 5) {
        printf("Usage : %s datafile_name number_of_iterations learning_rate\n", v[0]);
        return 0;
    }

    // Record start time
    time(&start_time);

    filename = v[1];
    number_of_iterations = atol(v[2]);
    learning_rate = atof(v[3]);
    if (c == 5) {
        model_filename = v[4];
    }
    //init error
    error = ss_error_create_new();
    if (error == NULL) {
        printf("Low Memory\n");
        return 0;
    }

    if (model_filename != NULL) {
        model_vector = ss_vector_read_csv(model_filename, 0, error);
        printf("Read model vector of size : %u \n", ss_vector_get_size(model_vector, NULL));
    }

    // load dataset into a matrix
    dataset = ss_matrix_read_csv(filename, 0, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(dataset, &dataset_rows, &dataset_columns, error);

    x = ss_matrix_create_new(dataset_rows, dataset_columns - 1, error);
    ss_on_error_throw(error, err);

    ss_matrix_get_dimensions(x, &x_rows, &x_columns, error);

    ss_matrix_copy(x,0,0,dataset,0,0,dataset_rows-1,dataset_columns-2,error);

    y = ss_matrix_get_vector(dataset, 0 , dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);
    ss_matrix_destroy(dataset, error);
    model = ss_linear_regression_fit(x, y, model_vector, number_of_iterations, learning_rate, callback_function, error);
    ss_on_error_throw(error, err);
    ss_vector_write_csv(model, "model.csv", error);
    goto exit_program;
    err:
        ss_get_error(error_message, 4096, error);
        printf("Error : %s\n", error_message);
    exit_program:
        ss_matrix_destroy(dataset, error);
        ss_matrix_destroy(x, error);
        ss_vector_destroy(y, error);
        ss_vector_destroy(model, error);
        ss_error_destroy(error);
        //sleep(5);
        time(&end_time);
        elapsed = difftime(end_time, start_time);
        printf("Task completed.\n");
        printf("Elapsed time: %.2f seconds\n", elapsed);
        return 0;
}