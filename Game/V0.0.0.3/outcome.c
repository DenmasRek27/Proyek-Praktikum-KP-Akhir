#include "outcome.h"
#include "util.h"

static const char *NAMA[5] = {
    "Sangat Menguntungkan", "Sedikit Menguntungkan", "Netral",
    "Tidak Menguntungkan", "Sangat Tidak Menguntungkan"
};
static const int DELTA[5] = { 10, 5, 0, -4, -8 };

Outcome tentukanOutcome(int karma, int bonus) {
    int r = acak(0, 99) - karma / 5 - bonus - 20;
    if (r < 15) return OUT_SANGAT_UNTUNG;
    if (r < 60) return OUT_UNTUNG;
    if (r < 80) return OUT_NETRAL;
    if (r < 90) return OUT_RUGI;
    return OUT_SANGAT_RUGI;
}

const char *namaOutcome(Outcome o) { return NAMA[o]; }
int deltaOutcome(Outcome o)        { return DELTA[o]; }
