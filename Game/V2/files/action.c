#include <stdio.h>
#include "action.h"
#include "combat.h"
#include "outcome.h"
#include "util.h"

#define JUMLAH_AKSI 6

typedef struct {
    const char *nama;
    int minUmur;
    int biayaEnergi;
    int statUtama;
    int biayaKes;
    int biayaMental;
} Aksi;

static const Aksi DAFTAR_AKSI[JUMLAH_AKSI] = {
    { "Bermain & mengamati sekitar", 1,  1, STAT_MENTAL,    0, 0 },
    { "Mengambil kayu & ranting",    4,  3, STAT_KEKAYAAN,  1, 0 },
    { "Meramu herbal sederhana",     6,  2, STAT_KESEHATAN, 0, 1 },
    { "Bekerja di ladang",           8,  4, STAT_KEKAYAAN,  2, 0 },
    { "Bergaul di desa",             8,  2, STAT_REPUTASI,  0, 1 },
    { "Berburu di hutan",            12, 5, STAT_KEKAYAAN,  3, 1 }
};

static void subEventHewanTerperangkap(Player *p) {
    int pilih;
    Outcome o;

    printf("\n[Sub-Kejadian] Saat mengambil kayu, kamu menemukan seekor hewan kecil terperangkap jerat!\n");
    printf("Energi tersisa: %d/%d | Karma: %d\n", p->energi, p->energiMaks, p->karma);
    printf("1. Bantu lepaskan (butuh 3 energi -> Karma naik, peluang dapat loot)\n");
    printf("2. Abaikan dan lanjutkan kerja\n");
    pilih = bacaAngka(1, 2);

    if (pilih == 2) {
        printf("Kamu mengabaikan hewan itu dan melanjutkan pekerjaan.\n");
        return;
    }

    if (!pakaiEnergi(p, 3)) {
        printf("Energimu terlalu habis untuk membantu! Kamu terpaksa meninggalkannya.\n");
        return;
    }

    ubahKarma(p, 5);
    printf("Kamu berusaha melepaskan jeratnya. Karma meningkat (+5) untuk keberuntungan ke depan.\n");

    o = tentukanOutcome(p->karma, 10);
    if (o <= OUT_UNTUNG) {
        p->ramuan++;
        ubahStat(p, STAT_MENTAL, 4);
        printf("Hewan terbebas! Kamu menemukan 1 ramuan di dekat semak dan hati terasa lega.\n");
    } else if (o == OUT_NETRAL) {
        ubahStat(p, STAT_MENTAL, 3);
        printf("Hewan terbebas dan lari masuk hutan. Tidak ada loot tambahan, tapi hatimu tenang.\n");
    } else {
        ubahStat(p, STAT_KESEHATAN, -4);
        printf("Hewan itu panik dan menggigit tanganmu sebelum kabur! Kesehatan -4.\n");
    }
}

void jalankanAksiKerja(Player *p) {
    int i, pilih, jumlahTersedia = 0, valid, indeksAsli;
    int indeksAksi[JUMLAH_AKSI];
    const Aksi *a;
    Outcome o;

    for (i = 0; i < JUMLAH_AKSI; i++) {
        if (p->umur >= DAFTAR_AKSI[i].minUmur) {
            indeksAksi[jumlahTersedia++] = i;
        }
    }

    while (1) {
        printf("\nPilih aksi (Energi tersedia: %d/%d):\n", p->energi, p->energiMaks);
        for (i = 0; i < jumlahTersedia; i++) {
            a = &DAFTAR_AKSI[indeksAksi[i]];
            printf("%d. %s [Energi -%d]\n", i + 1, a->nama, a->biayaEnergi);
        }
        printf("%d. Lewati / Pulihkan sedikit energi (+2 Energi, tanpa aksi berat)\n", jumlahTersedia + 1);

        pilih = bacaAngka(1, jumlahTersedia + 1);
        if (pilih == jumlahTersedia + 1) {
            p->energi += 2;
            if (p->energi > p->energiMaks) p->energi = p->energiMaks;
            printf("Kamu menarik napas sejenak. Energi sekarang %d/%d.\n", p->energi, p->energiMaks);
            return;
        }

        a = &DAFTAR_AKSI[indeksAksi[pilih - 1]];
        valid = 1;
        if (p->umur < a->minUmur) {
            printf("Belum cukup umur untuk aksi ini (butuh umur %d).\n", a->minUmur);
            valid = 0;
        } else if (p->energi < a->biayaEnergi) {
            printf("Energi tidak cukup (butuh %d, sisa %d).\n", a->biayaEnergi, p->energi);
            valid = 0;
        }
        if (valid) break;
    }

    pakaiEnergi(p, a->biayaEnergi);
    ubahStat(p, STAT_KESEHATAN, -a->biayaKes);
    ubahStat(p, STAT_MENTAL, -a->biayaMental);

    o = tentukanOutcome(p->karma, 0);
    ubahStat(p, a->statUtama, deltaOutcome(o));
    printf("%s -> %s (%s %+d | Energi sisa %d/%d)\n",
           a->nama, namaOutcome(o), NAMA_STAT[a->statUtama], deltaOutcome(o),
           p->energi, p->energiMaks);

    indeksAsli = indeksAksi[pilih - 1];
    if (indeksAsli == 1 && acak(1, 100) <= 45) {
        subEventHewanTerperangkap(p);
    }
    if (indeksAsli == 2 && o <= OUT_UNTUNG) {
        p->ramuan++;
        printf("Kamu berhasil membuat 1 ramuan.\n");
    }
    if (o == OUT_SANGAT_RUGI && p->umur >= 8) {
        printf("Sesuatu mengganggumu di tengah pekerjaan!\n");
        mulaiEncounter(p);
    }
}

void jalankanWaktuLuang(Player *p) {
    Outcome o;
    int s;

    printf("\nDi waktu luangmu, kamu ");
    if (p->umur >= 8 && acak(1, 100) <= 50) {
        printf("berpapasan dengan seseorang di perjalanan!\n");
        mulaiEncounter(p);
        return;
    }

    o = tentukanOutcome(p->karma, 0);
    s = acak(0, JUMLAH_STAT - 1);
    ubahStat(p, s, deltaOutcome(o));
    printf("menjalani momen santai — %s (%s %+d).\n",
           namaOutcome(o), NAMA_STAT[s], deltaOutcome(o));
}
