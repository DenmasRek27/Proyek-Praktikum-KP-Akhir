#include <stdio.h>
#include "player.h"

const char *NAMA_STAT[JUMLAH_STAT] = { "Kesehatan", "Mental", "Kekayaan", "Reputasi" };

static int batasi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void inisialisasiPlayer(Player *p) {
    p->stat[STAT_KESEHATAN] = 70;
    p->stat[STAT_MENTAL]    = 60;
    p->stat[STAT_KEKAYAAN]  = 30;
    p->stat[STAT_REPUTASI]  = 20;
    p->karma      = 0;
    p->umur       = 0;
    p->energiMaks = ENERGI_HARIAN_DEFAULT;
    p->energi     = ENERGI_HARIAN_DEFAULT;
    p->ramuan     = 0;
}

void resetEnergiHarian(Player *p) {
    p->energi = p->energiMaks;
}

int pakaiEnergi(Player *p, int biaya) {
    if (biaya <= 0) return 1;
    if (p->energi < biaya) return 0;
    p->energi -= biaya;
    return 1;
}

void ubahStat(Player *p, int idx, int delta) {
    p->stat[idx] = batasi(p->stat[idx] + delta, STAT_MIN, STAT_MAX);
}

void ubahKarma(Player *p, int delta) {
    p->karma = batasi(p->karma + delta, KARMA_MIN, KARMA_MAX);
}

void tampilStatus(const Player *p) {
    int i;
    printf("\n--- Umur %d | Energi %d/%d | Karma %d | Ramuan %d ---\n",
           p->umur, p->energi, p->energiMaks, p->karma, p->ramuan);
    for (i = 0; i < JUMLAH_STAT; i++) {
        printf("%-10s: %3d/%d\n", NAMA_STAT[i], p->stat[i], STAT_MAX);
    }
}
