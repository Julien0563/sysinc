#include "4096_t.h"

uint64_t bigadd(uint64_t *in0, uint64_t *in1, uint64_t *sum) {
    uint64_t carry = 0;
    size_t i;
    for (i = 0; i < S; i++) {
        uint64_t a = in0[i];
        uint64_t b = in1[i];
        uint64_t s = a + b + carry;
        carry = (s < a) || (carry && s == a);
        sum[i] = s;
    }
    return carry;
}

uint64_t bigsub(uint64_t *min, uint64_t *sub, uint64_t *dif) {
    uint64_t borrow = 0;
    size_t i;
    for (i = 0; i < S; i++) {
        uint64_t a = min[i];
        uint64_t b = sub[i];
        uint64_t d = a - b - borrow;
        borrow = (a < b) || (a == b && borrow);
        dif[i] = d;
    }
    return borrow;
}

/* ---- BigMul: 32-bit-limb schoolbook multiplication ---- */
uint64_t bigmul(uint64_t *in0, uint64_t *in1, uint64_t *out) {
    uint32_t *a = (uint32_t *)in0;
    uint32_t *b = (uint32_t *)in1;
    uint32_t wrk[4 * S];
    size_t i, j, n, k;
    uint64_t carry, t;

    n = 2 * S;
    memset(wrk, 0, sizeof(wrk));

    for (i = 0; i < n; i++) {
        if (a[i] == 0) {
            continue;
        }
        carry = 0;
        for (j = 0; j < n; j++) {
            t = (uint64_t)a[i] * (uint64_t)b[j] + wrk[i + j] + carry;
            wrk[i + j] = (uint32_t)t;
            carry = t >> 32;
        }
        k = i + n;
        while (carry) {
            t = (uint64_t)wrk[k] + carry;
            wrk[k] = (uint32_t)t;
            carry = t >> 32;
            k++;
        }
    }

    memcpy(out, wrk, BYTES);
    return 0;
}

/* ---- Division helpers (internal) ---- */
static int bigcmp(uint64_t *a, uint64_t *b) {
    size_t i = S;
    while (i--) {
        if (a[i] != b[i]) {
            return (a[i] > b[i]) ? 1 : -1;
        }
    }
    return 0;
}

static uint64_t get_bit(uint64_t *n, size_t idx) {
    return (n[idx / 64] >> (idx % 64)) & 1ULL;
}

static void set_bit(uint64_t *n, size_t idx) {
    n[idx / 64] |= ((uint64_t)1 << (idx % 64));
}

static void bigdivmod(uint64_t *num, uint64_t *den, uint64_t *quo, uint64_t *rem) {
    size_t bit;
    size_t k;
    uint64_t ovf, new_ovf;

    memset(quo, 0, BYTES);
    memset(rem, 0, BYTES);
    ovf = 0;

    bit = S * 64;
    while (bit-- > 0) {
        new_ovf = (rem[S - 1] >> 63) & 1ULL;
        for (k = S - 1; k > 0; k--) {
            rem[k] = (rem[k] << 1) | (rem[k - 1] >> 63);
        }
        rem[0] = (rem[0] << 1) | get_bit(num, bit);
        ovf = new_ovf;

        if (ovf || bigcmp(rem, den) >= 0) {
            bigsub(rem, den, rem);
            ovf = 0;
            set_bit(quo, bit);
        }
    }
}

/* ---- BigQuo / BigRem ---- */
uint64_t bigquo(uint64_t *num, uint64_t *den, uint64_t *quo) {
    uint64_t rem[S];
    bigdivmod(num, den, quo, rem);
    return 0;
}

uint64_t bigrem(uint64_t *num, uint64_t *den, uint64_t *rem) {
    uint64_t quo[S];
    bigdivmod(num, den, quo, rem);
    return 0;
}
