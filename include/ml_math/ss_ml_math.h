#ifndef __SS__ML__MATH__H__
#define __SS__ML__MATH__H__

#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>
#include<ss_matrix.h>
#include<ss_vector.h>
#include<ss_math.h>

double ss_mean_squared_error(ss_vector *left_vector, ss_vector *right_vector, ss_error *error);

double ss_r2_score(const ss_vector *y_vector, const ss_vector *y_hat_vector, ss_error *error);

#endif