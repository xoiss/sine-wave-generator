/**@file
 * @brief   Definitions of integer data types.
 * @details This file provides definitions for those integer data types actually implemented in the target platform.
 *  First of all these include data types with nonstandard width. Also this file introduces shortened notation for
 *  integer data types with explicit declaration of  data type properties such as signedness and width.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef INTTYPES_H
#define INTTYPES_H

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Integer data types with standard width.
 * @details The following notation is used for integer data types:
 *  - SIm -- signed integer value having m bits in total, including 1 bit for sign followed by m-1 integer bits
 *  - UIm -- unsigned integer value having m bits in total, all of them are integer bits, no sign bit
 * @details Signed integer SIm represents values in the discrete range [-2^m; +2^m-1]. Unsigned integer UIm represents
 *  values in the discrete range [0; 2^m-1].
 * @{
 */
typedef signed char         si8_t;      /**< Integer data type, signed, 8-bit width. */
typedef unsigned char       ui8_t;      /**< Integer data type, unsigned, 8-bit width. */
typedef signed short int    si16_t;     /**< Integer data type, signed, 16-bit width. */
typedef unsigned short int  ui16_t;     /**< Integer data type, unsigned, 16-bit width. */
typedef signed long int     si32_t;     /**< Integer data type, signed, 32-bit width. */
typedef unsigned long int   ui32_t;     /**< Integer data type, unsigned, 32-bit width. */
/**@}*/

/**@name    Integer data types with nonstandard width.
 * @details Nonstandard integer data types are emulated with the host platform's standard integer data types of
 *  appropriate width and signedness. They are used as containers for nonstandard integer values.
 * @details Integer container data type has more wide binary representation than the contained (emulated) integer data
 *  type, e.g. UI32 container is used for emulation of nonstandard UI22 data type. The higher order bits of the
 *  container are left unused. In the case of signed data (and signed container) the sign bit of the contained value is
 *  propagated through all the unused bits of the container including its own sign bit - by these means the container's
 *  sign repeats the contained value sign. In the case of unsigned data (and unsigned container) all the unused bits of
 *  the container are filled with zeroes.
 * @{
 */
typedef ui8_t   bool_t;     /**< Boolean data type, unsigned, 1-bit width. */
typedef si32_t  si22_t;     /**< Integer data type, signed, 22-bit width. */
typedef ui32_t  ui22_t;     /**< Integer data type, unsigned, 22-bit width. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* INTTYPES_H */
