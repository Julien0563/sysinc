#include <stdio.h>
#include <stdlib.h>
#include "4096_t.h"
#include "bigegcd.h"

/* hex string (optional 0x prefix) to 4096_t */
static void h2i(const char *h, uint64_t *n) {
    size_t i;
    int c, v, k;
    memset(n, 0, BYTES);
    if (h[0] == '0' && (h[1] == 'x' || h[1] == 'X')) {
        h += 2;
    }
    for (; *h; h++) {
        c = *h;
        if (c >= '0' && c <= '9') {
            v = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            v = c - 'a' + 10;
        } else {
            v = c - 'A' + 10;
        }
        for (k = 0; k < 4; k++) { /* n <<= 1, four times */
            for (i = S - 1; i > 0; i--) {
                n[i] = (n[i] << 1) | (n[i - 1] >> 63);
            }
            n[0] <<= 1;
        }
        n[0] |= (uint64_t)v;
    }
}

int main(int argc, char **argv) {
    uint64_t a[S], m[S], inv[S];
    size_t i;
    if (argc != 3) {
        return 2;
    }
    h2i(argv[1], a);
    h2i(argv[2], m);
    if (!bigmodinv(a, m, inv)) {
        printf("none\n");
        return 0;
    }
    for (i = S - 1; i < S; i--) {
        printf("%016lx", (unsigned long)inv[i]);
    }
    printf("\n");
    return 0;
}
