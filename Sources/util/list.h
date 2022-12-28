#pragma once

/*! \file list.h
    \brief Dynamic, exponentially growing list type.
*/

#include <stdbool.h>

typedef struct sw_list_int sw_list_int_t;

sw_list_int_t *sw_list_int_init(int reserve);
void sw_list_int_destroy(sw_list_int_t *lst);
int sw_list_int_len(sw_list_int_t *lst);
void sw_list_int_push(sw_list_int_t *lst, int el);
void sw_list_int_extend(sw_list_int_t *dest, sw_list_int_t *src);
void sw_list_int_reverse(sw_list_int_t *lst);
void sw_list_int_clear(sw_list_int_t *lst);
bool sw_list_int_contains(sw_list_int_t *lst, int el);
int sw_list_int_pop(sw_list_int_t *lst);
int sw_list_int_get(sw_list_int_t *lst, int index);
