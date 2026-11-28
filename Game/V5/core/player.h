#ifndef V5_PLAYER_H
#define V5_PLAYER_H

#include "jalur.h"

#define STAT_MIN 0
#define STAT_MAX 100
#define KARMA_MIN (-50)
#define KARMA_MAX 50
#define ENERGI_AWAL 3        /* umur <= 16 */
#define ENERGI_DEWASA 5      /* umur > 16 */
#define UMUR_DEWASA 16
#define BONUS_MEMBERSHIP 3
#define UMUR_MIN_MENANG 106
#define JUMLAH_FLAG 128      /* 16 byte bitfield */

enum { STAT_KESEHATAN, STAT_MENTAL, STAT_KEKAYAAN, STAT_REPUTASI, JUMLAH_STAT };
enum { RAMUAN_HEAL, RAMUAN_DMG, RAMUAN_KARMA, RAMUAN_SEGAR, RAMUAN_TENANG, JUMLAH_RAMUAN };
enum { EQ_PEDANG, EQ_ZIRAH, EQ_AMULET, JUMLAH_EQ };

typedef struct {
    int stat[JUMLAH_STAT];
    int karma;
    int umur;
    int energi;
    int energiMaks;
    int bonusEnergi;
    int ramuan[JUMLAH_RAMUAN];
    int bahan[JUMLAH_EQ];
    int levelEq[JUMLAH_EQ];        /* -1 = belum dirakit, 0..3 */
    int buffDmg;
    int buffKarma;
    int jalur[JUMLAH_JALUR];       /* poin afinitas per jalur */
    unsigned char flag[JUMLAH_FLAG / 8];
} Player;

extern const char *NAMA_STAT[JUMLAH_STAT];
extern const char *NAMA_RAMUAN[JUMLAH_RAMUAN];
extern const char *NAMA_EQ[JUMLAH_EQ];
extern const int HARGA_RAMUAN[JUMLAH_RAMUAN];

void inisialisasiPlayer(Player *p);
void resetEnergiHarian(Player *p);
int pakaiEnergi(Player *p, int biaya);
void ubahStat(Player *p, int idx, int delta);
void ubahKarma(Player *p, int delta);
void tambahJalur(Player *p, int jalur, int poin);
int karmaEfektif(const Player *p);
int efekEq(const Player *p, int eq);
int jalurPoin(const Player *p, int jalur);

void setFlag(Player *p, int flag);
int punyaFlag(const Player *p, int flag);

#endif
