#ifndef ENDING_H
#define ENDING_H

#include "player.h"

#define UMUR_MIN_MENANG 106

typedef enum {
    ENDING_NONE,
    ENDING_GAGAL_KESEHATAN,
    ENDING_GAGAL_MENTAL,
    ENDING_GAGAL_MISKIN,
    ENDING_MANDIRI,
    ENDING_KOMUNITAS,
    ENDING_BERTAHAN   /* sampai umur 106 tanpa syarat khusus */
} Ending;

Ending cekGagal(const Player *p);   /* dicek setiap hari */
Ending cekMenang(const Player *p);  /* dicek di akhir tahun */
void tampilEnding(Ending e);

#endif
