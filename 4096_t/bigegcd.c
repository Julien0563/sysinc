#include "4096_t.h"
#include "bigegcd.h"

/* Unsigned extended Euclidean algorithm.
 *
 * The coefficients t alternate in sign (0, +1, -q1, +, -, ...), so in
 * magnitudes every step is an addition:
 *     |t_{i+1}| = |t_{i-1}| + q_i * |t_i|
 * and |t_i| never exceeds m, so it all fits in 4096 bits.
 *
 * The sign is fixed by the iteration count j. After the loop, t0 holds
 * |t_j|, positive if j is odd, negative if j is even. If positive the
 * inverse is |t_j|; if negative it is m - |t_j|.
 *
 * The remainder is r0 - q*r1, one division per step. q*r1 <= r0, so
 * bigmul's truncation to 4096 bits never loses anything here. */

static int iszero(uint64_t *a) {
    size_t i;
    for (i = 0; i < S; i++) {
        if (a[i] != 0) {
            return 0;
        }
    }
    return 1;
}

static int isone(uint64_t *a) {
    size_t i;
    if (a[0] != 1) {
        return 0;
    }
    for (i = 1; i < S; i++) {
        if (a[i] != 0) {
            return 0;
        }
    }
    return 1;
}

static void copy(uint64_t *dst, uint64_t *src) {
    memcpy(dst, src, BYTES);
}

uint64_t bigmodinv(uint64_t *a, uint64_t *m, uint64_t *inv) {
    uint64_t r0[S], r1[S], t0[S], t1[S], q[S], p[S], tn[S];
    unsigned long j = 0;
    int ok;

    if (iszero(m) || isone(m)) {
        return 0;
    }

    copy(r0, m);
    bigrem(a, m, r1); /* a may be larger than m */
    memset(t0, 0, BYTES);
    memset(t1, 0, BYTES);
    t1[0] = 1;

    while (!iszero(r1)) {
        bigquo(r0, r1, q);
        bigmul(q, r1, p);   /* p = q * r1 */
        bigsub(r0, p, p);   /* p = r0 - q * r1 = r0 mod r1 */
        copy(r0, r1);
        copy(r1, p);

        bigmul(q, t1, tn);  /* tn = q * |t1| */
        bigadd(t0, tn, tn); /* tn = |t0| + q * |t1| */
        copy(t0, t1);
        copy(t1, tn);
        j++;
    }

    ok = isone(r0); /* r0 is now gcd(a, m) */
    if (ok) {
        if (j % 2 == 1) {
            copy(inv, t0);
        } else {
            bigsub(m, t0, inv);
        }
    }
    return ok;
}
