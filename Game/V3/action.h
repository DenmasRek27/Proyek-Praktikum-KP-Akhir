#ifndef ACTION_H
#define ACTION_H

#include "player.h"

/* Satu aktivitas dalam hari aksi. Return 1 bila pemain mengakhiri hari. */
int jalankanAksiKerja(Player *p);
void jalankanWaktuLuang(Player *p);

#endif
