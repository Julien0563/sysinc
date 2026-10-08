#include <stdio.h>
#include <string.h>

static unsigned long modexp(unsigned long m, unsigned long e, unsigned long n)
{
    unsigned long result = 1;
    m %= n;
    while (e > 0) {
        if (e & 1) result = (result * m) % n;
        m = (m * m) % n;
        e >>= 1;
    }
    return result % n;
}

int main(int argc, char *argv[])
{
    FILE *key, *in, *out;
    unsigned long n = 0, e = 0, d = 0, m = 0, c;
    int ch, i = 0, ok;

    if (argc != 4) {
        fprintf(stderr, "usage: %s -e|-d infile outfile\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        key = fopen("unsafe.pub", "r");
        if (key == NULL) {
            perror("unsafe.pub");
            return 1;
        }
        ok = fscanf(key, "-----BEGIN UNSAFE PUBLIC KEY-----\n%lx\n%lx\n"
                         "-----END UNSAFE PUBLIC KEY-----\n", &n, &e) == 2;
        fclose(key);
    } else if (strcmp(argv[1], "-d") == 0) {
        key = fopen("unsafe.bad", "r");
        if (key == NULL) {
            perror("unsafe.bad");
            return 1;
        }
        ok = fscanf(key, "-----BEGIN UNSAFE PRIVATE KEY-----\n%lx\n%lx\n%lx\n"
                         "-----END UNSAFE PRIVATE KEY-----\n", &n, &e, &d) == 3;
        fclose(key);
        e = d;
    } else {
        fprintf(stderr, "first argument must be -e or -d\n");
        return 1;
    }
    if (!ok || n == 0) {
        fprintf(stderr, "could not parse key file\n");
        return 1;
    }

    in = fopen(argv[2], "rb");
    if (in == NULL) {
        perror(argv[2]);
        return 1;
    }
    while ((ch = fgetc(in)) != EOF && i < 8) {
        m |= ((unsigned long)ch & 0xFFUL) << (8 * i);
        i++;
    }
    fclose(in);

    c = modexp(m, e, n);

    out = fopen(argv[3], "wb");
    if (out == NULL) {
        perror(argv[3]);
        return 1;
    }
    for (i = 0; i < 4; i++)
        fputc((int)((c >> (8 * i)) & 0xFFUL), out);
    if (fclose(out) != 0) {
        perror(argv[3]);
        return 1;
    }
    return 0;
}
