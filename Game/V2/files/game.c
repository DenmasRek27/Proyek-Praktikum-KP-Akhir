#include <stdio.h>
#include "game.h"
#include "player.h"
#include "action.h"
#include "ending.h"
#include "util.h"

#define BIAYA_HIDUP 3
#define PEMULIHAN_TAHUNAN 10

static int tentukanLompatan(const Player *p) {
    (void)p;
    return 1;
}

static void prolog(void) {
    bersihkanLayar();
    printf("=== KISAH DI TANAH PERBATASAN ===\n\n");
    printf("Kamu membuka mata di tepi hutan. Tanpa ingatan, tanpa nama, tanpa harta.\n");
    printf("Desa terdekat terlihat samar di kejauhan.\n\n");
    printf("[Tutorial]\n");
    printf("- Statistik: Kesehatan, Mental, Kekayaan, Reputasi (0-100).\n");
    printf("- Tiap tahun: 3 hari aksi lalu fase waktu luang.\n");
    printf("- Aksi terbuka bertahap sesuai umur & memakai Energi harian.\n");
    printf("- Hasil tak pasti punya 5 tingkat (cenderung untung); Karma menggeser peluang.\n");
    printf("- Kesehatan, Mental, atau Kekayaan habis = gagal.\n\n");
    printf("1. Lanjut ke Umur 1\n");
    bacaAngka(1, 1);
    bersihkanLayar();
}

static Ending jalankanTahun(Player *p) {
    int hari;
    Ending e;

    resetEnergiHarian(p);

    for (hari = 1; hari <= 3; hari++) {
        bersihkanLayar();
        printf("######## UMUR %d | Hari Aksi %d/3 ########", p->umur, hari);
        tampilStatus(p);

        jalankanAksiKerja(p);

        e = cekGagal(p);
        if (e != ENDING_NONE) return e;
        tungguEnter();
    }

    bersihkanLayar();
    printf("######## UMUR %d | Waktu Luang ########\n", p->umur);
    tampilStatus(p);
    jalankanWaktuLuang(p);

    e = cekGagal(p);
    if (e != ENDING_NONE) return e;
    tungguEnter();

    ubahStat(p, STAT_KEKAYAAN, -BIAYA_HIDUP);
    ubahStat(p, STAT_KESEHATAN, PEMULIHAN_TAHUNAN);
    printf("\nAkhir tahun: Kekayaan -%d (biaya hidup), Kesehatan +%d (pulih).\n",
           BIAYA_HIDUP, PEMULIHAN_TAHUNAN);
    tungguEnter();
    return cekGagal(p);
}

void jalankanGame(void) {
    Player p;
    Ending e = ENDING_NONE;

    inisialisasiPlayer(&p);
    prolog();
    p.umur = 1;

    while (e == ENDING_NONE) {
        e = jalankanTahun(&p);
        if (e == ENDING_NONE) e = cekMenang(&p);
        if (e == ENDING_NONE) p.umur += tentukanLompatan(&p);
    }
    bersihkanLayar();
    tampilStatus(&p);
    tampilEnding(e);
}
