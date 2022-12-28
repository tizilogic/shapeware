#include "list.h"

#include <assert.h>
#include <krink/memory.h>

typedef struct sw_list_int {
	int *arr;
	int cap;
	int len;
} sw_list_int_t;

sw_list_int_t *sw_list_int_init(int reserve) {
	sw_list_int_t *lst = (sw_list_int_t *)kr_malloc(sizeof(sw_list_int_t));
	assert(lst);
	lst->cap = reserve;
	lst->len = 0;
	if (reserve > 0) {
		lst->arr = (int *)kr_malloc(reserve * sizeof(int));
		assert(lst->arr);
	}
	else
		lst->arr = NULL;
	return lst;
}

void sw_list_int_destroy(sw_list_int_t *lst) {
	assert(lst);
	if (lst->arr != NULL) kr_free(lst->arr);
	kr_free(lst);
}

int sw_list_int_len(sw_list_int_t *lst) {
	assert(lst);
	return lst->len;
}

void sw_list_int_push(sw_list_int_t *lst, int el) {
	assert(lst);
	if (lst->len >= lst->cap) {
		if (lst->cap > 0) {
			lst->arr = (int *)kr_realloc(lst->arr, 2 * lst->cap * sizeof(int));
			lst->cap *= 2;
		}
		else {
			lst->arr = (int *)kr_malloc(sizeof(int));
			lst->cap = 1;
		}
		assert(lst->arr);
	}
	lst->arr[lst->len++] = el;
}

void sw_list_int_extend(sw_list_int_t *dest, sw_list_int_t *src) {
	assert(dest && src);
	int size = sw_list_int_len(src);
	for (int i = 0; i < size; ++i) sw_list_int_push(dest, src->arr[i]);
}

void sw_list_int_reverse(sw_list_int_t *lst) {
	assert(lst);
	int half = lst->len / 2;
	for (int i = 0; i < half; ++i) {
		int tmp = lst->arr[i];
		lst->arr[i] = lst->arr[lst->len - 1 - i];
		lst->arr[lst->len - 1 - i] = tmp;
	}
}

void sw_list_int_clear(sw_list_int_t *lst) {
	assert(lst);
	lst->len = 0;
}

bool sw_list_int_contains(sw_list_int_t *lst, int el) {
	assert(lst);
	for (int i = 0; i < lst->len; ++i)
		if (lst->arr[i] == el) return true;
	return false;
}

int sw_list_int_pop(sw_list_int_t *lst) {
	assert(lst);
	assert(lst->len > 0);
	--(lst->len);
	return lst->arr[lst->len];
}

int sw_list_int_get(sw_list_int_t *lst, int index) {
	assert(lst);
	assert(index >= 0);
	assert(lst->len > index);
	return lst->arr[index];
}
