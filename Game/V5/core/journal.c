#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "journal.h"

void jurnalReset(Journal *j) {
    j->teks[0] = 0;
    j->panjang = 0;
    j->versi++;
}

static void tambahBebas(Journal *j, const char *b) {
    int n = (int)strlen(b);
    if (j->panjang + n >= JURNAL_MAKS - 1) return;   /* penuh: diam-diam berhenti, bukan menimpa */
    memcpy(j->teks + j->panjang, b, (size_t)n + 1);
    j->panjang += n;
    j->versi++;
}

void jurnalTulis(Journal *j, const char *fmt, ...) {
    char b[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    if (j->panjang > 0) tambahBebas(j, "\n\n");
    tambahBebas(j, b);
}

void jurnalSambung(Journal *j, const char *fmt, ...) {
    char b[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    tambahBebas(j, b);
}

void jurnalParagraf(Journal *j) {
    if (j->panjang > 0 && j->teks[j->panjang - 1] != '\n') tambahBebas(j, "\n\n");
}
