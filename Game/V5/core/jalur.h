#ifndef V5_JALUR_H
#define V5_JALUR_H

/* Jalur = tema hidup. Aksi menambah poin jalur; poin menggeser pilihan yang
 * muncul kemudian dan menentukan ending bertema. */

enum { JALUR_NONE = -1 };
enum {
    JALUR_PEDANG,      /* petarung, pemburu, penjaga jalanan */
    JALUR_AKAR,        /* herbalis, perawat, penjaga alam */
    JALUR_KERTAS,      /* cendekia, penulis, peneliti */
    JALUR_KANTONG,     /* pedagang, negosiator, perantara */
    JALUR_LANGKAH,     /* pengembara, pengintai, penjelajah */
    JALUR_LENTERA,     /* mistik, roh, upacara */
    JUMLAH_JALUR
};

#define JALUR_MAKS 99

extern const char *NAMA_JALUR[JUMLAH_JALUR];
extern const char *JALUR_SINGKAT[JUMLAH_JALUR];
extern const char *JALUR_SIFAT[JUMLAH_JALUR];

int jalurDominan(const int *poin);                    /* -1 bila semua nol */
int jalurKedua(const int *poin, int dominan);         /* -1 bila tak ada */
int jalurTotal(const int *poin);

#endif
