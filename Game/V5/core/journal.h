#ifndef V5_JOURNAL_H
#define V5_JOURNAL_H

/* Buku catatan: buffer teks yang hanya bertambah. Tidak ada teks yang dibuang
 * saat halaman penuh - seluruh hidup tertulis di sini dan bisa dibaca ulang.
 * Pemecahan halaman dilakukan oleh UI (ui/book.c) karena butuh ukuran font. */

#define JURNAL_MAKS 65536

typedef struct {
    char teks[JURNAL_MAKS];
    int panjang;
    int versi;          /* naik setiap kali ada tambahan */
} Journal;

void jurnalReset(Journal *j);
void jurnalTulis(Journal *j, const char *fmt, ...);   /* tambah paragraf baru */
void jurnalSambung(Journal *j, const char *fmt, ...); /* lanjut kalimat yang sama */
void jurnalParagraf(Journal *j);                      /* paksa baris baru */

#endif
