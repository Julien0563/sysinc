#include "4096_t.h"

/* ---- BigAdd ---- */
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

/* ---- BigSub ---- */
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

/* ---- BigMul (stub for now) ---- */
uint64_t bigmul(uint64_t *in0, uint64_t *in1, uint64_t *out) {
    memset(out, 0, BYTES);
    (void)in0;
    (void)in1;
    return 0;
}

/* ---- BigQuo / BigRem (stubs for now) ---- */
uint64_t bigquo(uint64_t *num, uint64_t *den, uint64_t *quo) {
    memset(quo, 0, BYTES);
    (void)num;
    (void)den;
    return 0;
}

uint64_t bigrem(uint64_t *num, uint64_t *den, uint64_t *rem) {
    memset(rem, 0, BYTES);
    (void)num;
    (void)den;
    return 0;
}
