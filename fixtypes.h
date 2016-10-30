/**@file
 * @brief   Definitions of fixed point data types.
 * @details This file provides definitions for those fixed point data types actually implemented in the target platform.
 *  Also this file introduces shortened notation for fixed point data types with explicit declaration of data type
 *  properties such as signedness, width, number of integer and fractions bits.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef FIXTYPES_H
#define FIXTYPES_H

/*--------------------------------------------------------------------------------------------------------------------*/
#include "inttypes.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Fixed point data types.
 * @details The following notation is used for fixed point data types:
 *  - SQm.n -- signed fixed point value having 1 sign bit, m integer bits, and n fractional bits.
 *  - UQm.n -- unsigned fixed point value having no sign bit, m integer bits, and n fractional bits.
 *
 * @note    The accepted notation does not include the sign bit into the m value. Only the number of integer bits is
 *  included in m.
 * @details Signed fixed point SQm.n represents values in the discrete range [-2^m; +2^m-1/2^n] with resolution of
 *  1/2^n; negative values use two's complement format. Unsigned fixed point UQm.n represents values in the discrete
 *  range [0; 2^m-1/2^n] with resolution of 1/2^n.
 * @note    When it is necessary to represent the signed +2^m or unsigned 2^m values which are out of the domain of
 *  data types SQm.n and UQm.n, in particular cases it is allowed to use values -2^m and 0 respectively for them. The
 *  information on which value is actually implied, +2^m or -2^m, and 2^m or 0, shall be provided by external means.
 *  Note that type conversion and other fixed point data processing functions cannot treat such shuffle properly if they
 *  were not intentionally designed to support this. Also the SQ(m+1).(n-1) and UQ(m+1).(n-1) data types may be used
 *  instead if it is allowed to reduce the resolution; or SQ(m+1).n and UQ(m+1).n if it is possible to increase the
 *  width of the fixed point data type; or UQ(m+1).n may be used instead of SQm.n if negative values are treated
 *  separately.
 * @details Fixed point data types are simulated with the host platform integer data types of appropriate width and
 *  signedness. They are used as containers for fixed point values.
 * @note    When a fixed point data type is actually represented with an integer container data type of the simulator
 *  host platform which is wider than necessary (e.g., UQ0.22 which has 22 bits is represented actually with the UI32
 *  container), the higher order bits of such container are left unused. In the case of signed fixed point data the sign
 *  bit is propagated through the unused bits of the container; and in the case of unsigned fixed point data the unused
 *  bits of the container are filled with zeroes.
 * @{
 */
typedef si16_t  sq015_t;        /**< Fixed point data type, signed, no integer bits, 15 fractional bits. */
typedef ui16_t  uq016_t;        /**< Fixed point data type, unsigned, no integer bits, 16 fractional bits. */
typedef si22_t  sq021_t;        /**< Fixed point data type, signed, no integer bits, 21 fractional bits. */
typedef ui22_t  uq121_t;        /**< Fixed point data type, unsigned, 1 integer bit, 21 fractional bits. */
typedef ui22_t  uq022_t;        /**< Fixed point data type, unsigned, no integer bits, 22 fractional bits. */
/**@}*/

/**@name    Effective widths of binary representations of fixed point data types, in bits.
 * @details The effective width is the total number of bits in the binary representation of a fixed point value SQm.n or
 *  UQm.n. It equals to (s+m+n), where s stays for the width of the sign bit and equals either 1 for all signed data
 *  types or 0 for unsigned, m and n denote numbers of integer and fractional bits respectively.
 * @note    The width of the integer container used for simulation of particular fixed point data type is greater than
 *  or equal to the effective width of such fixed point data type.
 * @{
 */
#define SQ015_BIT   (1+0+15)    /**< Effective width of SQ0.15 data type. */
#define UQ016_BIT   (0+0+16)    /**< Effective width of UQ0.16 data type. */
#define SQ021_BIT   (1+0+21)    /**< Effective width of SQ0.21 data type. */
#define UQ121_BIT   (0+1+21)    /**< Effective width of UQ1.21 data type. */
#define UQ022_BIT   (0+0+22)    /**< Effective width of UQ0.22 data type. */
/**@}*/

/**@name    Number of fractional bits in binary representations of fixed point data types.
 * @details The number of fractional bits is specified with the n in the data type name SQm.n or UQm.n.
 * @{
 */
#define SQ015_FRAC  (15)        /**< Number of fractional bits in SQ0.15 data type. */
#define UQ016_FRAC  (16)        /**< Number of fractional bits in UQ0.16 data type. */
#define SQ021_FRAC  (21)        /**< Number of fractional bits in SQ0.21 data type. */
#define UQ121_FRAC  (21)        /**< Number of fractional bits in UQ1.21 data type. */
#define UQ022_FRAC  (22)        /**< Number of fractional bits in UQ0.22 data type. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Functions converting fixed point values width and/or precision preserving the signedness.
 * @param[in]   x   -- fixed point value to be converted.
 * @return  Fixed point value in the new data format.
 * @details These functions change either the number of integer m or number of fractional n bits, or both in the format
 *  of a fixed point value. The signedness of the value is preserved. If during conversion the total width (s+m+n)
 *  changes, the fixed point value may need to be repacked into a different integer container having the appropriate
 *  width. If (s+m+n) is increased so that the current container is not able to accommodate the new format, the smallest
 *  wider container is taken; and vice-versa if (s+m+n) is decreased so that a more narrow container may be used than
 *  the current one, the smallest container able to accommodate the new format is selected.
 * @details When a S/UQ(m1).(n1) value is converted to S/UQ(m2).(n2), on the first step of conversion, the integer
 *  container is extended if necessary in order not to lose the sign bit (if it exists), the integer bits (m1 or m2
 *  whichever is greater), and the final number n2 of fractional bits. Then, if n2>n1, the container value is shifted to
 *  the left (towards the higher-order bits) for n2-n1 bits and padded with n2-n1 zero bits to the right; and if n2<n1,
 *  the container value is shifted arithmetically (for signed data) or logically (for unsigned) to the right for n1-n2
 *  bits and the lower order bits are rejected. And finally, if the resulting (s+m+n) allows, the integer container is
 *  shrunk to the smallest applicable width.
 * @details If the integer container data type has more wide binary representation than the contained (simulated) fixed
 *  point data type (e.g., UI32 container is actually used for simulation of UQ0.22 data type). The higher order bits of
 *  the container are left unused. In the case of signed data (and signed container) the sign bit of the contained value
 *  is propagated through all the unused bits of the container including its own sign bit -- by these means the
 *  container's sign repeats the contained value sign. In the case of unsigned data (and unsigned container) all the
 *  unused bits of the container are filled with zeroes.
 * @{
 */
extern sq021_t sq021_from_sq015(const sq015_t x);       /**< Converts SQ0.15 value to SQ0.21 data type. */
extern uq121_t uq121_from_uq016(const uq016_t x);       /**< Converts UQ0.16 value to UQ1.21 data type. */
extern uq022_t uq022_from_uq016(const uq016_t x);       /**< Converts UQ0.16 value to UQ0.22 data type. */
extern sq015_t sq015_from_sq021(const sq021_t x);       /**< Converts SQ0.21 value to SQ0.15 data type. */
extern uq016_t uq016_from_uq022(const uq022_t x);       /**< Converts UQ0.22 value to UQ0.16 data type. */
/**@}*/

/**@name    Functions converting signed to/from unsigned fixed point values preserving width and number of integer bits.
 * @param[in]   x   -- fixed point value to be converted.
 * @return  Fixed point value in the new data format.
 * @details These functions either remove the sign bit or insert it. The total width of the data type and the number of
 *  integer bits m are preserved, while the number of fractional bits n is changed by one to compensate the change in
 *  the sign bit. The number of fractional bits n is increased by one when the sign bit is removed during conversion
 *  from a signed data type SQm.n to the corresponding unsigned UQm.(n+1); and it is decreased by one when the sign bit
 *  is inserted during conversion from an unsigned data type UQm.n to the corresponding signed SQm.(n-1). The integer
 *  container width is not changed during such conversion as soon as the total width (s+m+n) stays the same.
 * @note    Negative signed fixed point values may not be converted to unsigned.
 * @details When a non-negative SQm.n is converted to UQm.(n+1), the value of its integer container is shifted to the
 *  left (towards the higher-order bits) for one bit rejecting the original sign bit (which equals to zero) and padded
 *  with one zero bit to the right. Inversely, when a UQm.n is converted to SQm.(n-1), the value of its integer
 *  container is logically (i.e., considering the most significant bit as numeric but not the sign) shifted to the right
 *  for one bit rejecting the original least significant numeric bit and filling the new sign bit with zero which
 *  denotes the non-negative signed value.
 * @note    When an unsigned value is converted to signed of the same width the least significant numeric bit is lost
 *  decreasing the resolution by one order of binary magnitude.
 * @{
 */
extern uq016_t uq016_from_sq015(const sq015_t x);       /**< Converts SQ0.15 value to UQ0.16 data type. */
extern sq015_t sq015_from_uq016(const uq016_t x);       /**< Converts UQ0.16 value to SQ0.15 data type. */
extern uq022_t uq022_from_sq021(const sq021_t x);       /**< Converts SQ0.21 value to UQ0.22 data type. */
extern sq021_t sq021_from_uq022(const uq022_t x);       /**< Converts UQ0.22 value to SQ0.21 data type. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* FIXTYPES_H */
