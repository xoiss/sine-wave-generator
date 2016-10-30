/**@file
 * @brief   Interface to arithmetic functions on fixed point data types.
 * @details This file provides declarations of arithmetic functions in the field of fixed point numbers.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef FIXMATH_H
#define FIXMATH_H

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Returns the product of two fixed point values, unsigned fixed point 0.16-bit version.
 * @param[in]   a   -- the first multiplicand.
 * @param[in]   b   -- the second multiplicand.
 * @return  The product a*b.
 * @details The domain of the defined function is the superposition of all allowed values of \p a and \p b, which are in
 *  turn both belong to the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16] with resolution of 1/2^16.
 * @details The codomain of the defined function is the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16]
 *  with resolution of 1/2^16.
 * @details The product is rounded down to the nearest value with resolution of 1/2^16.
 */
extern uq016_t qmul_uq016(const uq016_t a, const uq016_t b);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* FIXMATH_H */
