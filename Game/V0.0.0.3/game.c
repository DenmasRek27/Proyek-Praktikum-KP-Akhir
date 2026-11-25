#include <stdio.h>
#include "game.h"
#include "player.h"
#include "action.h"
#include "ending.h"
#include "combat.h"
#include "util.h"

#define BIAYA_HIDUP 3
#define PEMULIHAN_TAHUNAN 10

#define HARI_AKSI 3
#define HARI_ACAK 2

static const int UMUR_BOSS[3] = { 16, 46, 86 };

/* +5 sampai umur 16, setelah itu +10 */
static int tentukanLompatan(const Player *p) {
    return p->umur < UMUR_DEWASA ? 5 : 10;
}

static void prolog(void) {
    bersihkanLayar();
    printf("=== KISAH DI TANAH PERBATASAN ===\n\n");
    printf("Kamu membuka mata di tepi hutan. Tanpa ingatan, tanpa nama, tanpa harta.\n");
    printf("Desa terdekat terlihat samar di kejauhan.\n\n");
    printf("[Tutorial]\n");
    printf("- Statistik: Kesehatan, Mental, Kekayaan, Reputasi (0-100).\n");
    printf("- Tiap Umur: 3 Hari Aksi, lalu 2 Hari Acak.\n");
    printf("- Tiap Hari Aksi kamu punya Energi untuk beberapa aktivitas, atau ke Toko/Pandai Besi.\n");
    printf("- Umur melompat: +5 sampai umur 16, lalu +10. Ada boss di umur 16, 46, dan 86.\n");
    printf("- Hasil tak pasti punya 5 tingkat (cenderung untung); Karma menggeser peluang.\n");
    printf("- Kesehatan, Mental, atau Kekayaan habis = gagal. Bertahan sampai umur 106 = tamat.\n\n");
    printf("1. Lanjut ke Umur 1\n");
    bacaAngka(1, 1);
    bersihkanLayar();
}

static Ending jalankanTahun(Player *p) {
    int hari, b;
    Ending e;

    for (hari = 1; hari <= HARI_AKSI; hari++) {
        resetEnergiHarian(p);
        bersihkanLayar();
        printf("######## UMUR %d | Hari Aksi %d/%d ########", p->umur, hari, HARI_AKSI);
        while (p->energi > 0) {
            tampilStatus(p);
            if (jalankanAksiKerja(p)) break;
            e = cekGagal(p);
            if (e != ENDING_NONE) return e;
        }
        tungguEnter();
    }

    for (hari = 1; hari <= HARI_ACAK; hari++) {
        resetEnergiHarian(p);
        bersihkanLayar();
        printf("######## UMUR %d | Hari Acak %d/%d ########", p->umur, hari, HARI_ACAK);
        tampilStatus(p);
        jalankanWaktuLuang(p);
        e = cekGagal(p);
        if (e != ENDING_NONE) return e;
        tungguEnter();
    }

    for (b = 0; b < 3; b++) {
        if (p->umur != UMUR_BOSS[b]) continue;
        bersihkanLayar();
        mulaiBoss(p, b);
        e = cekGagal(p);
        if (e != ENDING_NONE) return e;
        if (b == 0) printf("\nBantuan berakhir. Selamat datang di permainan yang sebenarnya.\n");
        tungguEnter();
    }

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
        if (e == ENDING_NONE) p.umur += tentukanLompatan(&p);
        if (e == ENDING_NONE) e = cekMenang(&p); /* baru aktif di umur 106 */
    }
    bersihkanLayar();
    tampilStatus(&p);
    tampilEnding(e);
}
