#ifndef __SS__VECTOR__H__
#define __SS__VECTOR__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>

typedef struct __ss_vector ss_vector;

ss_vector * ss_vector_create_new(uint32_t size, ss_error *error);
void __ss_vector_destroy(ss_vector **vector, ss_error *error);

#define ss_vector_destroy(vector, error) __ss_vector_destroy(&vector, error);

void ss_vector_set(ss_vector *vector, uint32_t index, double value, ss_error *error);
double ss_vector_get(const ss_vector *vector, uint32_t index, ss_error *error);

uint32_t ss_vector_get_size(const ss_vector *vector, ss_error *error);

void ss_vector_write(const ss_vector *vector, const char * filename, ss_error *error);
ss_vector * ss_vector_read(const char * filename, ss_error *error);
ss_vector * ss_vector_read_csv(const char * filename, uint8_t skip_lines, ss_error *error);

void ss_vector_write_csv(const ss_vector *vector, const char * filename, ss_error *error);

void ss_vector_fill_random(ss_vector *vector, ss_error *error);

void ss_vector_print(FILE *f, const ss_vector *vector, ss_error *error);

ss_vector * ss_vector_transpose(const ss_vector *vector, ss_vector *transposed_vector_container, ss_error *error);

#endif