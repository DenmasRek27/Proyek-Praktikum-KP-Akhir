#include <stdio.h>
#include <string.h>
#include "combat.h"
#include "outcome.h"
#include "util.h"

typedef enum { TARGET_MUSUH, TARGET_PEDAGANG } JenisTarget;
typedef enum { KONDISI_NORMAL, KONDISI_TERLUKA, KONDISI_SAKIT, KONDISI_KESULITAN } Kondisi;

typedef struct {
    char nama[24];
    JenisTarget jenis;
    Kondisi kondisi;
    int hp;
    int kekuatan;
} Target;

static const char *NAMA_MUSUH[3] = { "Serigala Lapar", "Bandit Jalanan", "Beruang Hutan" };
static const char *LOKASI[4]     = { "hutan", "jalan desa", "tepi sungai", "gerbang kota" };

/* Buat target acak: 60% musuh, 40% pedagang. Kondisi disembunyikan sampai di-Assessment. */
static void buatTarget(Target *t) {
    int r;
    if (acak(1, 100) <= 60) {
        t->jenis = TARGET_MUSUH;
        strcpy(t->nama, NAMA_MUSUH[acak(0, 2)]);
        t->hp = acak(25, 40);
        t->kekuatan = acak(3, 6);
        r = acak(1, 3);
        if (r == 2) { t->kondisi = KONDISI_TERLUKA; t->hp /= 2; }
        else if (r == 3) t->kondisi = KONDISI_SAKIT;
        else t->kondisi = KONDISI_NORMAL;
    } else {
        t->jenis = TARGET_PEDAGANG;
        strcpy(t->nama, "Pedagang Keliling");
        t->hp = 20;
        t->kekuatan = 0;
        t->kondisi = (acak(1, 3) <= 2) ? KONDISI_KESULITAN : KONDISI_NORMAL;
    }
}

/* Assessment: tampilkan kondisi target (tidak mengubah apa pun, hanya info) */
static void assessment(const Target *t) {
    printf("[Assessment] %s ", t->nama);
    switch (t->kondisi) {
        case KONDISI_TERLUKA:    printf("memiliki luka terbuka, geraknya melambat.\n"); break;
        case KONDISI_SAKIT:      printf("terbatuk dan lemas, jelas sedang sakit.\n"); break;
        case KONDISI_KESULITAN:  printf("kesulitan menjual dagangannya dan butuh pembeli.\n"); break;
        default:                 printf("tampak sehat dan tidak terburu-buru.\n"); break;
    }
}

/* Return 1 bila encounter selesai */
static int serang(Player *p, Target *t, int assessed) {
    int dmg;
    if (t->jenis == TARGET_PEDAGANG) {
        printf("Kamu merampok pedagang itu! Hartanya bertambah, namamu tercoreng.\n");
        ubahStat(p, STAT_KEKAYAAN, 8);
        ubahStat(p, STAT_REPUTASI, -10);
        ubahKarma(p, -10);
        return 1;
    }
    dmg = acak(8, 14);
    if (assessed && (t->kondisi == KONDISI_TERLUKA || t->kondisi == KONDISI_SAKIT)) {
        dmg += 6; /* bonus dari hasil Assessment */
        printf("Kamu menyerang celah kelemahannya!\n");
    }
    t->hp -= dmg;
    printf("Seranganmu menimbulkan %d damage.\n", dmg);
    if (t->hp <= 0) {
        printf("%s tumbang. Kamu menjarah sedikit bekal.\n", t->nama);
        ubahStat(p, STAT_KEKAYAAN, 5);
        ubahStat(p, STAT_REPUTASI, 3);
        return 1;
    }
    return 0;
}

static int negosiasi(Player *p, Target *t, int assessed) {
    /* Assessment pada pedagang yang kesulitan memberi bonus peluang */
    int bonus = (assessed && t->kondisi == KONDISI_KESULITAN) ? 20 : 0;
    Outcome o = tentukanOutcome(p->karma, bonus);
    int untung;
    printf("Hasil negosiasi: %s\n", namaOutcome(o));
    if (t->jenis == TARGET_PEDAGANG) {
        untung = deltaOutcome(o) + (bonus ? 5 : 0);
        ubahStat(p, STAT_KEKAYAAN, untung);
        if (untung > 0) ubahKarma(p, 1);
        printf("Transaksi selesai (kekayaan %+d).\n", untung);
        return 1;
    }
    if (o <= OUT_UNTUNG) {
        printf("%s mengurungkan niatnya dan pergi.\n", t->nama);
        ubahStat(p, STAT_REPUTASI, 2);
        ubahKarma(p, 2);
        return 1;
    }
    printf("%s tidak menggubris bicaramu.\n", t->nama);
    return 0;
}

static void pakaiItem(Player *p) {
    p->ramuan--;
    ubahStat(p, STAT_KESEHATAN, 20);
    printf("Kamu meminum ramuan. Kesehatan +20.\n");
}

static int kabur(Player *p, Target *t) {
    if (t->jenis == TARGET_PEDAGANG) {
        printf("Kamu pergi begitu saja.\n");
        return 1;
    }
    if (tentukanOutcome(p->karma, 0) <= OUT_NETRAL) {
        printf("Kamu berhasil kabur.\n");
        return 1;
    }
    printf("Kamu gagal kabur!\n");
    return 0;
}

static void giliranMusuh(Player *p, const Target *t) {
    int dmg = t->kekuatan + acak(0, 3);
    ubahStat(p, STAT_KESEHATAN, -dmg);
    printf("%s menyerangmu: Kesehatan -%d.\n", t->nama, dmg);
}

void mulaiEncounter(Player *p) {
    Target t;
    int assessed = 0, selesai = 0, pilih;

    buatTarget(&t);
    printf("\nKamu bertemu %s di %s, apa yang kamu mau lakukan?\n",
           t.nama, LOKASI[acak(0, 3)]);

    while (!selesai && p->stat[STAT_KESEHATAN] > 0) {
        printf("\n[Kesehatan %d | Ramuan %d]\n", p->stat[STAT_KESEHATAN], p->ramuan);
        printf("1. Serang\n2. Aksi (Negosiasi/Bicara)\n3. Item (Ramuan)\n4. Assessment\n5. Kabur\n");
        pilih = bacaAngka(1, 5);

        if (pilih == 3 && p->ramuan <= 0) {
            printf("Kamu tidak punya ramuan.\n");
            continue; /* tidak memakan giliran */
        }
        if (pilih == 4 && assessed) {
            printf("Kamu sudah memeriksa target ini.\n");
            continue;
        }

        switch (pilih) {
            case 1: selesai = serang(p, &t, assessed); break;
            case 2: selesai = negosiasi(p, &t, assessed); break;
            case 3: pakaiItem(p); break;
            case 4: assessed = 1; assessment(&t); break;
            case 5: selesai = kabur(p, &t); break;
        }
        /* Musuh membalas bila encounter belum selesai */
        if (!selesai && t.jenis == TARGET_MUSUH) giliranMusuh(p, &t);
    }
}
