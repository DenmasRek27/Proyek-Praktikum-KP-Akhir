#ifndef V5_AKSI_H
#define V5_AKSI_H

#include "../core/jalur.h"
#include "../core/player.h"

/* Pohon aksi. Setiap baris adalah satu pilihan yang bisa muncul di buku.
 * induk = INDUK_PAGI berarti muncul di hub Hari Aksi.
 * induk = INDUK_ACAK berarti muncul di hub Hari Acak.
 * induk >= 0 berarti baris itu hanya muncul setelah baris induk dipilih.
 * Inilah yang membuat satu aksi membuka aksi lain di tema yang sama. */

#define INDUK_PAGI (-2)
#define INDUK_ACAK (-1)

/* Mode penyelesaian */
enum { M_NORMAL, M_RISIKO, M_PASTI, M_CERITA };

/* Jenis jarahan */
enum { L_NONE, L_RAMUAN, L_RAMUAN_BAHAN, L_BAHAN, L_EMAS };

/* Latar */
enum { BG_PERBATASAN, BG_HUTAN, BG_DESA, BG_PASAR, BG_SUNGAI, BG_TEMPAAN,
       BG_BAHAYA, BG_RERUNTUHAN, BG_PADANG, BG_MALAM, BG_ALTAR, BG_PUNCAK,
       JUMLAH_BG };

/* Suasana kartu */
enum { VIBE_TENANG, VIBE_KERJA, VIBE_WASPADA, VIBE_TARUNG, VIBE_BERKAH,
       VIBE_DUKA, VIBE_LAIN, JUMLAH_VIBE };

/* Cuaca harian */
enum { CUACA_CERAH, CUACA_GERIMIS, CUACA_BERKABUT, CUACA_BERANGIN,
       CUACA_DINGIN, CUACA_PANAS, JUMLAH_CUACA };

typedef struct { signed char stat; short lo, hi; } Eff;

typedef struct {
    short id;
    short induk;
    signed char jalur;          /* JALUR_* atau JALUR_NONE */
    unsigned char bobot;        /* poin afinitas yang diberikan */
    unsigned char minAge;
    unsigned char energi;
    unsigned char mode;
    unsigned char vibe;
    unsigned char bg;
    unsigned char enc;          /* % memicu pertarungan */
    unsigned char sub;          /* % memicu sub-kejadian */
    signed char karma;
    unsigned char loot;
    short perluFlag;            /* -1 = tanpa syarat */
    short tolakFlag;            /* -1 = tanpa syarat */
    short beriFlag;             /* -1 = tidak memberi */
    unsigned char sekali;       /* 1 = hilang setelah dipakai */
    unsigned char cuacaMin, cuacaMaks;
    Eff e[2];
    Eff bad[2];
    const char *label;          /* kalimat pilihan di kartu */
    const char *lanjut;         /* prosa yang ditulis saat simpul dibuka */
} Aksi;

/* Registry: tabel-tabel dipecah per berkas lalu digabung di aksi_registry.c */
extern const Aksi AKSI_UMUM[];
extern const int JUMLAH_AKSI_UMUM;
extern const Aksi AKSI_PEDANG[];
extern const int JUMLAH_AKSI_PEDANG;
extern const Aksi AKSI_AKAR[];
extern const int JUMLAH_AKSI_AKAR;
extern const Aksi AKSI_KERTAS[];
extern const int JUMLAH_AKSI_KERTAS;
extern const Aksi AKSI_KANTONG[];
extern const int JUMLAH_AKSI_KANTONG;
extern const Aksi AKSI_LANGKAH[];
extern const int JUMLAH_AKSI_LANGKAH;
extern const Aksi AKSI_LENTERA[];
extern const int JUMLAH_AKSI_LENTERA;
extern const Aksi AKSI_ACAK[];
extern const int JUMLAH_AKSI_ACAK;

#define JUMLAH_BAGIAN_AKSI 8

int aksiTotal(void);
const Aksi *aksiKe(int indeks);              /* indeks global 0..aksiTotal()-1 */
const Aksi *aksiCariId(int id);              /* NULL bila tak ada */
const Aksi *aksiBagian(int bagian, int *jumlah);

/* Nomor flag cerita */
enum {
    FLG_HUTAN_MASUK = 0, FLG_HUTAN_DALAM,
    FLG_PEDANG1, FLG_PEDANG2, FLG_PEDANG3,
    FLG_AKAR1, FLG_AKAR2, FLG_AKAR3,
    FLG_KERTAS1, FLG_KERTAS2, FLG_KERTAS3,
    FLG_KANTONG1, FLG_KANTONG2, FLG_KANTONG3,
    FLG_LANGKAH1, FLG_LANGKAH2, FLG_LANGKAH3,
    FLG_LENTERA1, FLG_LENTERA2, FLG_LENTERA3,
    FLG_BOS1, FLG_BOS2, FLG_BOS3,
    FLG_KAWAN_DESA, FLG_KAWAN_HUTAN, FLG_KAWAN_PASAR,
    FLG_LUKA_BERAT, FLG_TENAR,
    FLG_JUMLAH
};

#endif
