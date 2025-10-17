#include<ss.h>
#include<private_ss.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

typedef struct __ss_data_provider{
    char *filename;
    FILE *file;
    uint32_t number_of_columns;
    uint8_t number_of_lines_in_header;
    provider_func_ptr ptr_to_provider_function;
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

FILE * ss_data_provider_get_file_pointer(const ss_data_provider *provider, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return NULL;
    }
    return provider -> file;
}

const char * ss_data_provider_get_filename(const ss_data_provider *provider, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return NULL;
    }
    return provider -> filename;
}

uint8_t ss_data_provider_get_number_of_lines_in_header(const ss_data_provider *provider, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return 0;
    }
    return provider -> number_of_lines_in_header;
}

void ss_data_provider_set_file_pointer(ss_data_provider *provider, FILE *file, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return;
    }
    if (file == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return;
    }
    provider -> file = file;
}

provider_func_ptr ss_data_provider_get_provider_function(const ss_data_provider *provider, ss_error *error) {

    if (error) ss_clear_error(error);
    if (provider == NULL) {
        ss_set_error(error, "Null Pointer argument", SS_NULL_POINTER);
        return NULL;
    }
    return provider -> ptr_to_provider_function;
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
