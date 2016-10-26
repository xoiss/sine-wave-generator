#include "fixtrig.h"

#include <stdlib.h>
#include <stdio.h>

int main(void) {
    uq016_t phi = 0;
    do {
        printf("%hu, %li\n", phi, sin_sq021(phi));
    } while (++phi != 0);
    fflush(stdout);
    return EXIT_SUCCESS;
}
