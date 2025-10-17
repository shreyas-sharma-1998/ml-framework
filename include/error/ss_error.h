#ifndef __SS__ERROR__H__
#define __SS__ERROR__H__
#include<stdint.h>
#include<stdio.h>

#define SS_LOW_MEMORY 1
#define SS_INVALID_DIMENSIONS 2
#define SS_INVALID_ROW_COUNT 3
#define SS_INVALID_COLUMN_COUNT 4
#define SS_NULL_POINTER 5
#define SS_INVALID_ROW_AND_COLUMN_INDEX 6
#define SS_INVALID_ROW_INDEX 7
#define SS_INVALID_COLUMN_INDEX 8
#define SS_FILE_WRITE_ERROR 9
#define SS_FILE_READ_ERROR 10
#define SS_CSV_FORMAT_ERROR 11
#define SS_CSV_EMPTY_ERROR 12
#define SS_INVALID_SIZE 13
#define SS_INVALID_INDEX 14
#define SS_FILE_FORMAT_ERROR 15
#define SS_FILE_CONTAINS_MATRIX 16
#define SS_GET_VECTOR_ERROR 17
#define SS_INVALID_DIMENSIONS_FOR_MATRIX_MULTIPLICATION 18
#define SS_INVALID_DIMENSIONS_FOR_MATRIX_VECTOR_MULTIPLICATION 19
#define SS_VECTOR_SUBTRACT_SIZE_ERROR 20
#define SS_VECTOR_SIZE_MISMATCH 21
#define SS_INVALID_MATRIX_CONTAINER_DIMENSIONS 22
#define SS_INVALID_VECTOR_CONTAINER_SIZE 22
#define SS_INVALID_NUMBER_OF_ITERATIONS 24
#define SS_INPUT_AND_TARGET_OUTPUT_SIZE_MISMATCH 25
#define SS_INVALID_SIZE_FOR_VECTOR_MULTIPLICATION 26
#define SS_INVALID_MODEL_SIZE 27

#define ss_on_error_throw(a,b) if(ss_has_error(a)) goto b

typedef struct __ss_error ss_error;

ss_error * ss_error_create_new();
void __ss_error_destroy(ss_error **error);

#define ss_error_destroy(error) __ss_error_destroy(&error)

uint8_t ss_has_error(const ss_error *error);

void ss_get_error(char *str, uint32_t size, const ss_error *error);

void ss_clear_error(ss_error *error);

uint32_t ss_get_error_len(const ss_error *error);

uint16_t ss_get_error_code(const ss_error *error);

#endif