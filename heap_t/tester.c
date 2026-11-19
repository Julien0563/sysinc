#include "heap_t.h"
#include <stdio.h>
#include <stdint.h>

#define HEAP_SIZE 6

bool gt_uint8_t(void *a, void *b) {
    return (*(uint8_t *)a > *(uint8_t *)b);
}

void print_uint8_ts(void *a) {
    size_t i;
    uint8_t *ptr = (uint8_t *)a;
    printf("[");
    for (i = 0; i < HEAP_SIZE - 1; i++) {
        printf("%u, ", ptr[i]);
    }
    printf("%u]\n", ptr[HEAP_SIZE - 1]);
}

int main() {
    uint8_t vals[HEAP_SIZE] = {128, 64, 192, 96, 160, 224}, i, j, *ptr;
    heap_t h = heap(sizeof(uint8_t), gt_uint8_t);
    
    printf("Printing array:\n");
    print_uint8_ts(vals);
    
    for (i = 0; i < HEAP_SIZE; i++) {
        insert(&h, &vals[i]);
        printf("Printing heap after add no. %u:\n", i);
        print_uint8_ts(h.eles);
    }

    printf("Printing heap:\n[");
    ptr = (uint8_t *)h.eles;
    for (i = 0; i < HEAP_SIZE - 1; i++) {
        printf("%u, ", ptr[i]);
    }
    printf("%u]\n", ptr[HEAP_SIZE - 1]);

    for (i = 0; i < HEAP_SIZE; i++) {
        ptr = maxpop(&h);
        vals[i] = *ptr;
        free(ptr);
    }

    printf("Printing heapsorted array:\n");
    print_uint8_ts(vals);
    
    for (i = 0; i < HEAP_SIZE - 1; i++) {
        if (vals[i] < vals[i+1]) {
            exit(1);
        }
    }

    return 0;
}
