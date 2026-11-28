#include <stdlib.h>
#include <string.h>
#include "util.h"

int acak(int min, int max) {
    if (max <= min) return min;
    return min + rand() % (max - min + 1);
}

int batasi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int peluang(int persen) {
    if (persen <= 0) return 0;
    if (persen >= 100) return 1;
    return acak(1, 100) <= persen;
}

int salinTeks(char *dst, int kapasitas, const char *src) {
    int n;
    if (kapasitas <= 0) return 0;
    if (!src) { dst[0] = 0; return 0; }
    n = (int)strlen(src);
    if (n >= kapasitas) n = kapasitas - 1;
    memcpy(dst, src, (size_t)n);
    dst[n] = 0;
    return n;
}

int tambahTeks(char *dst, int kapasitas, const char *src) {
    int n = (int)strlen(dst);
    if (!src) return n;
    return n + salinTeks(dst + n, kapasitas - n, src);
}
