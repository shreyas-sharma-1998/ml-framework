#include<ss.h>
#include<private_ss.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <unistd.h>
#include <stdint.h>

static FILE *data_file = NULL;
static FILE *gnuplot = NULL;

uint8_t callback_function(uint32_t i, double error, ss_vector *model) {
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
        fprintf(gnuplot, "set yrange [1e-1:1e+25]\n");
        fprintf(gnuplot, "set xrange [0:%d]\n", 25000000);
        fprintf(gnuplot, "plot 'error_data.txt' using 1:2 with lines lc rgb 'blue' title 'Raw Error', \\\n");
        fprintf(gnuplot, "'' using 1:2 smooth bezier with lines lw 2 lc rgb 'red' title 'Smoothed Error'\n");
        fflush(gnuplot);
    }

    // Append current iteration and error to data file
    if (i == 1 || i % 10000 == 0) {
        fprintf(data_file, "%u %f\n", i, error);
        fflush(data_file);
    }

    // Refresh plot
    if (i % 100000 == 0) {
        printf("Iteration %u, error %40.20f\n", i, error);
        fprintf(gnuplot, "replot\n");
        fflush(gnuplot);
    }
    return 1;
}

void kalu(ss_data_provider *wrapper, ss_matrix *matrix, uint32_t *rows_provided, ss_error *error) {

     char m;
     char str[51];
     double value;
     uint32_t r, c;
     int i;
     int number_of_columns;
     int skip_lines;
     FILE *file;
     char error_message[4096];
     uint32_t matrix_rows, matrix_columns;
     if (error) ss_clear_error(error);
     if (rows_provided) *rows_provided = 0;
     if (matrix == NULL) {
        ss_set_error(error, "Null pointer argument", SS_NULL_POINTER);
        return;
     }
     number_of_columns = ss_data_provider_get_column_count(wrapper, error);
     if (number_of_columns == 0) return;
     ss_matrix_get_dimensions(matrix, &matrix_rows, &matrix_columns, error);
     if (matrix_columns != number_of_columns) {
        sprintf(error_message, "Invalid matrix size (%ux%u), required (%ux%u)", matrix_rows, matrix_columns, matrix_rows, number_of_columns);
        ss_set_error(error, error_message, SS_INVALID_MATRIX_CONTAINER_DIMENSIONS);
        return;
     }
     file = ss_data_provider_get_file_pointer(wrapper, error);
     if (file == NULL) {
        file = fopen(ss_data_provider_get_filename(wrapper, error), "r");
        if (file == NULL) {
            sprintf(error_message, "Unable to read file %s", ss_data_provider_get_filename(wrapper, error));
            ss_set_error(error, error_message, SS_FILE_READ_ERROR);
            return;
        }
        skip_lines = ss_data_provider_get_number_of_lines_in_header(wrapper, error);
        while (skip_lines > 0) {
            m = fgetc(file);
            if (feof(file)) break;
            if (m == '\n') skip_lines--;
        }
        ss_data_provider_set_file_pointer(wrapper, file, error);
     }
     i = 0;
     r = 0;
     c = 0;
     while (r < matrix_rows) {
        m = fgetc(file);
        if (feof(file)) {
            rewind(file);
            continue;
        }
        if (m == ',' || m == '\n') {
            str[i] = '\0';
            i = 0;
            value = atof(str);
            ss_matrix_set(matrix, r, c, value, error);
            c++;
            if (c == matrix_columns) {
                r++;
                c = 0;
            }
        } else {
            str[i++] = m;
        }
     }
     if (rows_provided) *rows_provided = r;
}

int main(int c, char *v[]) {
    time_t start_time, end_time;
    double elapsed;
    char error_message[4096];
    ss_error *error;
    ss_matrix *dataset = NULL;
    ss_matrix *x = NULL;
    ss_vector *model = NULL;
    ss_vector *y = NULL;
    uint64_t number_of_iterations;
    double learning_rate;

    char *filename;
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
    //init error
    error = ss_error_create_new();
    if (error == NULL) {
        printf("Low Memory\n");
        return 0;
    }

    ss_data_provider *provider;
    provider = ss_data_provider_create(filename, 0, kalu, error);
    model = ss_linear_regression_stochastic_gradient_descent(provider, model_vector, number_of_iterations, learning_rate, callback_function, error);
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