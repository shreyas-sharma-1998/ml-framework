#ifndef __SS__MATH__H__
#define __SS__MATH__H__
#include<stdio.h>
#include<stdint.h>
#include<ss_matrix.h>
#include<ss_vector.h>
#include<ss_error.h>

ss_matrix * ss_multiply_matrix(ss_matrix *left_matrix, ss_matrix *right_matrix, ss_matrix *product_matrix_container, ss_error *error);

ss_vector * ss_multiply_matrix_with_vector(ss_matrix *matrix, ss_vector *vector, ss_vector *product_vector_container, ss_error *error);

ss_vector * ss_subtract_vector(const ss_vector *left_vector, const ss_vector *right_vector, ss_vector *difference_vector_container, ss_error *error);

double ss_multiply_vector_get_scalar(const ss_vector *left_vector, const ss_vector *right_vector, ss_error *error);

ss_vector * ss_multiply_vector_with_scalar(const ss_vector *left_vector, double right_value, ss_vector *product_vector_container, ss_error *error);

double ss_sum_vector(const ss_vector *vector, ss_error *error);

double ss_mean_vector(const ss_vector *vector, ss_error *error);

ss_vector * ss_subtract_scalar_from_vector(const ss_vector *left_vector, double right_value, ss_vector *subtract_vector_container, ss_error *error);

#endif