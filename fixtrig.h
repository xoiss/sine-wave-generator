#ifndef FIXTRIG_H
#define FIXTRIG_H

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"

/**\brief   Returns the sine given a momentary phase, signed fixed point 0.21-bit version.
 * \param[in]   phi     Normalized momentary phase.
 * \return  Momentary amplitude of the function sin(phi).
 * \details The domain of this function is the set of unsigned fixed point 0.16-bit values - i.e., the discrete range
 *  [0.0; +1.0-1/2^16] with resolution of 1/2^16. This range corresponds to the floating point range [0; 2*pi) with
 *  resolution of pi/2^15 radian. For example:
 *  | phase, radian  | fixed point value  | container code |
 *  |----------------|--------------------|----------------|
 *  | 2*pi           | not allowed        | not allowed    |
 *  | 2*pi - pi/2^15 | 0.9999847412109375 | 0xFFFF         |
 *  | 3*pi/2         | 0.75               | 0xC000         |
 *  | pi             | 0.5                | 0x8000         |
 *  | pi/2           | 0.25               | 0x4000         |
 *  | pi/2^15        | 0.0000152587890625 | 0x0001         |
 *  | 0              | 0.0                | 0x0000         |
 * \details The codomain of this function is the set of signed fixed point 0.21-bit values - i.e., the discrete range
 *  [-1.0; +1.0-1/2^21] with resolution of 1/2^21. This range corresponds to the floating point range [-1; +1) with
 *  resolution of 1/2^21. For example:
 *  | amplitude   | fixed point value        | container code |
 *  |-------------|--------------------------|----------------|
 *  | +1          | not achievable           | 0x200000       |
 *  | +1 - 1/2^21 | +0.999999523162841796875 | 0x1FFFFF       |
 *  | +0.75       | +0.75                    | 0x180000       |
 *  | +0.5        | +0.5                     | 0x100000       |
 *  | +1/2^21     | +0.000000476837158203125 | 0x000001       |
 *  |  0          |  0.0                     | 0x000000       |
 *  | -1/2^21     | -0.000000476837158203125 | 0x3FFFFF       |
 *  | -0.5        | -0.5                     | 0x300000       |
 *  | -1 + 1/2^21 | -0.999999523162841796875 | 0x200001       |
 *  | -1          | -1.0                     | 0x200000       |
 * \note    For the amplitude +1, which cannot be represented in the SQ0.21 fixed point format, the container code
 *  0x200000 is returned. This code represents the fixed point value -1.0. However, this value may be forced as-is to
 *  the unsigned integer format and in this case it may be involved in further calculations.
 */
extern sq021_t sin_sq021(const uq016_t phi);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* FIXTRIG_H */
