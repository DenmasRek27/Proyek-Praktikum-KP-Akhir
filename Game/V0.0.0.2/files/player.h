#ifndef PLAYER_H
#define PLAYER_H

#define STAT_MIN 0
#define STAT_MAX 100
#define KARMA_MIN -50
#define KARMA_MAX 50
#define ENERGI_HARIAN_DEFAULT 10

enum { STAT_KESEHATAN, STAT_MENTAL, STAT_KEKAYAAN, STAT_REPUTASI, JUMLAH_STAT };

typedef struct {
    int stat[JUMLAH_STAT];
    int karma;
    int umur;
    int energi;
    int energiMaks;
    int ramuan;
} Player;

extern const char *NAMA_STAT[JUMLAH_STAT];

void inisialisasiPlayer(Player *p);
void resetEnergiHarian(Player *p);
int pakaiEnergi(Player *p, int biaya);
void ubahStat(Player *p, int idx, int delta);
void ubahKarma(Player *p, int delta);
void tampilStatus(const Player *p);

#endif
