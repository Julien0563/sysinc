#ifndef BIGEGCD_H
#define BIGEGCD_H

#include <stdint.h>

/* inv = a^-1 mod m, by the unsigned extended Euclidean algorithm.
 * Returns 1 if the inverse exists, else 0 (and inv is not written). */
uint64_t bigmodinv(uint64_t *a, uint64_t *m, uint64_t *inv);

#endif
