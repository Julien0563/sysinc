#include <stdio.h>

static char num_to_b64(unsigned n) {
        if (n < 26) return 'A' + n;
        if (n < 52) return 'a' + (n - 26);
        if (n < 62) return '0' + (n - 52);
        return n == 62 ? '+' : '/';
}

int main(int argc, char *argv[]) {
        FILE *fp;
        unsigned char buf[3];
        size_t n;
        int col = 0;
        unsigned v;
        char out[4];

        if (argc != 2) {
                fprintf(stderr, "usage: %s FILE\n", argv[0]);
                return 1;
        }
        fp = fopen(argv[1], "rb");
        if (fp == NULL) {
                perror(argv[1]);
                return 1;
        }

        while ((n = fread(buf, 1, 3, fp)) > 0) {
                v = (unsigned)buf[0] << 16;
                if (n > 1) v |= (unsigned)buf[1] << 8;
                if (n > 2) v |= buf[2];

                out[0] = num_to_b64((v >> 18) & 63);
                out[1] = num_to_b64((v >> 12) & 63);
                out[2] = num_to_b64((v >> 6) & 63);
                out[3] = num_to_b64(v & 63);

                if (n < 3) out[3] = '=';
                if (n < 2) out[2] = '=';

                for (int i = 0; i < 4; i++) {
                        putchar(out[i]);
                        if (++col == 76) {
                                putchar('\n');
                                col = 0;
                        }
                }
        }
        if (col != 0) putchar('\n');

        fclose(fp);
        return 0;
}
