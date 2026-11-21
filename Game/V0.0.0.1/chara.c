/* character.c - Implementasi fungsi karakter */
#include <string.h>
#include "chara.h"

/* Batasi nilai statistik ke rentang 0..100 */
static int clamp(int v) {
    if (v < 0)   return 0;
    if (v > 100) return 100;
    return v;
}

void character_init(Character *c, const char *name) {
    strncpy(c->name, name, NAME_LEN - 1);
    c->name[NAME_LEN - 1] = '\0';
    c->age = START_AGE;
    c->health = c->mental = c->wealth = c->reputation = 50;
}

/* Terapkan konsekuensi pilihan ke 4 statistik sekaligus */
void character_apply(Character *c, int dh, int dm, int dw, int dr) {
    c->health     = clamp(c->health + dh);
    c->mental     = clamp(c->mental + dm);
    c->wealth     = clamp(c->wealth + dw);
    c->reputation = clamp(c->reputation + dr);
}

const char *character_dead_reason(const Character *c) {
    if (c->health     <= 0) return "Kesehatan habis - tubuhmu menyerah.";
    if (c->mental     <= 0) return "Mental hancur - kamu kehilangan arah hidup.";
    if (c->wealth     <= 0) return "Bangkrut total - hidupmu terpuruk dalam utang.";
    if (c->reputation <= 0) return "Reputasi hancur - semua orang menjauhimu.";
    return NULL;
}
