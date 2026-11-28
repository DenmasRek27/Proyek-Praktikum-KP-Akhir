#ifndef COMBAT_H
#define COMBAT_H

#include "player.h"

/* Satu encounter berbasis giliran dengan target acak (musuh / pedagang) */
void mulaiEncounter(Player *p);
/* Boss tingkat 0..2, tanpa kabur/negosiasi */
void mulaiBoss(Player *p, int tingkat);

#endif
