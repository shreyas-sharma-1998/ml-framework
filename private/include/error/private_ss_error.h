#ifndef __PRIVATE__SS__ERROR__H__
#define __PRIVATE__SS__ERROR__H__
#include<stdio.h>
#include<stdint.h>

typedef struct __ss_error ss_error;

void ss_set_error(ss_error *error, const char *error_str, uint16_t fallback_error_code);

#endif