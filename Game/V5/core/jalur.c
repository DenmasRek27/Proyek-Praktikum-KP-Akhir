#include "jalur.h"

const char *NAMA_JALUR[JUMLAH_JALUR] = {
    "Jalur Pedang", "Jalur Akar", "Jalur Kertas",
    "Jalur Kantong", "Jalur Langkah", "Jalur Lentera"
};

const char *JALUR_SINGKAT[JUMLAH_JALUR] = {
    "Pedang", "Akar", "Kertas", "Kantong", "Langkah", "Lentera"
};

const char *JALUR_SIFAT[JUMLAH_JALUR] = {
    "tangan yang terbiasa menggenggam baja",
    "tangan yang terbiasa menyentuh tanah dan daun",
    "tangan yang terbiasa memegang pena dan buku",
    "tangan yang terbiasa menghitung koin",
    "kaki yang tak pernah betah diam",
    "mata yang terbiasa menatap cahaya kecil"
};

int jalurTotal(const int *poin) {
    int i, t = 0;
    for (i = 0; i < JUMLAH_JALUR; i++) t += poin[i];
    return t;
}

int jalurDominan(const int *poin) {
    int i, best = JALUR_NONE, nilai = 0;
    for (i = 0; i < JUMLAH_JALUR; i++)
        if (poin[i] > nilai) { nilai = poin[i]; best = i; }
    return best;
}

int jalurKedua(const int *poin, int dominan) {
    int i, best = JALUR_NONE, nilai = 0;
    for (i = 0; i < JUMLAH_JALUR; i++) {
        if (i == dominan) continue;
        if (poin[i] > nilai) { nilai = poin[i]; best = i; }
    }
    return best;
}
