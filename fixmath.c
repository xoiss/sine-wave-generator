/**@file
 * @brief   Implementation of arithmetic functions on fixed point data types.
 * @details This file implements arithmetic functions in the field of fixed point numbers.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixmath.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the product of two fixed point values, unsigned fixed point 0.16-bit version. */
uq016_t qmul_uq016(const uq016 a, const uq016 b) {
    return ((ui32_t)a * (ui32_t)b) >> UQ016_FRAC;
}

/*--------------------------------------------------------------------------------------------------------------------*/
