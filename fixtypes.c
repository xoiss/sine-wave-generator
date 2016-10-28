/*--------------------------------------------------------------------------------------------------------------------*/
/**\brief   Composes a single bit mask having only one given bit selected.
 * \param[in]   nr      Index (starting with 0) of the bit to be selected.
 * \return  The requested bit mask.
 * \note    The \p nr must be less than the number of bits in the native integer data type of the host simulator
 *  platform (32 bits) to avoid the undefined behavior.
 */
#define BIT(nr)         (1UL << (nr))

/**\brief   Composes a multiple bit mask having the given number of contiguous lower order bits selected.
 * \param[in]   nr      Number of the lower order bits to be selected.
 * \return  The requested bit mask.
 * \note    The \p nr must be less than the number of bits in the native integer data type of the host simulator
 *  platform (32 bits) to avoid the undefined behavior.
 */
#define BIT_MASK(nr)    (BIT(nr) - 1UL)

/*--------------------------------------------------------------------------------------------------------------------*/
/**\name    Single bit masks for the sign bit of a fixed point value.
 * \details The sign bit exists only in signed fixed point data formats. It occupies the highest order bit of the binary
 *  representation of a fixed point data type. Unsigned fixed point data formats have no dedicated bit for the sign.
 * \note    If a fixed point data type width is less than the width of its integer container used for simulation of the
 *  fixed point data type, the sign bit of the contained fixed point data type does not coincide with the sign bit of
 *  the container data type. However, if the container value is properly constructed (i.e., if all the unused higher
 *  order bits of the container propagate the value of the sign bit of the contained fixed point value) the integer sign
 *  of the container always repeats the sign of the contained fixed point value.
 */
/**@{*/
#define SQ015_SIGN      (BIT(SQ015_BIT - 1))        /*!< Bit mask for the sign bit of the SQ0.15 data type. */
#define SQ021_SIGN      (BIT(SQ021_BIT - 1))        /*!< Bit mask for the sign bit of the SQ0.21 data type. */
/**@}*/

/**\name    Multiple bit masks for the effective bits of a fixed point value.
 * \details If a fixed point data type width equals to the integer container data type width, all bits of the container
 *  integer data type are considered effective. Inversely, if the integer container data type is wider than the
 *  contained fixed point data type, there are unused higher order bits in the container; in this case the remaining
 *  lower order bits (i.e., those actually used for representing the contained fixed point data type) are considered
 *  effective - they are only those included into the mask.
 */
/**@{*/
#define SQ015_MASK      (BIT_MASK(SQ015_BIT))       /*!< Bit mask for effective bits of the SQ0.15 data type. */
#define UQ016_MASK      (BIT_MASK(UQ016_BIT))       /*!< Bit mask for effective bits of the UQ0.16 data type. */
#define SQ021_MASK      (BIT_MASK(SQ021_BIT))       /*!< Bit mask for effective bits of the SQ0.21 data type. */
#define UQ121_MASK      (BIT_MASK(UQ121_BIT))       /*!< Bit mask for effective bits of the UQ1.21 data type. */
#define UQ022_MASK      (BIT_MASK(UQ022_BIT))       /*!< Bit mask for effective bits of the UQ0.22 data type. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"
#include <assert.h>

/*--------------------------------------------------------------------------------------------------------------------*/
/* Converts SQ0.15 value to SQ0.21 data type. */
sq021_t sq021_from_sq015(const sq015_t x) {
    assert(((x & SQ015_SIGN ? ~x : x) & ~SQ015_MASK) == 0);
    return((sq021_t)x << (SQ021_FRAC - SQ015_FRAC));
}

/* Converts UQ0.16 value to UQ1.21 data type. */
uq121_t uq121_from_uq016(const uq016_t x) {
    assert((x & ~UQ016_MASK) == 0);
    return((uq121_t)x << (UQ121_FRAC - UQ016_FRAC));
}

/* Converts UQ0.16 value to UQ0.22 data type. */
uq022_t uq022_from_uq016(const uq016_t x) {
    assert((x & ~UQ016_MASK) == 0);
    return((uq022_t)x << (UQ022_FRAC - UQ016_FRAC));
}

/* Converts SQ0.21 value to SQ0.15 data type. */
sq015_t sq015_from_sq021(const sq021_t x) {
    assert(((x & SQ021_SIGN ? ~x : x) & ~SQ021_MASK) == 0);
    return(x >> (SQ021_FRAC - SQ015_FRAC));
}

/* Converts UQ0.22 value to UQ0.16 data type. */
uq016_t uq016_from_uq022(const uq022_t x) {
    assert((x & ~UQ022_MASK) == 0);
    return(x >> (UQ022_FRAC - UQ016_FRAC));
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Converts SQ0.15 value to UQ0.16 data type. */
uq016_t uq016_from_sq015(const sq015_t x) {
    assert(((x & SQ015_SIGN ? ~x : x) & ~SQ015_MASK) == 0);
    assert(x >= 0);
    return(x << 1);
}

/* Converts UQ0.16 value to SQ0.15 data type. */
sq015_t sq015_from_uq016(const uq016_t x) {
    assert((x & ~UQ016_MASK) == 0);
    return(x >> 1);
}

/* Converts SQ0.21 value to UQ0.22 data type. */
uq022_t uq022_from_sq021(const sq021_t x) {
    assert(((x & SQ021_SIGN ? ~x : x) & ~SQ021_MASK) == 0);
    assert(x >= 0);
    return(x << 1);
}

/* Converts UQ0.22 value to SQ0.21 data type. */
sq021_t sq021_from_uq022(const uq022_t x) {
    assert((x & ~UQ022_MASK) == 0);
    return(x >> 1);
}

/*--------------------------------------------------------------------------------------------------------------------*/
