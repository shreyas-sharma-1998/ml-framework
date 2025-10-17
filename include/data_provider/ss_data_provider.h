#ifndef __SS__DATA__PROVIDER__H__
#define __SS__DATA__PROVIDER__H__

#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>
#include<ss_matrix.h>
#include<ss_vector.h>
#include<ss_math.h>
#include<ss_ml_math.h>

typedef struct __ss_data_provider ss_data_provider;

typedef void (*provider_func_ptr)(ss_data_provider *, ss_matrix *, uint32_t *, ss_error *);

ss_data_provider * ss_data_provider_create(const char *filename,
    uint8_t number_of_lines_in_header,
    void (*ptr_to_provider_function)(ss_data_provider *, ss_matrix *, uint32_t *rows_provided, ss_error *)
    , ss_error *error);

void ss_data_provider_destroy(ss_data_provider *provider, ss_error *error);

uint32_t ss_data_provider_get_column_count(ss_data_provider *provider, ss_error *error);

FILE * ss_data_provider_get_file_pointer(const ss_data_provider *provider, ss_error *error);

void ss_data_provider_set_file_pointer(ss_data_provider *provider, FILE *file, ss_error *error);

provider_func_ptr ss_data_provider_get_provider_function(const ss_data_provider *provider, ss_error *error);

const char * ss_data_provider_get_filename(const ss_data_provider *provider, ss_error *error);

uint8_t ss_data_provider_get_number_of_lines_in_header(const ss_data_provider *provider, ss_error *error);


#endif