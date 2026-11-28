/* ui.h - Tampilan konsol: ASCII art, layar, input */
#ifndef UI_H
#define UI_H
#include "chara.h"
#include "events.h"

void ui_clear(void);
void ui_pause(void);                       /* tunggu ENTER */
void ui_title(void);                       /* Title Screen */
void ui_read_name(char *buf, int size);    /* input nama */
void ui_status(const Character *c);        /* bar statistik */
char ui_event(const Event *e);             /* tampilkan event, return 'A'/'B' */

#endif
