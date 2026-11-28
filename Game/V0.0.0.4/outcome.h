#ifndef OUTCOME_H
#define OUTCOME_H

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

#endif
