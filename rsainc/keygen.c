#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define E 0x10001UL

static int is_prime(unsigned long x)
{
    unsigned long i;
    if (x < 2) return 0;
    if (x % 2 == 0) return x == 2;
    for (i = 3; i * i <= x; i += 2)
        if (x % i == 0) return 0;
    return 1;
}

/* random prime in [2^15, 2^16) */
static unsigned long rand_prime(void)
{
    unsigned long p;
    do {
        p = 0x8000UL + (unsigned long)rand() % 0x8000UL;
    } while (!is_prime(p));
    return p;
}

/* modular inverse of a mod m via extended Euclid; returns 0 if none */
static unsigned long mod_inverse(unsigned long a, unsigned long m)
{
    long t = 0, newt = 1;
    long r = (long)m, newr = (long)a;
    long q, tmp;
    while (newr != 0) {
        q = r / newr;
        tmp = t - q * newt; t = newt; newt = tmp;
        tmp = r - q * newr; r = newr; newr = tmp;
    }
    if (r != 1) return 0;
    if (t < 0) t += (long)m;
    return (unsigned long)t;
}

int main(void)
{
    unsigned long p, q, n, phi, d;
    FILE *f;

    srand((unsigned int)time(NULL));

    for (;;) {
        p = rand_prime();
        q = rand_prime();
        if (p == q) continue;
        n = p * q;
        if (n < 0x80000000UL) continue; /* must exceed 1 << 31 */
        phi = (p - 1) * (q - 1);
        if (phi % E == 0) continue;
        d = mod_inverse(E, phi);
        if (d == 0) continue;
        break;
    }

    f = fopen("unsafe.bad", "w");
    if (f == NULL) {
        perror("unsafe.bad");
        return 1;
    }
    fprintf(f, "-----BEGIN UNSAFE PRIVATE KEY-----\n%lx\n%lx\n%lx\n"
               "-----END UNSAFE PRIVATE KEY-----\n", n, E, d);
    if (fclose(f) != 0) {
        perror("unsafe.bad");
        return 1;
    }

    f = fopen("unsafe.pub", "w");
    if (f == NULL) {
        perror("unsafe.pub");
        return 1;
    }
    fprintf(f, "-----BEGIN UNSAFE PUBLIC KEY-----\n%lx\n%lx\n"
               "-----END UNSAFE PUBLIC KEY-----\n", n, E);
    if (fclose(f) != 0) {
        perror("unsafe.pub");
        return 1;
    }
    return 0;
}
