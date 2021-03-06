/**@file
 * @brief   Implementation of trigonometry functions on fixed point data types.
 * @details This file implements trigonometry functions used for sine wave signal generation in the field of fixed point
 *  numbers.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtrig.h"
#include "fixmath.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Returns the sine given a momentary phase, unsigned fixed point 0.16-bit version.
 * @param[in]   phi -- momentary phase.
 * @return  Momentary amplitude of the function sin(phi).
 * @details The domain of the defined function is the subset of UQ0.16 values in the discrete range [0.0; 0.25-1/2^16]
 *  with resolution of 1/2^16. This range is scaled to the floating point range [0; pi/2) with resolution of pi/2^15
 *  radian:
 *  | phase, radian  | fixed point value  | container code |
 *  |----------------|--------------------|----------------|
 *  | pi/2           | not allowed (*)    | not allowed    |
 *  | pi/2 - pi/2^15 | 0.2499847412109375 | 0x3FFF         |
 *  | pi/4           | 0.125              | 0x2000         |
 *  | pi/2^15        | 0.0000152587890625 | 0x0001         |
 *  | 0              | 0.0                | 0x0000         |
 * @note    Values of the phase outside the domain range [0; pi/2) radians shall be projected into the domain.
 * @details The codomain of the defined function is the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16]
 *  with resolution of 1/2^16. This range corresponds to the floating point range [0; 1) with resolution of 1/2^16:
 *  | amplitude  | fixed point value  | container code |
 *  |------------|--------------------|----------------|
 *  | 1          | not reachable (*)  | not reachable  |
 *  | 1 - 1/2^16 | 0.9999847412109375 | 0xFFFF         |
 *  | 0.75       | 0.75               | 0xC000         |
 *  | 0.5        | 0.5                | 0x8000         |
 *  | 1/2^16     | 0.0000152587890625 | 0x0001         |
 *  | 0          | 0.0                | 0x0000         |
 * @note    The momentary amplitude value 1 exactly cannot be represented as a UQ0.16 value. However, it is actually
 *  never reached taking into account the resolution of \p phi.
 */
static uq016_t qsin_uq016(const uq016_t phi);

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Phase-to-sine lookup table (LUT).
 * @param[in]   phi -- momentary phase.
 * @return  Momentary amplitude of the function sin(phi).
 * @details This table keeps the exact values of the function sin(phi) for the argument \p phi in the discrete range
 *  [0; pi/2) radians with regular step between knots on the phi axis. The table has only one column for the value of
 *  sin(phi) and 256 records each corresponding to one of the allowed \p phi values.
 * @details The domain of this table is the set of unsigned 8-bit integer keys - i.e., the discrete range [0; 255]. This
 *  range corresponds to the floating point range [0; pi/2) with resolution of pi/512 radian:
 *  | phi, radian   | fixed point value | container code | 8-bit integer key |
 *  |---------------|-------------------|----------------|-------------------|
 *  | pi/2          | not allowed       | not allowed    | not allowed       |
 *  | pi/2 - pi/512 | 0.2490234375      | 0x3FC0         | 255               |
 *  | pi/4          | 0.125             | 0x2000         | 128               |
 *  | pi/512        | 0.0009765625      | 0x0040         | 1                 |
 *  | 0             | 0.0               | 0x0000         | 0                 |
 * @details To obtain the unsigned 8-bit integer key which may be used as the index into the table given the fixed point
 *  \p phi value, the following formula shall be used:
 *  - key = phi.code / 64, where:
 *  - phi.code is the unsigned integer 16-bit container code of the fixed point value of \p phi. Note that as soon as
 *      \p phi belongs to the range [0.0; 0.25), which corresponds to [0; pi/2) radians, the phi.code belongs to the
 *      range [0x0000; 0x3FFF] and consequently key will belong to the range [0; 255].
 *  - 64 is the number of distinguishable phi values which may be represented as different fixed point values between
 *      any two adjacent entries into the table.
 * @details For the case when the given phase belongs to the whole range [0; 2*pi) radians and does not necessarily
 *  belong to the domain [0; pi/2), the following conversion shall be taken:
 *  | phase, radian  | phi, radian   | fixed point expression | postprocessing               |
 *  |----------------|---------------|------------------------|------------------------------|
 *  |  2*pi          | not reachable | not reachable          |                              |
 *  | (3*pi/2; 2*pi) | 2*pi - phase  | -1.0 - phase           | inverse the sign of sin(phi) |
 *  |  3*pi/2        | not allowed   | not allowed            | sin(3*pi/2) = -1             |
 *  | [pi; 3*pi/2)   | phase - pi    | phase - 0.5            | inverse the sign of sin(phi) |
 *  | (pi/2; pi)     | pi - phase    | 0.5 - phase            |                              |
 *  |  pi/2          | not allowed   | not allowed            | sin(pi/2) = +1               |
 *  | [0; pi/2)      | phase         | phase                  |                              |
 * @details The codomain of this table is the set of unsigned fixed point 0.16-bit values - i.e., the discrete range
 *  [0.0; 1.0-1/2^16] with resolution of 1/2^16. This range corresponds to the floating point range [0; 1) with
 *  resolution of 1/2^16:
 *  | amplitude  | fixed point value  | container code |
 *  |------------|--------------------|----------------|
 *  | 1          | not reachable (*)  | not reachable  |
 *  | 1 - 1/2^16 | 0.9999847412109375 | 0xFFFF         |
 *  | 0.75       | 0.75               | 0xC000         |
 *  | 0.5        | 0.5                | 0x8000         |
 *  | 1/2^16     | 0.0000152587890625 | 0x0001         |
 *  | 0          | 0.0                | 0x0000         |
 * @note    The momentary amplitude value 1 exactly cannot be represented as a UQ0.16 value. However, it is actually
 *  never reached taking into account the resolution of \p phi.
 */
static const uq016_t qsin_lut[] = {
    0x0000, 0x0192, 0x0324, 0x04B6, 0x0648, 0x07DA, 0x096C, 0x0AFE,
    0x0C90, 0x0E21, 0x0FB3, 0x1144, 0x12D5, 0x1466, 0x15F7, 0x1787,
    0x1918, 0x1AA8, 0x1C38, 0x1DC7, 0x1F56, 0x20E5, 0x2274, 0x2402,
    0x2590, 0x271E, 0x28AB, 0x2A38, 0x2BC4, 0x2D50, 0x2EDC, 0x3067,
    0x31F1, 0x337C, 0x3505, 0x368E, 0x3817, 0x399F, 0x3B27, 0x3CAE,
    0x3E34, 0x3FBA, 0x413F, 0x42C3, 0x4447, 0x45CB, 0x474D, 0x48CF,
    0x4A50, 0x4BD1, 0x4D50, 0x4ECF, 0x504D, 0x51CB, 0x5348, 0x54C3,
    0x563E, 0x57B9, 0x5932, 0x5AAA, 0x5C22, 0x5D99, 0x5F0F, 0x6084,
    0x61F8, 0x636B, 0x64DD, 0x664E, 0x67BE, 0x692D, 0x6A9B, 0x6C08,
    0x6D74, 0x6EDF, 0x7049, 0x71B2, 0x731A, 0x7480, 0x75E6, 0x774A,
    0x78AD, 0x7A10, 0x7B70, 0x7CD0, 0x7E2F, 0x7F8C, 0x80E8, 0x8243,
    0x839C, 0x84F5, 0x864C, 0x87A1, 0x88F6, 0x8A49, 0x8B9A, 0x8CEB,
    0x8E3A, 0x8F88, 0x90D4, 0x921F, 0x9368, 0x94B0, 0x95F7, 0x973C,
    0x9880, 0x99C2, 0x9B03, 0x9C42, 0x9D80, 0x9EBC, 0x9FF7, 0xA130,
    0xA268, 0xA39E, 0xA4D2, 0xA605, 0xA736, 0xA866, 0xA994, 0xAAC1,
    0xABEB, 0xAD14, 0xAE3C, 0xAF62, 0xB086, 0xB1A8, 0xB2C9, 0xB3E8,
    0xB505, 0xB620, 0xB73A, 0xB852, 0xB968, 0xBA7D, 0xBB8F, 0xBCA0,
    0xBDAF, 0xBEBC, 0xBFC7, 0xC0D1, 0xC1D8, 0xC2DE, 0xC3E2, 0xC4E4,
    0xC5E4, 0xC6E2, 0xC7DE, 0xC8D9, 0xC9D1, 0xCAC7, 0xCBBC, 0xCCAE,
    0xCD9F, 0xCE8E, 0xCF7A, 0xD065, 0xD14D, 0xD234, 0xD318, 0xD3FB,
    0xD4DB, 0xD5BA, 0xD696, 0xD770, 0xD848, 0xD91E, 0xD9F2, 0xDAC4,
    0xDB94, 0xDC62, 0xDD2D, 0xDDF7, 0xDEBE, 0xDF83, 0xE046, 0xE107,
    0xE1C6, 0xE282, 0xE33C, 0xE3F4, 0xE4AA, 0xE55E, 0xE610, 0xE6BF,
    0xE76C, 0xE817, 0xE8BF, 0xE966, 0xEA0A, 0xEAAB, 0xEB4B, 0xEBE8,
    0xEC83, 0xED1C, 0xEDB3, 0xEE47, 0xEED9, 0xEF68, 0xEFF5, 0xF080,
    0xF109, 0xF18F, 0xF213, 0xF295, 0xF314, 0xF391, 0xF40C, 0xF484,
    0xF4FA, 0xF56E, 0xF5DF, 0xF64E, 0xF6BA, 0xF724, 0xF78C, 0xF7F1,
    0xF854, 0xF8B4, 0xF913, 0xF96E, 0xF9C8, 0xFA1F, 0xFA73, 0xFAC5,
    0xFB15, 0xFB62, 0xFBAD, 0xFBF5, 0xFC3B, 0xFC7F, 0xFCC0, 0xFCFE,
    0xFD3B, 0xFD74, 0xFDAC, 0xFDE1, 0xFE13, 0xFE43, 0xFE71, 0xFE9C,
    0xFEC4, 0xFEEB, 0xFF0E, 0xFF30, 0xFF4E, 0xFF6B, 0xFF85, 0xFF9C,
    0xFFB1, 0xFFC4, 0xFFD4, 0xFFE1, 0xFFEC, 0xFFF5, 0xFFFB, 0xFFFF,
};

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the sine given a momentary phase, signed fixed point 0.15-bit version. */
uq016_t qsin_uq016(const uq016_t phi) {

    /**@cond false*/
    #define _PHI_RANK   (POW2(UQ016_BIT) / 4)           /* Number of different phi values in the first quadrant. */
    #define _KEY_RANK   (ARRAY_SIZE(qsin_lut))          /* Number of entries in the phase-to-sine LUT. */
    #define _COEF_RANK  (_PHI_RANK / _KEY_RANK)         /* Number of different phi values between LUT entries. */
    #define _COEF_BIT   (LOG2(_COEF_RANK))              /* Width of the linear interpolation coefficient. */
    #define _COEF_MASK  (BIT_MASK(_COEF_BIT))           /* Bit mask for linear interpolation coefficient. */
    #define _1          (0x0000u)                       /* Container value for UQ0.16 value 1.0
                                                         * represented as 0.0 modulo 1.0. */
    /**@endcond*/

    ui8_t   key0;       /* Left side key into the phase-to-sine LUT. */
    uq016_t coef;       /* Linear interpolation coefficient. */

    static_assert(POW2(sizeof(key0) * 8) == _KEY_RANK);

    key0 = phi >> _COEF_BIT;
    coef = (phi & _COEF_MASK) << (UQ016_BIT - _COEF_BIT);

    if (coef == 0) {
        return qsin_lut[key0];

    } else {
        ui8_t   key1;               /* Right side key into the phase-to-sine LUT. */
        uq016_t val0, val1;         /* Left and right side values taken from the LUT with linear weight. */
        key1 = key0 + 1;
        val1 = key1 == 0 ? coef : qmul_uq016(qsin_lut[key1], coef);
        val0 = qmul_uq016(qsin_lut[key0], _1 - coef);
        return val0 + val1;
    }

    #undef  _PHI_RANK
    #undef  _KEY_RANK
    #undef  _COEF_RANK
    #undef  _COEF_BIT
    #undef  _COEF_MASK
    #undef  _1
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the modulated sine given a momentary phase and momentary attenuation factor, signed fixed point 0.15-bit
 * version. */
sq015_t msin_sq015(const uq016_t phi, const uq016_t att) {

    /**@cond false*/
    #define _PI2    (0x4000u)       /* Container value for UQ0.16 value 0.25 which stays for pi/2 radian. */
    #define _PI     (0x8000u)       /* Container value for UQ0.16 value 0.5 which stays for pi radian. */
    #define _3PI2   (0xC000u)       /* Container value for UQ0.16 value 0.75 which stays for 3*pi/2 radian. */
    #define _1P     (0x7FFF)        /* Container value for SQ0.15 value +1.0-1/2^15. */
    #define _1N     (0x8000)        /* Container value for SQ0.15 value -1.0. */
    #define _1      (0x0000u)       /* Container value for UQ0.16 value 1.0 represented as 0.0 modulo 1.0. */
    /**@endcond*/

    if (phi == _PI2) {
        return att == 0 ? _1P : +sq015_from_uq016(_1 - att);

    } else if (phi == _3PI2) {
        return att == 0 ? _1N : -sq015_from_uq016(_1 - att);

    } else {
        uq016_t phi1 = phi;     /* Value of phi brought into the first quadrant - i.e., the range [0; pi/2) radian. */
        bool_t  neg = 0;        /* Equals to 1 if sin(phi) < 0; equals to 0 if sin(phi) >= 0. */
        uq016_t usin;           /* Unsigned 0.16-bit absolute value of sin(phi). */
        bool_t  lsb;            /* Value of LSB of 0.16-bit value returned by sin(phi) before rounding to 0.15-bit. */
        sq015_t ssin;           /* Signed 0.15-bit absolute value of sin(phi). */

        if (phi >= _PI) {
            phi1 -= _PI;
            neg = 1;
        }
        if (phi1 > _PI2) {
            phi1 = _PI - phi1;
        }

        usin = qsin_uq016(phi1);

        if (att > 0) {
            usin = qmul_uq016(usin, _1 - att);
        }

        lsb = usin & 1;
        ssin = sq015_from_uq016(usin);
        if (lsb && ssin < 0x7FFF) {
            ++ssin;
        }

        return neg ? -ssin : +ssin;
    }

    #undef  _PI2
    #undef  _PI
    #undef  _3PI2
    #undef  _1P
    #undef  _1N
    #undef  _1
}

/*--------------------------------------------------------------------------------------------------------------------*/
