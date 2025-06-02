#include<ss.h>
#include<stdlib.h>
#include<string.h>

typedef struct __ss_error {
    uint16_t fallback_error_code;
    char *error_str;
} ss_error;

ss_error * ss_error_create_new() {
    ss_error *error;
    error = (ss_error *)malloc(sizeof(ss_error));
    if (error != NULL) {
        error -> error_str = NULL;
        error -> fallback_error_code = 0;
    }
    return error;
}

void __ss_error_destroy(ss_error **error) {
    if (error == NULL || *error == NULL) return;
    if ((*error) -> error_str) free((*error) -> error_str);
    free(*error);
    *error = NULL;
}

uint8_t ss_has_error(const ss_error *error) {
    if (error == NULL) return 0;
    if (error -> error_str == NULL && error -> fallback_error_code == 0) {
        return 0;
    }
    return 1;
}

void ss_set_error(ss_error *error, const char *error_str, uint16_t fallback_error_code) {
    if (!error) return;
    if (ss_has_error(error)) ss_clear_error(error);
    error -> fallback_error_code = fallback_error_code;
    error -> error_str = (char *)malloc(strlen(error_str) + 1);
    if (error -> error_str == NULL) return;
    strcpy(error -> error_str, error_str);
}

void ss_get_error(char *str, uint32_t size, const ss_error *error) {
    uint32_t i;
    uint32_t end_point;
    if (str && size > 0) *str = '\0';
    if (!error || !str || size == 0) return;
    if (error -> error_str == NULL && error -> fallback_error_code == 0) return;
    if (error -> error_str != NULL) {
        end_point = size - 1;
        for (i=0; i < end_point && error -> error_str[i] != '\0'; ++i) {
            str[i] = error -> error_str[i];
        }
        str[i] = '\0';
    }
    else {
        if (size < 6) return;
        sprintf(str, "%u", error -> fallback_error_code);
    }
}

void ss_clear_error(ss_error *error) {
    if (!error) return;
    if (error -> error_str) {
        free(error -> error_str);
        error -> error_str = NULL;
    }
    error -> fallback_error_code = 0;
}

uint32_t ss_get_error_len(const ss_error *error) {
    if (!error) return 0;
    if (error -> error_str == NULL && error -> fallback_error_code == 0) return 0;
    if (error -> error_str != NULL) return strlen(error -> error_str);
    return 5;
}

uint16_t ss_get_error_code(const ss_error *error) {
    if (error == NULL) return 0;
    return error -> fallback_error_code;
}
