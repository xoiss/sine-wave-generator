/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtrig.h"
#include <assert.h>

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the sine given a momentary phase, signed fixed point 0.21-bit version. */
sq021_t sin_sq021(const uq016_t phi) {
    return sq021_from_uq022(uq022_from_uq016(phi));     /* skeleton */
}

/*--------------------------------------------------------------------------------------------------------------------*/
