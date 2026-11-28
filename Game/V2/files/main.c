#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "util.h"

int main(void) {
    srand((unsigned)time(NULL));
    jalankanGame();
    tungguEnter(); /* biar window tidak tertutup */
    return 0;
}
