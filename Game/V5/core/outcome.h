#ifndef V5_OUTCOME_H
#define V5_OUTCOME_H

/* Satu dadu untuk seluruh permainan: lima tingkat hasil.
 * Karma, bonus aksi, dan cuaca menggeser lemparan. */

typedef enum {
    OUT_SANGAT_UNTUNG,
    OUT_UNTUNG,
    OUT_NETRAL,
    OUT_RUGI,
    OUT_SANGAT_RUGI
} Outcome;

Outcome tentukanOutcome(int karma, int bonus);
const char *namaOutcome(Outcome o);
int deltaOutcome(Outcome o);
int outcomeBagus(Outcome o);          /* 1 untuk dua tingkat teratas */

#endif
