#ifndef FIXTYPES_H
#define FIXTYPES_H

/*--------------------------------------------------------------------------------------------------------------------*/
/**\name    Widths of binary representations of fixed point data types, in bits.
 * \details The following notation is used for fixed point data types:
 *  - SQm.n - signed fixed point value having 1 sign bit, m integer bits, and n fractional bits
 *  - UQm.n - unsigned fixed point value having no sign bit, m integer bits, and n fractional bits
 *
 * \note    The accepted notation does not include the sign bit into the m value. Only the number of integer bits is
 *  included.
 * \details The total number of bits in a fixed point value binary representation, the width, equals to (1+m+n) for
 *  signed and (0+m+n) for unsigned data types, where 1 stays for the sign bit in the case of signed data type, 0 means
 *  absence of the sign bit in the case of unsigned data type, m and n denote numbers of integer and fractional bits
 *  respectively.
 */
/**@{*/
#define SQ015_BIT   (1+0+15)    /*!< Width of SQ0.15 data type. */
#define UQ016_BIT   (0+0+16)    /*!< Width of UQ0.16 data type. */
#define SQ021_BIT   (1+0+21)    /*!< Width of SQ0.21 data type. */
#define UQ022_BIT   (0+0+22)    /*!< Width of UQ0.22 data type. */
/**@}*/

#include <stdint.h>

/**\name    Fixed point data types.
 * \details Signed fixed point SQm.n represents values in the discrete range [-2^m; +2^m-1/2^n] with resolution of
 *  1/2^n. Unsigned fixed point UQm.n represents values in the discrete range [0; +2^m-1/2^n] with resolution of 1/2^n.
 * \details Fixed point data types are emulated with the host platform integer data types of appropriate width and
 *  signedness. They are used as containers for fixed point values.
 * \note    When a fixed point data type is actually represented with an integer container data type of the emulator
 *  host platform which is wider than necessary (e.g., UQ0.22 which has 22 bits is represented with uint32 container),
 *  the higher order bits of such container are left unused. In the case of signed fixed point data the sign bit is
 *  propagated through the unused bits of the container; and in the case of unsigned fixed point data the unused bits of
 *  the container are filled with zeroes.
 */
/**@{*/
typedef int16_t     sq015_t;    /*!< SQ0.15 - signed fixed point data, no integer bits, 15 fractional bits. */
typedef uint16_t    uq016_t;    /*!< UQ0.16 - unsigned fixed point data, no integer bits, 16 fractional bits. */
typedef int32_t     sq021_t;    /*!< SQ0.21 - signed fixed point data, no integer bits, 21 fractional bits. */
typedef uint32_t    uq022_t;    /*!< UQ0.22 - unsigned fixed point data, no integer bits, 22 fractional bits. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**\name    Functions converting fixed point values between data types of different widths preserving the signedness.
 * \param[in]   x   Fixed point value to be converted.
 * \return  Fixed point value in the new data format.
 * \details When a fixed point value, signed or unsigned, of width W0 is extended to a wider width W1 preserving the
 *  signedness, it is shifted to the left (towards the higher-order bits of the container) for W1-W0 bits and padded
 *  with zeroes on the right. Inversely, when it is shrunk to a more narrow width W2, it is arithmetically (i.e.,
 *  preserving the sign) or logically (i.e., considering the highest, most significant, bit as numeric) shifted to the
 *  right for W0-W2 bits and the corresponding number of the lower order bits are simply rejected; selection between
 *  arithmetic and logic shift is done according to the signedness of the data type. If the resulting fixed point data
 *  type width is less than the width of its integer container, the unused higher order bits of the container propagate
 *  the sign of the fixed point value or filled with zeroes if the value is unsigned.
 */
/**@{*/
extern sq021_t sq021_from_sq015(const sq015_t x);       /*!< Converts SQ0.15 value to SQ0.21 data type. */
extern uq022_t uq022_from_uq016(const uq016_t x);       /*!< Converts UQ0.16 value to UQ0.22 data type. */
extern sq015_t sq015_from_sq021(const sq021_t x);       /*!< Converts SQ0.21 value to SQ0.15 data type. */
extern uq016_t uq016_from_uq022(const uq022_t x);       /*!< Converts UQ0.22 value to UQ0.16 data type. */
/**@}*/

/**\name    Functions converting signed and unsigned fixed point values to each other preserving the width.
 * \param[in]   x   Fixed point value to be converted.
 * \return  Fixed point value in the new data format.
 * \details These functions either remove the sign bit, when converting a signed value to corresponding unsigned data
 *  type, or insert it, when converting in opposite direction. The width of the data type is preserved. Namely the total
 *  width (i.e., including the sign bit) is preserved, while the number of numeric (integer and fractional) bits is
 *  changed by one: increased by one when converting signed to unsigned, or decreased by one when converting in opposite
 *  direction. The integer container width is not changed during the conversion as soon as the total width is preserved.
 * \details When a signed fixed point nonnegative value is converted to unsigned, it is shifted to the left (towards the
 *  higher-order bits of the container) for one bit rejecting the original sign bit (which equals to zero) and padded
 *  with one zero bit on the right. Inversely, when an unsigned value is converted to signed, it is logically (i.e.,
 *  considering the most significant bit as numeric) shifted to the right for one bit rejecting the original least
 *  significant numeric bit and filling the new sign bit with zero which denotes the nonnegative signed value.
 * \note    Negative fixed point values may not be converted to unsigned.
 * \note    When an unsigned value is converted to signed of the same width the least significant numeric bit is lost
 *  decreasing the resolution by one order of binary magnitude.
 */
/**@{*/
extern uq016_t uq016_from_sq015(const sq015_t x);       /*!< Converts SQ0.15 value to UQ0.16 data type. */
extern sq015_t sq015_from_uq016(const uq016_t x);       /*!< Converts UQ0.16 value to SQ0.15 data type. */
extern uq022_t uq022_from_sq021(const sq021_t x);       /*!< Converts SQ0.21 value to UQ0.22 data type. */
extern sq021_t sq021_from_uq022(const uq022_t x);       /*!< Converts UQ0.22 value to SQ0.21 data type. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* FIXTYPES_H */
