#include "4096_t.h"

void prigmp(uint64_t *big, uint8_t words);

/* usage: ./primetest.out [words]   (default 32 words = 2048 bits)
 * prints one random prime as hex */
int main(int argc, char **argv) {
    uint64_t p[S];
    size_t i;
    int words = 32;

    if (argc > 1) {
        words = atoi(argv[1]);
    }
    prigmp(p, (uint8_t)words);
    for (i = S - 1; i < S; i--) {
        printf("%016llx", (unsigned long long)p[i]);
    }
    printf("\n");
    return 0;
}
