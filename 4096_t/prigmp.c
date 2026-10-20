#include <gmp.h>
#include "4096_t.h"

/* Fill big with a random prime of exactly words*64 bits (S words total, the
 * upper words are zero). Randomness comes from /dev/random.
 * Top two bits and the low bit are forced before the search, so the prime has
 * the full length and the product of two of them has twice as many bits.
 * If mpz_nextprime runs off the top of the range we draw again. */
void prigmp(uint64_t *big, uint8_t words) {
    mpz_t m;
    FILE *fp;
    size_t got;

    if (words == 0 || words > S) {
        words = S;
    }
    for (;;) {
        memset(big, 0, BYTES);
        fp = fopen("/dev/random", "r");
        if (fp == NULL) {
            perror("/dev/random");
            exit(1);
        }
        got = fread(big, sizeof(uint64_t), words, fp);
        fclose(fp);
        if (got != words) {
            fprintf(stderr, "short read from /dev/random\n");
            exit(1);
        }
        big[words - 1] |= ((uint64_t)3 << 62);
        big[0] |= 1;

        mpz_init(m);
        mpz_import(m, S, -1, sizeof(uint64_t), 0, 0, big);
        mpz_nextprime(m, m);
        if (mpz_sizeinbase(m, 2) <= (size_t)words * 64) {
            memset(big, 0, BYTES);
            mpz_export(big, NULL, -1, sizeof(uint64_t), 0, 0, m);
            mpz_clear(m);
            return;
        }
        mpz_clear(m);
    }
}
