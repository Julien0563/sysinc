/* heap_t.c */

/* Max heap stored in one contiguous array of untyped, fixed-size elements.
   0-indexed: the children of i are 2i+1 and 2i+2, the parent of i is (i-1)/2.
   Elements are copied in and out with memcpy, so any ele_size works. */

#include "heap_t.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Room for this many elements is allocated up front. */
#define INIT_CAP 16

#define ELE(h, i) ((void *)((char *)(h)->eles + (i) * (h)->ele_size))

static void die(const char *msg)
{
    fprintf(stderr, "heap_t: %s\n", msg);
    exit(1);
}

/* Swap elements i and j byte by byte, so no temporary allocation. */
static void swap(heap_t *h, size_t i, size_t j)
{
    unsigned char *a = ELE(h, i), *b = ELE(h, j), t;
    size_t k;

    for (k = 0; k < h->ele_size; k++) {
        t = a[k];
        a[k] = b[k];
        b[k] = t;
    }
}

static void sift_up(heap_t *h, size_t i)
{
    size_t parent;

    while (i > 0) {
        parent = (i - 1) / 2;
        if (!h->gt(ELE(h, i), ELE(h, parent)))
            break;
        swap(h, i, parent);
        i = parent;
    }
}

static void sift_down(heap_t *h, size_t i)
{
    size_t left, right, big;

    for (;;) {
        left = 2 * i + 1;
        right = left + 1;
        big = i;

        if (left < h->num_eles && h->gt(ELE(h, left), ELE(h, big)))
            big = left;
        if (right < h->num_eles && h->gt(ELE(h, right), ELE(h, big)))
            big = right;
        if (big == i)
            return;
        swap(h, i, big);
        i = big;
    }
}

heap_t heap(size_t ele_size, bool (*gt)(void *, void *))
{
    heap_t h;

    if (ele_size == 0)
        die("element size must be positive");

    h.ele_size = ele_size;
    h.num_eles = 0;
    h.capacity = INIT_CAP;
    h.gt = gt;
    /* calloc: unused slots read as zero rather than as garbage. */
    h.eles = calloc(h.capacity, ele_size);
    if (h.eles == NULL)
        die("out of memory");
    return h;
}

void insert(heap_t *heap, void *ele_ptr)
{
    if (heap->num_eles == heap->capacity) {
        size_t old = heap->capacity;
        size_t new_cap = old * 2;
        void *tmp;

        if (new_cap / 2 != old || new_cap > SIZE_MAX / heap->ele_size)
            die("heap too large");
        tmp = realloc(heap->eles, new_cap * heap->ele_size);
        if (tmp == NULL)
            die("out of memory");
        heap->eles = tmp;
        heap->capacity = new_cap;
        memset(ELE(heap, old), 0, (new_cap - old) * heap->ele_size);
    }

    memcpy(ELE(heap, heap->num_eles), ele_ptr, heap->ele_size);
    heap->num_eles++;
    sift_up(heap, heap->num_eles - 1);
}

void *maxpop(heap_t *heap)
{
    void *out;

    if (heap->num_eles == 0)
        return NULL;

    out = malloc(heap->ele_size);
    if (out == NULL)
        die("out of memory");
    memcpy(out, ELE(heap, 0), heap->ele_size);

    /* Move the last leaf to the root, then sift it down. The vacated last
       slot is simply left outside num_eles. */
    heap->num_eles--;
    if (heap->num_eles > 0) {
        memcpy(ELE(heap, 0), ELE(heap, heap->num_eles), heap->ele_size);
        sift_down(heap, 0);
    }
    return out;
}
