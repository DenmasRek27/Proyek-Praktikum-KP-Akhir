#include <stdio.h>
#include "shop.h"
#include "util.h"

#define HARGA_BAHAN 5
#define HARGA_MEMBERSHIP 40
#define HARGA_RAKIT 10
#define BAHAN_PER_EQ 3
#define MENU_MEMBERSHIP (JUMLAH_RAMUAN + JUMLAH_EQ + 1)
#define MENU_KELUAR (MENU_MEMBERSHIP + 1)

static const int HARGA_RAMUAN[JUMLAH_RAMUAN] = { 6, 8, 8 };
static const int HARGA_UPGRADE[3] = { 15, 30, 50 }; /* ke level 1, 2, 3 */
static const char *NAMA_BAHAN[JUMLAH_EQ] = { "Bijih Besi", "Kulit Keras", "Batu Permata" };

/* Belanja tidak boleh membuat Kekayaan 0 (itu = gagal) */
static int bayar(Player *p, int harga) {
    if (p->stat[STAT_KEKAYAAN] <= harga) { printf("Kekayaanmu tidak cukup.\n"); return 0; }
    ubahStat(p, STAT_KEKAYAAN, -harga);
    return 1;
}

void bukaToko(Player *p) {
    int i, n;
    while (1) {
        printf("\n[Toko] Kekayaan: %d\n", p->stat[STAT_KEKAYAAN]);
        for (i = 0; i < JUMLAH_RAMUAN; i++)
            printf("%d. %s - %d\n", i + 1, NAMA_RAMUAN[i], HARGA_RAMUAN[i]);
        for (i = 0; i < JUMLAH_EQ; i++)
            printf("%d. %s (bahan %s) - %d\n", JUMLAH_RAMUAN + i + 1, NAMA_BAHAN[i], NAMA_EQ[i], HARGA_BAHAN);
        printf("%d. Membership Pelatihan (Energi harian +%d) - %d%s\n", MENU_MEMBERSHIP,
               BONUS_MEMBERSHIP, HARGA_MEMBERSHIP, p->bonusEnergi ? " [dimiliki]" : "");
        printf("%d. Keluar\n", MENU_KELUAR);
        n = bacaAngka(1, MENU_KELUAR);
        if (n == MENU_KELUAR) return;
        if (n <= JUMLAH_RAMUAN) {
            if (bayar(p, HARGA_RAMUAN[n - 1])) p->ramuan[n - 1]++;
        } else if (n < MENU_MEMBERSHIP) {
            if (bayar(p, HARGA_BAHAN)) p->bahan[n - JUMLAH_RAMUAN - 1]++;
        } else if (p->bonusEnergi) {
            printf("Kamu sudah menjadi anggota.\n");
        } else if (bayar(p, HARGA_MEMBERSHIP)) {
            p->bonusEnergi = BONUS_MEMBERSHIP;
            printf("Kamu resmi anggota pelatihan. Energi harian naik mulai hari berikutnya.\n");
        }
    }
}

void bukaPandaiBesi(Player *p) {
    int i, n, lv;
    while (1) {
        printf("\n[Pandai Besi] Kekayaan: %d\n", p->stat[STAT_KEKAYAAN]);
        for (i = 0; i < JUMLAH_EQ; i++) {
            lv = p->levelEq[i];
            if (lv < 0)
                printf("%d. Rakit %s (butuh %d %s, punya %d) - %d\n", i + 1, NAMA_EQ[i],
                       BAHAN_PER_EQ, NAMA_BAHAN[i], p->bahan[i], HARGA_RAKIT);
            else if (lv < 3)
                printf("%d. Tingkatkan %s Lv%d -> Lv%d - %d\n", i + 1, NAMA_EQ[i], lv, lv + 1, HARGA_UPGRADE[lv]);
            else
                printf("%d. %s (Lv3, maksimal)\n", i + 1, NAMA_EQ[i]);
        }
        printf("%d. Keluar\n", JUMLAH_EQ + 1);
        n = bacaAngka(1, JUMLAH_EQ + 1);
        if (n > JUMLAH_EQ) return;
        i = n - 1;
        lv = p->levelEq[i];
        if (lv < 0) {
            if (p->bahan[i] < BAHAN_PER_EQ) printf("Bahan kurang.\n");
            else if (bayar(p, HARGA_RAKIT)) {
                p->bahan[i] -= BAHAN_PER_EQ;
                p->levelEq[i] = 0;
                printf("%s selesai dirakit!\n", NAMA_EQ[i]);
            }
        } else if (lv < 3) {
            if (bayar(p, HARGA_UPGRADE[lv])) {
                p->levelEq[i]++;
                printf("%s naik ke Lv%d!\n", NAMA_EQ[i], p->levelEq[i]);
            }
        } else printf("Sudah level maksimal.\n");
    }
}
