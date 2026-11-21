#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "util.h"

int main(void) {
    srand((unsigned)time(NULL));
    jalankanGame();
    tungguEnter(); /* agar jendela Dev-C++ tidak langsung tertutup */
    return 0;
}
