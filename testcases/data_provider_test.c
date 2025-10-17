#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ss.h>
#include<private_ss.h>
//date provider function

typedef struct __ss_data_provider ss_data_provider;

typedef struct __ss_data_provider{
    char *filename;
    FILE *file;
    uint32_t number_of_columns;
    uint8_t number_of_lines_in_header;
    void (*ptr_to_provider_function)(ss_data_provider *, ss_matrix *, uint32_t *rows_provided, ss_error *);
} ss_data_provider;

ss_data_provider * ss_data_provider_create(const char *filename,
    uint8_t number_of_lines_in_header,
    void (*ptr_to_provider_function)(ss_data_provider *, ss_matrix *, uint32_t *rows_provided, ss_error *)
    , ss_error *error) {

    ss_data_provider *provider = NULL;
    if (error) ss_clear_error(error);

    if (filename == NULL) {
        ss_set_error(error, "Null Pointer argument 1", SS_NULL_POINTER);
        return NULL;
    }
    if (ptr_to_provider_function == NULL) {
        ss_set_error(error, "Null Pointer argument 3", SS_NULL_POINTER);
        return NULL;
    }

    // check existence of the file, if does not exist return null
    provider = (ss_data_provider *)malloc(sizeof(ss_data_provider));
    if (provider == NULL) {
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        return NULL;
    }
    provider -> number_of_lines_in_header = number_of_lines_in_header;
    provider -> ptr_to_provider_function = ptr_to_provider_function;
    provider -> filename = (char *)malloc(sizeof(char)*(strlen(filename)+1));
    if (provider -> filename == NULL) {
        free(provider);
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        return NULL;
    }
    strcpy(provider -> filename, filename);
    provider -> file = NULL;
    provider -> number_of_columns = 0;
    return provider;
}

void ss_data_provider_destroy(ss_data_provider *provider, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return;
    }
    if (provider -> filename) free(provider -> filename);
    if (provider -> file) fclose(provider -> file);
    free(provider);
}

uint32_t ss_data_provider_get_column_count(ss_data_provider *provider, ss_error *error) {
    int skip_lines;
    char error_message[4096];
    char m;
    uint32_t number_of_columns;
    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return 0;
    }
    if (provider -> number_of_columns == 0) {
        provider -> file = fopen(provider -> filename, "r");
        if (provider -> file == NULL) {
            sprintf(error_message, "Unable to read file %s", provider -> filename);
            ss_set_error(error, error_message, SS_FILE_READ_ERROR);
            return 0;
        }
        skip_lines = provider -> number_of_lines_in_header;
        while (skip_lines > 0) {
            m = fgetc(provider -> file);
            if (feof(provider -> file)) break;
            if (m == '\n') skip_lines--;
        }
        if (skip_lines > 0) {
            sprintf(error_message, "Invalid csv format of file %s", provider -> filename);
            ss_set_error(error, error_message, SS_CSV_FORMAT_ERROR);
            fclose(provider -> file);
            provider -> file = NULL;
            return 0;
        }
        number_of_columns = 0;
        while (1) {
            m = fgetc(provider -> file);
            if (m == '\n' || feof(provider -> file)) break;
            if (m == ',') number_of_columns++;
        }
        if (m != '\n') {
            sprintf(error_message, "Invalid csv format of file %s", provider -> filename);
            ss_set_error(error, error_message, SS_CSV_FORMAT_ERROR);
            fclose(provider -> file);
            provider -> file = NULL;
            return 0;
        }
        provider -> number_of_columns = number_of_columns + 1;
        fclose(provider -> file);
        provider -> file = NULL;
    }
    return provider -> number_of_columns;
}

void do_something_like_fit(ss_data_provider *provider, ss_error *error) {

    uint32_t j, c;
    double value;
    uint32_t rows_filled_by_provider;
    ss_matrix *matrix = NULL;
    uint32_t matrix_rows;
    uint32_t number_of_columns;
    if (error) ss_clear_error(error);

    number_of_columns = ss_data_provider_get_column_count(provider, error);
    matrix_rows = 10;
    matrix = ss_matrix_create_new(1, number_of_columns, error);
    while (1) {
        provider -> ptr_to_provider_function(provider, matrix, &rows_filled_by_provider, error);
        if (ss_has_error(error)) break;
        printf("Rows fetched : %u\n", rows_filled_by_provider);
        if (rows_filled_by_provider == 0) break;
        for (j = 0; j < rows_filled_by_provider; ++j) {
            for (c = 0; c < number_of_columns; ++c) {
                value = ss_matrix_get(matrix, j, c, error);
                printf("%f ", value);
            }
            printf("\n");
        }
        if (rows_filled_by_provider < matrix_rows) break;
    }
    ss_matrix_destroy(matrix, error);
}


// Programmer is Amit
// 1) he knows the name of the dataset file
// 2) he is aware that the contents of the file cannot be loaded in DS, in one shot
// 3) he is designing a data provider that will provide data as asked for

void kalu(ss_data_provider *wrapper, ss_matrix *matrix, uint32_t *rows_provided, ss_error *error) {

     char m;
     char str[51];
     double value;
     uint32_t r, c;
     int i;
     int number_of_columns;
     int skip_lines;
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
     if (wrapper -> file == NULL) {
        wrapper -> file = fopen(wrapper -> filename, "r");
        if (wrapper -> file == NULL) {
            sprintf(error_message, "Unable to read file %s", wrapper -> filename);
            ss_set_error(error, error_message, SS_FILE_READ_ERROR);
            return;
        }
        skip_lines = wrapper -> number_of_lines_in_header;
        while (skip_lines > 0) {
            m = fgetc(wrapper -> file);
            if (feof(wrapper -> file)) break;
            if (m == '\n') skip_lines--;
        }
     }
     i = 0;
     r = 0;
     c = 0;
     while (r < matrix_rows) {
        m = fgetc(wrapper -> file);
        if (feof(wrapper -> file)) break;
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

int main() {
    ss_error *error;
    ss_data_provider *provider;
    error = ss_error_create_new();
    provider = ss_data_provider_create("train_data.csv", 0, kalu, error);
    if (ss_has_error(error)) {
        printf("Some issue\n");
        return 0;
    }
    do_something_like_fit(provider, error);
    if (ss_has_error(error)) {
        printf("Some issue\n");
        return 0;
    }
    ss_data_provider_destroy(provider, error);
    ss_error_destroy(error);
    return 0;
}