#include <stdio.h>
#include <string.h>
#include "player.h"
#include "util.h"

const char *NAMA_STAT[JUMLAH_STAT] = { "Kesehatan", "Mental", "Kekayaan", "Reputasi" };

const char *NAMA_RAMUAN[JUMLAH_RAMUAN] = { "Ramuan Penyembuh", "Ramuan Kekuatan", "Ramuan Keberuntungan" };
const char *NAMA_EQ[JUMLAH_EQ] = { "Pedang", "Baju Zirah", "Amulet Keberuntungan" };
/* efek per level 0..3: damage+, damage masuk-, karma+ */
static const int EFEK[JUMLAH_EQ][4] = { {3,5,8,12}, {1,2,3,5}, {5,10,15,25} };

static int batasi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void inisialisasiPlayer(Player *p) {
    int i;
    memset(p, 0, sizeof *p);
    for (i = 0; i < JUMLAH_EQ; i++) p->levelEq[i] = -1;
    p->stat[STAT_KESEHATAN] = 70;
    p->stat[STAT_MENTAL]    = 60;
    p->stat[STAT_KEKAYAAN]  = 30;
    p->stat[STAT_REPUTASI]  = 20;
    p->energiMaks = p->energi = ENERGI_AWAL;
}

void resetEnergiHarian(Player *p) {
    p->energiMaks = (p->umur <= UMUR_DEWASA ? ENERGI_AWAL : ENERGI_DEWASA) + p->bonusEnergi;
    p->energi = p->energiMaks;
    p->buffKarma = 0;
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
    printf("\n--- Umur %d | Energi %d/%d | Karma %d | Ramuan %d/%d/%d ---\n",
           p->umur, p->energi, p->energiMaks, p->karma,
           p->ramuan[RAMUAN_HEAL], p->ramuan[RAMUAN_DMG], p->ramuan[RAMUAN_KARMA]);
    for (i = 0; i < JUMLAH_STAT; i++) {
        printf("%-10s: %3d/%d\n", NAMA_STAT[i], p->stat[i], STAT_MAX);
    }
}

int efekEq(const Player *p, int eq) {
    return p->levelEq[eq] < 0 ? 0 : EFEK[eq][p->levelEq[eq]];
}

int karmaEfektif(const Player *p) {
    return p->karma + p->buffKarma + efekEq(p, EQ_AMULET);
}

int pakaiRamuan(Player *p, int combat) {
    int i;
    for (i = 0; i < JUMLAH_RAMUAN; i++)
        printf("%d. %s (x%d)%s\n", i + 1, NAMA_RAMUAN[i], p->ramuan[i],
               (i == RAMUAN_DMG && !combat) ? " [hanya combat]" : "");
    printf("%d. Batal\n", JUMLAH_RAMUAN + 1);
    i = bacaAngka(1, JUMLAH_RAMUAN + 1) - 1;
    if (i == JUMLAH_RAMUAN) return 0;
    if (p->ramuan[i] <= 0) { printf("Kamu tidak punya ramuan itu.\n"); return 0; }
    if (i == RAMUAN_DMG && !combat) { printf("Hanya bisa dipakai saat combat.\n"); return 0; }
    p->ramuan[i]--;
    if (i == RAMUAN_HEAL) { ubahStat(p, STAT_KESEHATAN, 20); printf("Kesehatan +20.\n"); }
    else if (i == RAMUAN_DMG) { p->buffDmg = 1; printf("Seranganmu menguat selama pertarungan ini.\n"); }
    else { p->buffKarma = 20; printf("Keberuntunganmu naik sampai akhir hari.\n"); }
    return 1;
}
