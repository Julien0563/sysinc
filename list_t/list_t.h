/* list_t.h */
#ifndef LIST_T
#define LIST_T

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
typedef uint64_t bool;

typedef void **list_t;

list_t list_new();
void list_free(list_t l);
void list_print(list_t l);
void list_append(list_t l, void *x);
void list_extend(list_t l1, list_t l2);
void list_insert(list_t l, size_t i, void *x);
bool list_remove(list_t l, void *x);
void *list_pop(list_t l, size_t i);
void list_clear(list_t l);
size_t list_index(list_t l, void *x);
uint64_t list_count(list_t l, void *x);
void list_reverse(list_t l);
list_t list_copy(list_t l);

#endif
