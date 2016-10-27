/*--------------------------------------------------------------------------------------------------------------------*/
/**
 * \brief   Returns the number of elements in the given array.
 * \param[in]   x   Identifier of the array variable.
 * \return  Number of elements.
 * \note    This macro cannot be used for arrays passed to a function through its parameters list as soon as such
 *  parameters are considered as pointers by the compiler (namely pointers, but not arrays), and the information about 
 *  the number of elements is ignored by the compiler even it is explicitly specified in a function declaration.
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtrig.h"

/**
 * \brief   Phase-to-sine lookup table (LUT).
 * \details This table keeps the exact values of the function sin(phi) for the argument phi in the discrete range
 *  [+pi/2; +pi) radians with regular step between knots on the phi axis. The lower boundary is included into, and the
 *  upper boundary is excluded from the range.
 * \note    The range [+pi/2; +pi) was accepted instead of, for example, [0; +pi/2) or (0; +pi/2] because it allows to
 *  keep the value of sin(+pi/2) = 1, which may have nontrivial integer code, as the first element of the array (the
 *  element with index 0) and omit storing the sin(0) = 0, which has trivial integer code 0.
 * \details The table has only one column for the value of sin(phi) and a fixed but configurable number of records each
 *  corresponding to one of the allowed phi values. By these means the table provides values of sin(phi) for phi in the
 *  discrete range from +pi/2 to (+pi - pi/(2*#LUT)) with the regular step of pi/(2*#LUT) radians, where #LUT is the
 *  number of records in the table.
 * \details Records in the table are sorted in the ascending order of the phi. The first record (the one with the
 *  index 0) corresponds to phi = +pi/2, and the last record (the one with the index #LUT-1) corresponds to
 *  phi = (+pi/2 + (#LUT-1) * pi/(2*#LUT)) = (+pi - pi/(2*#LUT)) radians. Records are accessible randomly with the
 *  integer index i in the range from 0 to #LUT-1. The i-th record (starting with 0) provides the value of sin(phi) for
 *  phi = +pi/2 + i * pi/(2*#LUT) radians.
 * \details For the case when the given phase belongs to the whole range [-pi; +pi) and does not necessarily belong to
 *  the range [+pi/2; +pi) radians, the following conversion shall be taken:\n
 *  - phi := 0, if phase == -pi or phase == 0 (this value is not stored in the table)
 *  - phi := phase, if phase in [+pi/2; +pi)
 *  - phi := pi - phase, if phase in (0; +pi/2]
 *  - phi := -phase, if phase in [-pi/2; 0)
 *  - phi := pi + phase, if phase in (-pi; -pi/2]
 *
 * \note    The total number of records in the table must be a whole power of two and specified in the module settings.
 *  It provides the lb(#LUT) to be an integer value, where lb() is the binary logarithm.
 * \details The momentary phase is encoded with a Pw-bits integer value having Pw-1 numeric bits and one sign bit. If
 *  Pw-1 == lb(#LUT), then the integer code of phi obtained from the integer code of phase and subtracted the integer
 *  code of +pi/2 may be used directly as the index into the table. If Pw-1 > lb(#LUT), then the lower order bits of the
 *  integer code of (phi - pi/2) shall be dropped. The case Pw-1 < lb(#LUT) does not take place as soon as the following
 *  inequality holds Pw-1 >= lb(#LUT).
 * \details The stored value of sin(phi) is encoded as 22-bit unsigned integer value. Value 0 is encoded as 0; value 1
 *  is encoded as 2^22-1; values y in the range (0; 1) are encoded as y * (2^22-1) rounded to the nearest integer.
 */
static const uq016_t sin_lut[] = {
    0xFFFF, 0xFFFE, 0xFFFA, 0xFFF4, 0xFFEB, 0xFFE0, 0xFFD3, 0xFFC3,
    0xFFB0, 0xFF9B, 0xFF84, 0xFF6A, 0xFF4D, 0xFF2F, 0xFF0D, 0xFEEA,
    0xFEC3, 0xFE9B, 0xFE70, 0xFE42, 0xFE12, 0xFDE0, 0xFDAB, 0xFD73,
    0xFD3A, 0xFCFD, 0xFCBF, 0xFC7E, 0xFC3A, 0xFBF4, 0xFBAC, 0xFB61,
    0xFB14, 0xFAC4, 0xFA72, 0xFA1E, 0xF9C7, 0xF96D, 0xF912, 0xF8B4,
    0xF853, 0xF7F0, 0xF78B, 0xF723, 0xF6B9, 0xF64D, 0xF5DE, 0xF56D,
    0xF4F9, 0xF483, 0xF40B, 0xF390, 0xF313, 0xF294, 0xF212, 0xF18E,
    0xF108, 0xF07F, 0xEFF5, 0xEF67, 0xEED8, 0xEE46, 0xEDB2, 0xED1B,
    0xEC82, 0xEBE7, 0xEB4A, 0xEAAB, 0xEA09, 0xE965, 0xE8BE, 0xE816,
    0xE76B, 0xE6BE, 0xE60F, 0xE55D, 0xE4A9, 0xE3F4, 0xE33B, 0xE281,
    0xE1C5, 0xE106, 0xE045, 0xDF82, 0xDEBD, 0xDDF6, 0xDD2C, 0xDC61,
    0xDB93, 0xDAC3, 0xD9F2, 0xD91E, 0xD847, 0xD76F, 0xD695, 0xD5B9,
    0xD4DA, 0xD3FA, 0xD317, 0xD233, 0xD14C, 0xD064, 0xCF79, 0xCE8D,
    0xCD9E, 0xCCAE, 0xCBBB, 0xCAC7, 0xC9D0, 0xC8D8, 0xC7DE, 0xC6E1,
    0xC5E3, 0xC4E3, 0xC3E1, 0xC2DD, 0xC1D8, 0xC0D0, 0xBFC7, 0xBEBB,
    0xBDAE, 0xBC9F, 0xBB8E, 0xBA7C, 0xB968, 0xB851, 0xB739, 0xB620,
    0xB504, 0xB3E7, 0xB2C8, 0xB1A7, 0xB085, 0xAF61, 0xAE3B, 0xAD14,
    0xABEB, 0xAAC0, 0xA993, 0xA865, 0xA736, 0xA604, 0xA4D2, 0xA39D,
    0xA267, 0xA12F, 0x9FF6, 0x9EBB, 0x9D7F, 0x9C41, 0x9B02, 0x99C1,
    0x987F, 0x973B, 0x95F6, 0x94B0, 0x9368, 0x921E, 0x90D3, 0x8F87,
    0x8E39, 0x8CEA, 0x8B9A, 0x8A48, 0x88F5, 0x87A1, 0x864B, 0x84F4,
    0x839C, 0x8242, 0x80E7, 0x7F8B, 0x7E2E, 0x7CD0, 0x7B70, 0x7A0F,
    0x78AD, 0x774A, 0x75E5, 0x7480, 0x7319, 0x71B2, 0x7049, 0x6EDF,
    0x6D74, 0x6C08, 0x6A9B, 0x692D, 0x67BD, 0x664D, 0x64DC, 0x636A,
    0x61F7, 0x6083, 0x5F0E, 0x5D98, 0x5C22, 0x5AAA, 0x5932, 0x57B8,
    0x563E, 0x54C3, 0x5347, 0x51CB, 0x504D, 0x4ECF, 0x4D50, 0x4BD0,
    0x4A50, 0x48CF, 0x474D, 0x45CA, 0x4447, 0x42C3, 0x413F, 0x3FB9,
    0x3E34, 0x3CAD, 0x3B26, 0x399F, 0x3817, 0x368E, 0x3505, 0x337B,
    0x31F1, 0x3067, 0x2EDC, 0x2D50, 0x2BC4, 0x2A38, 0x28AB, 0x271E,
    0x2590, 0x2402, 0x2274, 0x20E5, 0x1F56, 0x1DC7, 0x1C37, 0x1AA8,
    0x1918, 0x1787, 0x15F7, 0x1466, 0x12D5, 0x1144, 0x0FB3, 0x0E21,
    0x0C90, 0x0AFE, 0x096C, 0x07DA, 0x0648, 0x04B6, 0x0324, 0x0192,
};

/*--------------------------------------------------------------------------------------------------------------------*/
#include <assert.h>

/* Returns the sine given a momentary phase, signed fixed point 0.21-bit version. */
sq021_t sin_sq021(const uq016_t phi) {
    return sq021_from_uq022(uq022_from_uq016(phi));     /* skeleton */
}

/*--------------------------------------------------------------------------------------------------------------------*/
