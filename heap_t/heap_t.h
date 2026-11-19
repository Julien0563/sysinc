#include <stdbool.h>
#include <stdlib.h>

typedef struct heap_struct {
    /* These may be changed, but probably are okay */
    size_t ele_size; /* size of element */
    size_t num_eles; /* number of elements */
    size_t capacity; /* current capacity of the heap */
    bool (*gt)(void *, void *); /* for "greater than" */
    /* You must store all data elements s.t. reachable from 1 pointer */
    void *eles;
} heap_t;

heap_t heap(size_t ele_size, bool (*gt)(void *, void *));
void insert(heap_t *heap, void *ele_ptr);
void *maxpop(heap_t *heap);
