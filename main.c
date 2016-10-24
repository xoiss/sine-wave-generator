#include "sinegen.h"

#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int i;
    for (i = 0; i < 65536*2; i++)
        printf("%i\n", geni(1));
    fflush(stdout);
    return EXIT_SUCCESS;
}
