/* main.c - Main game loop Syntax Odyssey
 * Build: gcc -std=c99 -Wall -Wextra -pedantic *.c -o syntax_odyssey */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chara.h"
#include "events.h"
#include "ui.h"

/* Format penyimpanan riwayat pilihan hidup (ARRAY of struct) */
typedef struct { int age; const char *title; char pick; } History;

/* Efek penuaan alami: makin tua, kesehatan makin turun (percabangan) */
static void natural_aging(Character *c) {
    if (c->age < 30)      character_apply(c, 0, 0, 0, 0);
    else if (c->age < 50) character_apply(c, -2, 0, 0, 0);
    else                  character_apply(c, -5, -2, 0, 0);
}

/* Ringkasan akhir hidup + riwayat pilihan */
static void ending(const Character *c, const History h[], int n, const char *reason) {
    int i, avg = (c->health + c->mental + c->wealth + c->reputation) / 4;
    ui_clear();
    printf("========== ENDGAME SUMMARY ==========\n");
    if (reason) printf("GAME OVER pada usia %d: %s\n\n", c->age, reason);
    else        printf("%s menjalani hidup penuh hingga usia %d!\n\n", c->name, c->age);
    ui_status(c);
    if (avg >= 70)      printf("Gelar: Legenda yang Dikenang\n");
    else if (avg >= 45) printf("Gelar: Manusia Biasa yang Bahagia\n");
    else                printf("Gelar: Jiwa yang Penuh Penyesalan\n");
    printf("\nRiwayat pilihan hidup:\n");
    for (i = 0; i < n; i++)
        printf("  Usia %2d - %-26s -> %c\n", h[i].age, h[i].title, h[i].pick);
    printf("\nTerima kasih sudah bermain Syntax Odyssey!\n");
}

int main(void) {
    Character hero;
    History history[MAX_PHASE];
    int used[64] = {0};            /* penanda event yang sudah muncul */
    int hist_n = 0;
    char name[NAME_LEN];
    const char *reason = NULL;

    srand((unsigned)time(NULL));
    ui_title();
    ui_read_name(name, sizeof name);
    character_init(&hero, name);

    /* MAIN LOOP: usia naik per fase sampai MAX_AGE atau karakter gugur */
    while (hero.age <= MAX_AGE && !reason) {
        int idx = events_pick(hero.age, used);
        const Event *ev = &EVENT_DB[idx];
        used[idx] = 1;

        ui_clear();
        ui_status(&hero);
        char pick = ui_event(ev);

        /* Percabangan: terapkan konsekuensi sesuai pilihan */
        const Choice *ch = (pick == 'A') ? &ev->a : &ev->b;
        character_apply(&hero, ch->health, ch->mental, ch->wealth, ch->reputation);
        printf("\n>> %s\n", ch->result);
        printf("   (Kes %+d | Men %+d | Kek %+d | Rep %+d)\n",
               ch->health, ch->mental, ch->wealth, ch->reputation);

        history[hist_n++] = (History){hero.age, ev->title, pick};

        natural_aging(&hero);
        reason = character_dead_reason(&hero);   /* cek game over */
        ui_pause();
        hero.age += AGE_STEP;
    }
    if (hero.age > MAX_AGE) hero.age = MAX_AGE;  /* rapikan tampilan usia akhir */
    ending(&hero, history, hist_n, reason);
    return 0;
}
