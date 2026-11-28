#include "outcome.h"
#include "util.h"

static const char *NAMA[5] = {
    "Sangat Menguntungkan", "Menguntungkan", "Netral",
    "Tidak Menguntungkan", "Sangat Tidak Menguntungkan"
};
static const int DELTA[5] = { 10, 5, 0, -4, -8 };

/* Lemparan 0..99 digeser karma (tiap 5 karma = 1 poin), bonus aksi, dan cuaca.
 * Band: 0-11 sangat untung, 12-51 untung, 52-73 netral, 74-87 rugi, 88-99 sangat rugi. */
Outcome tentukanOutcome(int karma, int bonus) {
    int r = acak(0, 99) - karma / 5 - bonus - 12;
    if (r < 12) return OUT_SANGAT_UNTUNG;
    if (r < 52) return OUT_UNTUNG;
    if (r < 74) return OUT_NETRAL;
    if (r < 88) return OUT_RUGI;
    return OUT_SANGAT_RUGI;
}

const char *namaOutcome(Outcome o) {
    if (o < 0 || o > OUT_SANGAT_RUGI) return NAMA[OUT_NETRAL];
    return NAMA[o];
}

int deltaOutcome(Outcome o) {
    if (o < 0 || o > OUT_SANGAT_RUGI) return 0;
    return DELTA[o];
}

int outcomeBagus(Outcome o) {
    return o == OUT_SANGAT_UNTUNG || o == OUT_UNTUNG;
}
