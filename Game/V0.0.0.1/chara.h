/* character.h - Struct Karakter & fungsi statistik */
#ifndef CHARACTER_H
#define CHARACTER_H

#define NAME_LEN  32
#define START_AGE 6     /* usia awal permainan   */
#define MAX_AGE   70    /* usia akhir (endgame)  */
#define AGE_STEP  4     /* lompatan usia per fase */
#define MAX_PHASE ((MAX_AGE - START_AGE) / AGE_STEP + 1)

/* STRUCT: data utama pemain */
typedef struct {
    char name[NAME_LEN];
    int  age;
    int  health;      /* Kesehatan 0-100 */
    int  mental;      /* Mental    0-100 */
    int  wealth;      /* Kekayaan  0-100 */
    int  reputation;  /* Reputasi  0-100 */
} Character;

void character_init(Character *c, const char *name);
void character_apply(Character *c, int dh, int dm, int dw, int dr);
/* Mengembalikan nama statistik yang mencapai 0, atau NULL jika masih aman */
const char *character_dead_reason(const Character *c);

#endif
