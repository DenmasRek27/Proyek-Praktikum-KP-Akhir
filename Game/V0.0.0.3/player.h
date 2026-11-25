#ifndef PLAYER_H
#define PLAYER_H

#define STAT_MIN 0
#define STAT_MAX 100
#define KARMA_MIN -50
#define KARMA_MAX 50
#define ENERGI_AWAL 3      /* umur 1-16 */
#define ENERGI_DEWASA 5    /* umur > 16 */
#define UMUR_DEWASA 16
#define BONUS_MEMBERSHIP 3

enum { RAMUAN_HEAL, RAMUAN_DMG, RAMUAN_KARMA, JUMLAH_RAMUAN };
enum { EQ_PEDANG, EQ_ZIRAH, EQ_AMULET, JUMLAH_EQ };
enum { STAT_KESEHATAN, STAT_MENTAL, STAT_KEKAYAAN, STAT_REPUTASI, JUMLAH_STAT };

typedef struct {
    int stat[JUMLAH_STAT];
    int karma;
    int umur;
    int energi;
    int energiMaks;
    int bonusEnergi;            /* dari Membership Pelatihan */
    int ramuan[JUMLAH_RAMUAN];
    int bahan[JUMLAH_EQ];       /* bahan crafting per equipment */
    int levelEq[JUMLAH_EQ];     /* -1 = belum punya, 0..3 */
    int buffDmg;                /* sampai akhir combat */
    int buffKarma;              /* sampai akhir hari */
} Player;

extern const char *NAMA_STAT[JUMLAH_STAT];
extern const char *NAMA_RAMUAN[JUMLAH_RAMUAN];
extern const char *NAMA_EQ[JUMLAH_EQ];

void inisialisasiPlayer(Player *p);
void resetEnergiHarian(Player *p);
int pakaiEnergi(Player *p, int biaya);
void ubahStat(Player *p, int idx, int delta);
void ubahKarma(Player *p, int delta);
void tampilStatus(const Player *p);
int karmaEfektif(const Player *p);          /* karma + amulet + buff */
int efekEq(const Player *p, int eq);        /* 0 bila belum punya */
int pakaiRamuan(Player *p, int combat);     /* 1 bila ramuan terpakai */

#endif
