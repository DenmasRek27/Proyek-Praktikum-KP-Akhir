#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int acak(int min, int max) {
    return min + rand() % (max - min + 1);
}

int bacaAngka(int min, int max) {
    char buf[32];
    int n;
    while (1) {
        printf("> ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) exit(0);
        if (sscanf(buf, "%d", &n) == 1 && n >= min && n <= max) return n;
        printf("Masukkan angka %d-%d.\n", min, max);
    }
}

void tungguEnter(void) {
    char buf[32];
    printf("\n[Tekan Enter untuk lanjut...]");
    if (fgets(buf, sizeof(buf), stdin) == NULL) exit(0);
}

void bersihkanLayar(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
