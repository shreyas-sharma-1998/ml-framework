#ifndef __SS__MATRIX__H__
#define __SS__MATRIX__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>

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

void ss_matrix_transpose(ss_matrix *matrix, ss_error *error);
ss_matrix * ss_matrix_transpose_create_new(const ss_matrix *matrix, ss_error *error);

void ss_matrix_print(FILE *f, const ss_matrix *matrix, ss_error *error);

#endif