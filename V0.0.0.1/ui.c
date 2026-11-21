/* ui.c - Implementasi tampilan */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ui.h"

/* Bersihkan layar: aman lintas OS (Windows = cls, lainnya = clear) */
void ui_clear(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ui_pause(void) {
    char buf[8];
    printf("\n[ENTER] untuk lanjut...");
    fgets(buf, sizeof buf, stdin);
}

/* ASCII art sederhana; backslash harus ditulis \\ di string C */
void ui_title(void) {
    ui_clear();
    printf(
    "  ____              _              \n"
    " / ___| _   _ _ __ | |_ __ ___  __ \n"
    " \\___ \\| | | | '_ \\| __/ _` \\ \\/ / \n"
    "  ___) | |_| | | | | || (_| |>  <  \n"
    " |____/ \\__, |_| |_|\\__\\__,_/_/\\_\\ \n"
    "        |___/   O D Y S S E Y      \n\n"
    "    @   Life Sim | Adventure | RPG   \n"
    "   /|\\  Dari usia %d hingga %d tahun \n"
    "   / \\  ------------------------------\n", START_AGE, MAX_AGE);
}

void ui_read_name(char *buf, int size) {
    printf("\nMasukkan nama karakter: ");
    if (!fgets(buf, size, stdin) || buf[0] == '\n') strcpy(buf, "Pemain");
    buf[strcspn(buf, "\n")] = '\0';   /* buang newline */
}

/* Gambar bar [#####-----] untuk nilai 0-100 */
static void bar(const char *label, int v) {
    int i, filled = v / 10;
    printf("  %-11s [", label);
    for (i = 0; i < 10; i++) putchar(i < filled ? '#' : '-');
    printf("] %3d\n", v);
}

void ui_status(const Character *c) {
    printf("=== %s | Usia: %d ===\n", c->name, c->age);
    bar("Kesehatan", c->health);
    bar("Mental", c->mental);
    bar("Kekayaan", c->wealth);
    bar("Reputasi", c->reputation);
    printf("--------------------------------------\n");
}

/* Tampilkan event, validasi input sampai 'A' atau 'B' */
char ui_event(const Event *e) {
    char buf[16], pick = 0;
    printf("\n*** SPECIAL ENCOUNTER: %s ***\n\n%s\n\n", e->title, e->story);
    printf("  [A] %s\n  [B] %s\n", e->a.text, e->b.text);
    while (pick != 'A' && pick != 'B') {
        printf("\nPilihanmu (A/B): ");
        if (fgets(buf, sizeof buf, stdin)) pick = (char)toupper((unsigned char)buf[0]);
    }
    return pick;
}
