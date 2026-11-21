/* events.h - Struct Event & database event (array) */
#ifndef EVENTS_H
#define EVENTS_H

/* STRUCT: satu pilihan beserta konsekuensinya */
typedef struct {
    const char *text;     /* teks pilihan             */
    const char *result;   /* narasi hasil pilihan     */
    int health, mental, wealth, reputation; /* perubahan stat */
} Choice;

/* STRUCT: satu Special Encounter */
typedef struct {
    int min_age, max_age;   /* rentang usia event ini muncul */
    const char *title;
    const char *story;
    Choice a, b;            /* tepat 2 opsi: A dan B */
} Event;

extern const Event EVENT_DB[];   /* array daftar event */
extern const int   EVENT_COUNT;

/* Pilih event acak yang cocok dengan usia & belum terpakai (used[]) */
int events_pick(int age, int used[]);

#endif
