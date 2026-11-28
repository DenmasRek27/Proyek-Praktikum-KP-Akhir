#include <string.h>
#include "player.h"
#include "util.h"

const char *NAMA_STAT[JUMLAH_STAT] = { "Kesehatan", "Mental", "Kekayaan", "Reputasi" };

const char *NAMA_RAMUAN[JUMLAH_RAMUAN] = {
    "ramuan penyembuh", "ramuan kekuatan", "ramuan keberuntungan",
    "ramuan penyegar", "ramuan penenang"
};

const char *NAMA_EQ[JUMLAH_EQ] = { "Pedang", "Baju Zirah", "Amulet Keberuntungan" };

const int HARGA_RAMUAN[JUMLAH_RAMUAN] = { 6, 8, 8, 7, 7 };

/* efek per level 0..3: tambahan damage, penahan damage, bonus karma */
static const int EFEK[JUMLAH_EQ][4] = { { 3, 5, 8, 12 }, { 1, 2, 3, 5 }, { 5, 10, 15, 25 } };

void inisialisasiPlayer(Player *p) {
    int i;
    memset(p, 0, sizeof *p);
    for (i = 0; i < JUMLAH_EQ; i++) p->levelEq[i] = -1;
    p->stat[STAT_KESEHATAN] = 70;
    p->stat[STAT_MENTAL] = 60;
    p->stat[STAT_KEKAYAAN] = 30;
    p->stat[STAT_REPUTASI] = 20;
    p->energiMaks = p->energi = ENERGI_AWAL;
    p->umur = 1;
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
    if (idx < 0 || idx >= JUMLAH_STAT) return;
    p->stat[idx] = batasi(p->stat[idx] + delta, STAT_MIN, STAT_MAX);
}

void ubahKarma(Player *p, int delta) {
    p->karma = batasi(p->karma + delta, KARMA_MIN, KARMA_MAX);
}

void tambahJalur(Player *p, int jalur, int poin) {
    if (jalur < 0 || jalur >= JUMLAH_JALUR) return;
    p->jalur[jalur] = batasi(p->jalur[jalur] + poin, 0, JALUR_MAKS);
}

int efekEq(const Player *p, int eq) {
    if (eq < 0 || eq >= JUMLAH_EQ) return 0;
    return p->levelEq[eq] < 0 ? 0 : EFEK[eq][p->levelEq[eq]];
}

int karmaEfektif(const Player *p) {
    return p->karma + p->buffKarma + efekEq(p, EQ_AMULET);
}

int jalurPoin(const Player *p, int jalur) {
    if (jalur < 0 || jalur >= JUMLAH_JALUR) return 0;
    return p->jalur[jalur];
}

void setFlag(Player *p, int flag) {
    if (flag < 0 || flag >= JUMLAH_FLAG) return;
    p->flag[flag >> 3] |= (unsigned char)(1u << (flag & 7));
}

int punyaFlag(const Player *p, int flag) {
    if (flag < 0 || flag >= JUMLAH_FLAG) return 0;
    return (p->flag[flag >> 3] >> (flag & 7)) & 1;
}
