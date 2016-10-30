/**@file
 * @brief   Interface to trigonometry functions on fixed point data types.
 * @details This file provides declarations of trigonometry functions used for sine wave signal generation in the field
 *  of fixed point numbers.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef FIXTRIG_H
#define FIXTRIG_H

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Returns the modulated sine given a momentary phase and momentary attenuation factor, signed fixed point
 *  0.15-bit version.
 * @param[in]   phi -- momentary phase.
 * @param[in]   att -- momentary attenuation factor.
 * @return  Momentary amplitude of the function sin(phi)*(1-att).
 * @details The amplitude modulation scheme is normally given with the formula u(t) = sin(phi(t))*(1+M*F(t)), where
 *  phi(t) is the momentary phase of the unmodulated carrier, M is the modulation coefficient, and F(t) is the
 *  modulating signal of an arbitrary form. Here M is assumed to belong to the range [0; 1] and F(t) to the range
 *  [-1; +1] which covers the modulation depth in the range from 0% to 100% and excludes the overmodulation. In this
 *  formula u(t) will have the codomain in the range [-2; +2].
 * @details The conventional formula may be normalized to the range [-1; +1] by dividing the both sides of the equation
 *  by 2 which gives u(t)/max|u| = sin(phi(t))*(1+M*F(t))/2. The factor (1+M*F(t))/2 may be denoted as (1-att), if att
 *  is defined as (1-M*F(t))/2.
 * @note    The \p att parameter, the attenuation factor, must not be mixed up with the modulation coefficient
 *  (modulation depth) which was denoted as M. The \p att stays for the value of (1-M*F(t))/2, where M is the modulation
 *  coefficient, and F(t) is the modulating signal. Also the \p att may be used simply as a constant linear attenuation
 *  factor.
 * @details The domain of the defined function sin(phi)*(1-att) is the superposition of all allowed momentary phases
 *  \p phi and all allowed momentary attenuation factors \p att.
 * @details The allowed values of \p phi are the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16] with
 *  resolution of 1/2^16. This range is scaled to the floating point range [0; 2*pi) with resolution of pi/2^15 radian:
 *  | phase, radian  | fixed point value  | container code |
 *  |----------------|--------------------|----------------|
 *  | 2*pi           | not allowed (*)    | not allowed    |
 *  | 2*pi - pi/2^15 | 0.9999847412109375 | 0xFFFF         |
 *  | 3*pi/2         | 0.75               | 0xC000         |
 *  | pi             | 0.5                | 0x8000         |
 *  | pi/2           | 0.25               | 0x4000         |
 *  | pi/2^15        | 0.0000152587890625 | 0x0001         |
 *  | 0              | 0.0                | 0x0000         |
 * @note    Values of the phase outside the domain range [0; 2*pi) radians shall be projected into the domain with the
 *  formula phi = phase-2*pi*k, where k is an integer.
 * @details The allowed values of \p att are the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16] with
 *  resolution of 1/2^16. This range is scaled to the floating point range [0; 1) with resolution of pi/2^16:
 *  | attenuation | fixed point value  | container code |
 *  |-------------|--------------------|----------------|
 *  | 1           | not allowed (*)    | not allowed    |
 *  | 1 - 1/2^16  | 0.9999847412109375 | 0xFFFF         |
 *  | 0.75        | 0.75               | 0xC000         |
 *  | 0.5         | 0.5                | 0x8000         |
 *  | 1/2^16      | 0.0000152587890625 | 0x0001         |
 *  | 0           | 0.0                | 0x0000         |
 * @note    The attenuation factor value 1 exactly, which cannot be represented as a UQ0.16 value, shall be substituted
 *  with 1-1/2^16. Indeed, as soon as the returned value of the function is of the type SQ0.15, which has smaller
 *  resolution 1/2^15 than the \p att, the output signal will be constantly 0 when \p att is specified as 1-1/2^16 as
 *  well as it would be specified as 1 exactly.
 * @details The codomain of the defined function sin(phi)*(1-att) is the set of SQ0.15 values in the discrete range
 *  [-1.0; +1.0-1/2^15] with resolution of 1/2^15. This range corresponds to the floating point range [-1; +1) with
 *  resolution of 1/2^15:
 *  | amplitude   | fixed point value  | container code |
 *  |-------------|--------------------|----------------|
 *  | +1          | not reachable (*)  | not reachable  |
 *  | +1 - 1/2^15 | +0.999969482421875 | 0x7FFF         |
 *  | +0.75       | +0.75              | 0x6000         |
 *  | +0.5        | +0.5               | 0x4000         |
 *  | +1/2^15     | +0.000030517578125 | 0x0001         |
 *  |  0          |  0.0               | 0x0000         |
 *  | -1/2^15     | -0.000030517578125 | 0x7FFF         |
 *  | -0.5        | -0.5               | 0xC000         |
 *  | -1 + 1/2^15 | -0.999969482421875 | 0x8001         |
 *  | -1          | -1.0               | 0x8000         |
 * @note    The momentary amplitude value +1 exactly, which cannot be represented as a SQ0.15 value, is substituted with
 *  1-1/2^15.
 */
extern sq015_t msin_sq015(const uq016_t phi, const uq016_t att);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* FIXTRIG_H */
