#ifndef __SS__MATRIX__H__
#define __SS__MATRIX__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>
#include<ss_vector.h>

typedef struct __ss_matrix ss_matrix;

ss_matrix * ss_matrix_create_new(uint32_t rows, uint32_t columns, ss_error *error);
void __ss_matrix_destroy(ss_matrix **matrix, ss_error *error);

#define ss_matrix_destroy(matrix, error) __ss_matrix_destroy(&matrix, error);

void ss_matrix_set(ss_matrix *matrix, uint32_t row, uint32_t column, double value, ss_error *error);
double ss_matrix_get(const ss_matrix *matrix, uint32_t row, uint32_t column, ss_error *error);

void ss_matrix_get_dimensions(const ss_matrix *matrix, uint32_t *rows, uint32_t *columns, ss_error *error);
uint32_t ss_matrix_get_row_count(const ss_matrix *matrix, ss_error *error);
uint32_t ss_matrix_get_column_count(const ss_matrix *matrix, ss_error *error);

void ss_matrix_write(const ss_matrix *matrix, const char * filename, ss_error *error);
ss_matrix * ss_matrix_read(const char * filename, ss_error *error);
ss_matrix * ss_matrix_read_csv(const char * filename, uint8_t skip_lines, ss_error *error);
void ss_matrix_write_csv(const ss_matrix *matrix, const char * filename, ss_error *error);

void ss_matrix_transpose(ss_matrix *matrix, ss_error *error);
ss_matrix * ss_matrix_transpose_create_new(const ss_matrix *matrix, ss_error *error);

void ss_matrix_fill(ss_matrix *input_matrix, uint32_t start_row_index, uint32_t start_column_index, uint32_t end_row_index, uint32_t end_column_index, double fill_value, ss_error *error);

void ss_matrix_copy(ss_matrix *target, uint32_t target_start_row_index, uint32_t target_start_column_index, const ss_matrix *source, uint32_t source_start_row_index, uint32_t source_start_column_index, uint32_t source_end_row_index, uint32_t source_end_column_index, ss_error *error);

ss_vector * ss_matrix_get_vector(const ss_matrix *source, uint32_t source_start_row_index, uint32_t source_start_column_index, uint32_t source_end_row_index, uint32_t source_end_column_index, ss_error *error);

void ss_matrix_print(FILE *f, const ss_matrix *matrix, ss_error *error);

#endif