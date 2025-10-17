#include<ss.h>
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
//        fprintf(gnuplot, "set logscale y\n");
        fprintf(gnuplot, "set grid\n");
//        fprintf(gnuplot, "set yrange [1e-1:1e+25]\n");
//        fprintf(gnuplot, "set xrange [0:%d]\n", 25000000);  // e.g. 2100000
        fprintf(gnuplot, "set style line 1 lc rgb '#0072BD' lt 1 lw 2 pt 7 ps 1.5\n");
        fprintf(gnuplot, "plot 'error_data.txt' using 1:2 with lines ls 1 title 'Error'\n");
        fflush(gnuplot);
    }
    // Append current iteration and error to data file
    if (i == 1 || i % 100 == 0) {
        fprintf(data_file, "%u %f\n", i, error);
        fflush(data_file); // ensure it's written immediately
    }
    // Refresh plot
    if (i % 1000 == 0) {
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
    model = ss_linear_regression_batch_gradient_descent(x, y, model_vector, number_of_iterations, learning_rate, callback_function, error);
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