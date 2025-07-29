#ifndef __PRIVATE__SS__VECTOR__H__
#define __PRIVATE__SS__VECTOR__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>

typedef struct __ss_vector ss_vector;
typedef struct __ss_matrix ss_matrix;

ss_matrix * ss_vector_get_matrix(const ss_vector *vector, ss_error *error);

void ss_vector_set_matrix(ss_vector *vector, ss_matrix *matrix, ss_error *error);

#endif