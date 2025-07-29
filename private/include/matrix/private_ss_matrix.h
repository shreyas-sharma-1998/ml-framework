#ifndef __PRIVATE__SS__MATRIX__H__
#define __PRIVATE__SS__MATRIX__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>

typedef struct __ss_matrix ss_matrix;

double * ss_matrix_get_data(const ss_matrix *matrix, ss_error *error);

#endif