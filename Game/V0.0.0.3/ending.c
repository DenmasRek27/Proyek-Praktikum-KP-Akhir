#include <stdio.h>
#include "ending.h"

Ending cekGagal(const Player *p) {
    if (p->stat[STAT_KESEHATAN] <= 0) return ENDING_GAGAL_KESEHATAN;
    if (p->stat[STAT_MENTAL] <= 0)    return ENDING_GAGAL_MENTAL;
    if (p->stat[STAT_KEKAYAAN] <= 0)  return ENDING_GAGAL_MISKIN;
    return ENDING_NONE;
}

Ending cekMenang(const Player *p) {
    int i, seimbang = 1;
    if (p->umur < UMUR_MIN_MENANG) return ENDING_NONE;

    /* Komunitas: dikenal warga dan hidup stabil */
    if (p->stat[STAT_REPUTASI] >= 65 && p->stat[STAT_KESEHATAN] >= 40 &&
        p->stat[STAT_MENTAL] >= 40 && p->stat[STAT_KEKAYAAN] >= 40)
        return ENDING_KOMUNITAS;

    /* Mandiri: semua stat seimbang di atas 55 */
    for (i = 0; i < JUMLAH_STAT; i++)
        if (p->stat[i] < 55) seimbang = 0;
    if (seimbang) return ENDING_MANDIRI;

    return ENDING_BERTAHAN;
}

void tampilEnding(Ending e) {
    printf("\n========== TAMAT ==========\n");
    switch (e) {
        case ENDING_GAGAL_KESEHATAN:
            printf("Tubuhmu kolaps oleh luka dan penyakit. Kamu tak bangun lagi.\n"); break;
        case ENDING_GAGAL_MENTAL:
            printf("Pikiranmu runtuh. Orang desa melihatmu mengigau di tengah hutan.\n"); break;
        case ENDING_GAGAL_MISKIN:
            printf("Kamu jatuh miskin dan kelaparan tanpa ada yang menolong.\n"); break;
        case ENDING_MANDIRI:
            printf("Hidup Mandiri Seimbang: kamu bertahan sendiri dengan tubuh, pikiran, dan harta terjaga.\n"); break;
        case ENDING_KOMUNITAS:
            printf("Membaur Menjadi Rakyat Biasa: kamu menemukan tempat dan peran di tengah komunitas.\n"); break;
        case ENDING_BERTAHAN:
            printf("Kamu mencapai umur 106. Hidupmu panjang, meski tak menonjol.\n"); break;
        default: break;
    }
}
