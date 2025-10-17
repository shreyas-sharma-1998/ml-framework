#ifndef __SS__LINEAR__REGRESSION__H__
#define __SS__LINEAR__REGRESSION__H__

#include<stdio.h>
#include<stdint.h>
#include<ss_error.h>
#include<ss_matrix.h>
#include<ss_vector.h>
#include<ss_math.h>
#include<ss_ml_math.h>
#include<ss_data_provider.h>

ss_vector * ss_linear_regression_batch_gradient_descent(const ss_matrix *x_matrix,
    const ss_vector *y_vector,
    const ss_vector *model_vector_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double , ss_vector *),
    ss_error *error);

ss_vector * ss_linear_regression_stochastic_gradient_descent(ss_data_provider *data_provider,
    const ss_vector *model_vector_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double , ss_vector *),
    ss_error *error);

ss_vector *ss_logistic_regression_batch_gradient_descent(const ss_matrix *x_matrix,
    const ss_vector *y_vector,
    const ss_vector *theta_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double, ss_vector *),
    ss_error *error);

#endif