#include<ss.h>
#include<private_ss.h>
#include<stdint.h>

ss_vector * ss_linear_regression_batch_gradient_descent(const ss_matrix *x_matrix,
    const ss_vector *y_vector,
    const ss_vector *model_vector_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double , ss_vector *),
    ss_error *error) {

        ss_matrix *X_matrix = NULL;
        ss_matrix *XT_matrix = NULL;
        ss_vector *Y_HAT_vector = NULL;
        ss_vector *E_vector = NULL;
        ss_vector *model_vector = NULL;
        ss_vector *ET_vector = NULL;
        ss_vector *ETE_vector = NULL;
        ss_vector *XTE_vector = NULL;
        double error_value;
        uint64_t i;
        char error_message[4096];
        uint32_t x_matrix_rows, x_matrix_columns;
        uint32_t X_matrix_rows, X_matrix_columns;
        uint32_t y_vector_size;
        uint32_t model_vector_size;

        if (error) ss_clear_error(error);

        if (x_matrix == NULL) {
            ss_set_error(error, "Null Pointer argument (1)", SS_NULL_POINTER);
            goto catch_error;
        }
        if (y_vector == NULL) {
            ss_set_error(error, "Null Pointer argument (2)", SS_NULL_POINTER);
            goto catch_error;
        }
        if (iterations == 0) {
            ss_set_error(error, "Cannot generate model with 0 iterations", SS_INVALID_NUMBER_OF_ITERATIONS);
            goto catch_error;
        }
        ss_matrix_get_dimensions(x_matrix, &x_matrix_rows, &x_matrix_columns, error);
        y_vector_size = ss_vector_get_size(y_vector, error);
        if (y_vector_size != x_matrix_rows) {
            sprintf(error_message, "y size (%u) does not match x rows (%u)", y_vector_size, x_matrix_rows);
            ss_set_error(error, error_message, SS_INPUT_AND_TARGET_OUTPUT_SIZE_MISMATCH);
            goto catch_error;
        }
        X_matrix_rows = x_matrix_rows;
        X_matrix_columns = x_matrix_columns + 1;
        X_matrix = ss_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
        if (X_matrix == NULL) goto catch_error;
        ss_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);
        ss_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);
        XT_matrix = ss_matrix_transpose_create_new(X_matrix, error);
        if (XT_matrix == NULL) goto catch_error;
        Y_HAT_vector = ss_vector_create_new(y_vector_size, error);
        if (Y_HAT_vector == NULL) goto catch_error;
        E_vector = ss_vector_create_new(y_vector_size, error);
        if (E_vector == NULL) goto catch_error;
        ET_vector = ss_vector_create_new(y_vector_size, error);
        if (ET_vector == NULL) goto catch_error;
        ETE_vector = ss_vector_create_new(1, error);
        if (ETE_vector == NULL) goto catch_error;
        model_vector_size = X_matrix_columns;
        if (model_vector_start != NULL) {
            model_vector = ss_vector_transpose(model_vector_start, NULL, error);
            if (model_vector == NULL) goto catch_error;
        } else {
            model_vector = ss_vector_create_new(model_vector_size, error);
            if (model_vector == NULL) goto catch_error;
            ss_vector_fill_random(model_vector, -1, 1, error);
        }
        XTE_vector = ss_vector_create_new(model_vector_size, error);
        if (XTE_vector == NULL) goto catch_error;
        for ( i = 1; i <= iterations; ++i) {
            Y_HAT_vector = ss_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);
            E_vector = ss_subtract_vector(Y_HAT_vector, y_vector, E_vector, error);
            ET_vector = ss_vector_transpose(E_vector, ET_vector, error);
            error_value = ss_multiply_vector_get_scalar(ET_vector, E_vector, error);
            error_value = error_value / (2*X_matrix_rows);

            if (on_iteration_completed && !on_iteration_completed(i, error_value, model_vector)) {
                break;
            }

            XTE_vector = ss_multiply_matrix_with_vector(XT_matrix, E_vector, XTE_vector, error);
            XTE_vector = ss_multiply_vector_with_scalar(XTE_vector, (1.0 / X_matrix_rows)*learning_rate, XTE_vector, error);
            model_vector = ss_subtract_vector(model_vector, XTE_vector, model_vector, error);
        }
        goto prepare_for_exit;
        catch_error:
            ss_vector_destroy(model_vector, NULL);
        prepare_for_exit:
            ss_matrix_destroy(X_matrix, NULL);
            ss_matrix_destroy(XT_matrix, NULL);
            ss_vector_destroy(Y_HAT_vector, NULL);
            ss_vector_destroy(E_vector, NULL);
            ss_vector_destroy(ET_vector, NULL);
            ss_vector_destroy(XTE_vector, NULL);
        return model_vector;
    }

ss_vector * ss_linear_regression_stochastic_gradient_descent(ss_data_provider *data_provider,
    const ss_vector *model_vector_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double , ss_vector *),
    ss_error *error) {

        uint32_t rows_filled_by_provider;
        ss_matrix *dataset = NULL;
        char error_message[4096];
        error_message[0] = '\0';
        uint32_t number_of_columns;
        uint32_t dataset_rows, dataset_columns;
        ss_matrix *x_matrix = NULL;
        ss_vector *y = NULL;
        provider_func_ptr ptr_to_provider_function;
        ss_matrix *X_matrix = NULL;
        ss_matrix *XT_matrix = NULL;
        ss_vector *Y_HAT_vector = NULL;
        ss_vector *E_vector = NULL;
        ss_vector *model_vector = NULL;
        ss_vector *ET_vector = NULL;
        ss_vector *ETE_vector = NULL;
        ss_vector *XTE_vector = NULL;
        double error_value;
        uint32_t x_matrix_rows, x_matrix_columns;
        uint32_t X_matrix_rows, X_matrix_columns;
        uint32_t y_vector_size;
        uint32_t model_vector_size;
        if (error) ss_clear_error(error);

        if (data_provider == NULL) {
            ss_set_error(error, "Null Pointer argument (1)", SS_NULL_POINTER);
            goto catch_error;
        }
        if (iterations == 0) {
            ss_set_error(error, "Cannot generate model with 0 iterations", SS_INVALID_NUMBER_OF_ITERATIONS);
            goto catch_error;
        }

        number_of_columns = ss_data_provider_get_column_count(data_provider, error);
        dataset = ss_matrix_create_new(1, number_of_columns, error);
        ptr_to_provider_function = ss_data_provider_get_provider_function(data_provider, error);
        x_matrix_rows = 1;
        x_matrix_columns = number_of_columns - 1;
        y_vector_size = 1;
        X_matrix_rows = x_matrix_rows;
        X_matrix_columns = x_matrix_columns + 1;

        X_matrix = ss_matrix_create_new(X_matrix_rows, X_matrix_columns, error);
        if (X_matrix == NULL) goto catch_error;
        ss_matrix_fill(X_matrix, 0, 0, X_matrix_rows - 1, 0, 1.0, NULL);
        Y_HAT_vector = ss_vector_create_new(y_vector_size, error);
        if (Y_HAT_vector == NULL) goto catch_error;
        E_vector = ss_vector_create_new(y_vector_size, error);
        if (E_vector == NULL) goto catch_error;
        ET_vector = ss_vector_create_new(y_vector_size, error);
        if (ET_vector == NULL) goto catch_error;
        ETE_vector = ss_vector_create_new(1, error);
        if (ETE_vector == NULL) goto catch_error;
        model_vector_size = X_matrix_columns;
        if (model_vector_start != NULL) {
            model_vector = ss_vector_transpose(model_vector_start, NULL, error);
            if (model_vector == NULL) goto catch_error;
        } else {
            model_vector = ss_vector_create_new(model_vector_size, error);
            if (model_vector == NULL) goto catch_error;
            ss_vector_fill_random(model_vector, -1, 1, error);
        }
        XTE_vector = ss_vector_create_new(model_vector_size, error);
        if (XTE_vector == NULL) goto catch_error;

        for (int iteration = 1; iteration <= iterations; ++iteration) {
            ptr_to_provider_function(data_provider, dataset, &rows_filled_by_provider, error);
            if (rows_filled_by_provider == 0) break;
            ss_on_error_throw(error, catch_error);
            ss_matrix_get_dimensions(dataset, &dataset_rows, &dataset_columns, error);
            x_matrix = ss_matrix_create_new(dataset_rows, dataset_columns - 1, error);
            ss_matrix_copy(x_matrix,0,0,dataset,0,0,dataset_rows-1,dataset_columns-2,error);
            y = ss_matrix_get_vector(dataset, 0 , dataset_columns - 1, dataset_rows - 1, dataset_columns - 1, error);

            ss_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_matrix_rows - 1, x_matrix_columns - 1, NULL);
            XT_matrix = ss_matrix_transpose_create_new(X_matrix, error);

            Y_HAT_vector = ss_multiply_matrix_with_vector(X_matrix, model_vector, Y_HAT_vector, error);
            E_vector = ss_subtract_vector(Y_HAT_vector, y, E_vector, error);
            ET_vector = ss_vector_transpose(E_vector, ET_vector, error);
            error_value = ss_multiply_vector_get_scalar(ET_vector, E_vector, error);
            error_value = error_value / (2*X_matrix_rows);

            if (on_iteration_completed && !on_iteration_completed(iteration, error_value, model_vector)) {
                break;
            }

            XTE_vector = ss_multiply_matrix_with_vector(XT_matrix, E_vector, XTE_vector, error);
            XTE_vector = ss_multiply_vector_with_scalar(XTE_vector, (1.0 / X_matrix_rows)*learning_rate, XTE_vector, error);
            model_vector = ss_subtract_vector(model_vector, XTE_vector, model_vector, error);
            ss_matrix_destroy(XT_matrix, error);
        }

        goto exit_program;
        catch_error:
            ss_get_error(error_message, 4096, error);
            printf("Error : %s\n", error_message);
            return NULL;
        exit_program:
            ss_matrix_destroy(dataset, error);
            ss_matrix_destroy(x_matrix, error);
            ss_vector_destroy(y, error);
            ss_error_destroy(error);
        return model_vector;
}

ss_vector *ss_logistic_regression_batch_gradient_descent(const ss_matrix *x_matrix,
    const ss_vector *y_vector,
    const ss_vector *theta_start,
    uint64_t iterations,
    double learning_rate,
    uint8_t (*on_iteration_completed)(uint32_t, double, ss_vector *),
    ss_error *error) {

    ss_matrix *X_matrix = NULL;
    ss_matrix *XT_matrix = NULL;
    ss_vector *theta = NULL;
    ss_vector *Z = NULL;
    ss_vector *H = NULL;
    ss_vector *E = NULL;
    ss_vector *XT_E = NULL;
    ss_vector *log_H = NULL;
    ss_vector *one_minus_H = NULL;
    ss_vector *log_1_minus_H = NULL;
    ss_vector *y_log_H = NULL;
    ss_vector *one_minus_y = NULL;
    ss_vector *one_minus_y_log_1_minus_H = NULL;
    ss_vector *sum_vector = NULL;
    uint32_t x_rows, x_cols, X_rows, X_cols, y_size;
    uint64_t i;
    char error_message[1024];

    if (error) ss_clear_error(error);

    if (!x_matrix || !y_vector) {
        ss_set_error(error, "Null argument", SS_NULL_POINTER);
        return NULL;
    }

    ss_matrix_get_dimensions(x_matrix, &x_rows, &x_cols, error);
    y_size = ss_vector_get_size(y_vector, error);
    if (y_size != x_rows) {
        sprintf(error_message, "Mismatch: y (%u) vs x rows (%u)", y_size, x_rows);
        ss_set_error(error, error_message, SS_INPUT_AND_TARGET_OUTPUT_SIZE_MISMATCH);
        return NULL;
    }

    // Add bias column to X
    X_rows = x_rows;
    X_cols = x_cols + 1;
    X_matrix = ss_matrix_create_new(X_rows, X_cols, error);
    ss_matrix_fill(X_matrix, 0, 0, X_rows - 1, 0, 1.0, NULL);  // Bias
    ss_matrix_copy(X_matrix, 0, 1, x_matrix, 0, 0, x_rows - 1, x_cols - 1, NULL);

    XT_matrix = ss_matrix_transpose_create_new(X_matrix, error);

    // Initialize theta
    if (theta_start) {
        theta = ss_vector_transpose(theta_start, NULL, error);
    } else {
        theta = ss_vector_create_new(X_cols, error);
        ss_vector_fill_random(theta, -0.01, 0.01, error);
    }

    Z = ss_vector_create_new(X_rows, error);
    H = ss_vector_create_new(X_rows, error);
    E = ss_vector_create_new(X_rows, error);
    log_H = ss_vector_create_new(X_rows, error);
    one_minus_H = ss_vector_create_new(X_rows, error);
    log_1_minus_H = ss_vector_create_new(X_rows, error);
    y_log_H = ss_vector_create_new(X_rows, error);
    one_minus_y = ss_vector_create_new(X_rows, error);
    one_minus_y_log_1_minus_H = ss_vector_create_new(X_rows, error);
    sum_vector = ss_vector_create_new(X_rows, error);

    XT_E = ss_vector_create_new(X_cols, error);

    for (i = 1; i <= iterations; ++i) {
        // Z = X * theta
        Z = ss_multiply_matrix_with_vector(X_matrix, theta, Z, error);
        // H = sigmoid(Z)
        H = ss_sigmoid_vector(Z, H, error);
        // E = H - y
        E = ss_subtract_vector(H, y_vector, E, error);

        // Compute cost (cross-entropy loss)
        // cost = -1/m * sum( y*log(h) + (1-y)*log(1-h) )
        log_H = ss_log_vector(H, log_H, error);
        // log(1 - h)
        one_minus_H = ss_subtract_vector_from_scalar(1.0, H, one_minus_H, error);
        log_1_minus_H = ss_log_vector(one_minus_H, log_1_minus_H, error);
        // y * log(h)
        y_log_H = ss_multiply_vector_element_wise(y_vector, log_H, y_log_H, error);
        // (1 - y) * log(1 - h)
        one_minus_y = ss_subtract_vector_from_scalar(1.0, y_vector, one_minus_y, error);
        one_minus_y_log_1_minus_H = ss_multiply_vector_element_wise(one_minus_y, log_1_minus_H, one_minus_y_log_1_minus_H, error);

        // Total cost vector = y_log_H + (1 - y) * log(1 - h)
        sum_vector = ss_add_vector_element_wise(y_log_H, one_minus_y_log_1_minus_H, sum_vector, error);

        // Scalar cost = -1/m * sum(sum_vector)

        double cost = -ss_sum_vector(sum_vector, error) / X_rows;

        if (on_iteration_completed && !on_iteration_completed(i, cost, theta)) break;

        // Gradient = (1/m) * X^T * E
        XT_E = ss_multiply_matrix_with_vector(XT_matrix, E, XT_E, error);
        XT_E = ss_multiply_vector_with_scalar(XT_E, learning_rate / X_rows, XT_E, error);
        theta = ss_subtract_vector(theta, XT_E, theta, error);
    }

    // Cleanup
    ss_vector_destroy(log_H, NULL);
    ss_vector_destroy(one_minus_H, NULL);
    ss_vector_destroy(log_1_minus_H, NULL);
    ss_vector_destroy(y_log_H, NULL);
    ss_vector_destroy(one_minus_y, NULL);
    ss_vector_destroy(one_minus_y_log_1_minus_H, NULL);
    ss_vector_destroy(sum_vector, NULL);
    ss_matrix_destroy(X_matrix, NULL);
    ss_matrix_destroy(XT_matrix, NULL);
    ss_vector_destroy(Z, NULL);
    ss_vector_destroy(H, NULL);
    ss_vector_destroy(E, NULL);
    ss_vector_destroy(XT_E, NULL);

    return theta;
}