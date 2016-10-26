/*--------------------------------------------------------------------------------------------------------------------*/
/**
 * \brief   Composes a single bit mask having only one given bit selected.
 * \param[in]   nr      Index (starting with 0) of the bit to be selected.
 * \return  The requested bit mask.
 * \note    The \p nr must be less than the number of bits in the native integer data type of the host simulator
 *  platform (32 bits) to avoid the undefined behavior.
 */
#define BIT(nr)         (1UL << (nr))

/**
 * \brief   Composes a multiple bit mask having the given number of contiguous lower order bits selected.
 * \param[in]   nr      Number of the lower order bits to be selected.
 * \return  The requested bit mask.
 * \note    The \p nr must be less than the number of bits in the native integer data type of the host simulator
 *  platform (32 bits) to avoid the undefined behavior.
 */
#define BIT_MASK(nr)    (BIT(nr) - 1UL)

/*--------------------------------------------------------------------------------------------------------------------*/
/**
 * \name    Single bit masks for the sign bit of a fixed point value.
 */
/**@{*/
#define SQ015_SIGN      (BIT(SQ015_BIT - 1))        /*!< Bit mask for the sign bit of the SQ0.15 data type. */
#define SQ021_SIGN      (BIT(SQ021_BIT - 1))        /*!< Bit mask for the sign bit of the SQ0.21 data type. */
/**@}*/

/**
 * \name    Multiple bit masks for the numeric bits of a fixed point value.
 */
/**@{*/
#define SQ015_MASK      (BIT_MASK(SQ015_BIT))       /*!< Bit mask for numeric bits of the SQ0.15 data type. */
#define UQ016_MASK      (BIT_MASK(UQ016_BIT))       /*!< Bit mask for numeric bits of the UQ0.16 data type. */
#define SQ021_MASK      (BIT_MASK(SQ021_BIT))       /*!< Bit mask for numeric bits of the SQ0.21 data type. */
#define UQ022_MASK      (BIT_MASK(UQ022_BIT))       /*!< Bit mask for numeric bits of the UQ0.22 data type. */
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"
#include <assert.h>

/*--------------------------------------------------------------------------------------------------------------------*/
/* Converts SQ0.15 value to SQ0.21 data type. */
sq021_t sq015_to_sq021(const sq015_t x) {
    assert(((x & SQ015_SIGN ? ~x : x) & ~SQ015_MASK) == 0);
    return((sq021_t)x << (SQ021_BIT - SQ015_BIT));
}

/* Converts UQ0.16 value to UQ0.22 data type. */
uq022_t uq016_to_uq022(const uq016_t x) {
    assert((x & ~UQ016_MASK) == 0);
    return((uq022_t)x << (UQ022_BIT - UQ016_BIT));
}

/* Converts SQ0.21 value to SQ0.15 data type. */
sq015_t sq021_to_sq015(const sq021_t x) {
    assert(((x & SQ021_SIGN ? ~x : x) & ~SQ021_MASK) == 0);
    return(x >> (SQ021_BIT - SQ015_BIT));
}

/* Converts UQ0.22 value to UQ0.16 data type. */
uq016_t uq022_to_uq016(const uq022_t x) {
    assert((x & ~UQ022_MASK) == 0);
    return(x >> (UQ022_BIT - UQ016_BIT));
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Converts SQ0.15 value to UQ0.16 data type. */
uq016_t sq015_to_uq016(const sq015_t x) {
    assert(((x & SQ015_SIGN ? ~x : x) & ~SQ015_MASK) == 0);
    assert(x >= 0);
    return(x << 1);
}

/* Converts UQ0.16 value to SQ0.15 data type. */
sq015_t uq016_to_sq015(const uq016_t x) {
    assert((x & ~UQ016_MASK) == 0);
    return(x >> 1);
}

/* Converts SQ0.21 value to UQ0.22 data type. */
uq022_t sq021_to_uq022(const sq021_t x) {
    assert(((x & SQ021_SIGN ? ~x : x) & ~SQ021_MASK) == 0);
    assert(x >= 0);
    return(x << 1);
}

/* Converts UQ0.22 value to SQ0.21 data type. */
sq021_t uq022_to_sq021(const uq022_t x) {
    assert((x & ~UQ022_MASK) == 0);
    return(x >> 1);
}

/*--------------------------------------------------------------------------------------------------------------------*/
