#include "sinegen.h"

/**
 * \brief   Returns wave momentary phase increment (dPhi) per single sample depending on the generator frequency (Fo).
 * \param[in]   k_fo    Natural value of k from the expression Fo = (k * F_step), where Fo is the desired frequency to
 *  be generated, and F_step is the frequency step. Both Fo and Fs are expressed in Hertz.
 * \return  Phase increment, integer value expressed in units of 2*pi/Ps radians, where Ps is the number of samples per
 *  single wave period.
 * \details The following formula is used:\n
 *  dPhi [radians/sample] = 2*pi * (Fo [Hz] / Fs [Hz]),\n
 *  where Fo is the generator frequency in Hz, Fs is the sample rate in Hz.
 * \details If converted to integer value expressed in units of 2*pi/Ps radians, the following formula takes place:\n
 *  dPhi [integer/sample] = dPhi [radians/sample] / 2*pi * Ps = (Fo [Hz] / Fs [Hz]) * Ps,\n
 *  where Ps is the number of samples per single wave period, and 2*pi radians stay for the single wave period.
 * \details As soon as Fs [Hz] = Ps * 2^Ks, where Ks is the sampling rate factor, the following takes place:\n
 *  dPhi [integer/sample] = Fo [Hz] / (Ps * 2^Ks) * Ps = Fo [Hz] / 2^Ks.
 * \details And finally, if Fo is given as integer value expressed in units of 2^Ks Hz, the following holds:\n
 *  dPhi [integer/sample] = Fo [integer].
 * \note    Phase increment per single sample (dPhi), if evaluated as integer value expressed in units of
 *  2*pi/Ps radians, numerically equals to the generator frequency (Fo), if specified as integer value expressed in
 *  units of 2^Ks Hz; and it does not depend on the sampling rate (Fs), number of samples per single wave period (Ps),
 *  and sampling rate factor (Ks).
 */
#define PHASE_INC_PER_SAMPLE(k_fo)    (k_fo)

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
static const dsp_unt22_t sini_lut[1 << SINE_LUT_RANK] = {
    0x3FFFFF, 0x3FFFFA, 0x3FFFEB, 0x3FFFD3, 0x3FFFB0, 0x3FFF84, 0x3FFF4D, 0x3FFF0D,
    0x3FFEC3, 0x3FFE6F, 0x3FFE12, 0x3FFDAA, 0x3FFD38, 0x3FFCBD, 0x3FFC38, 0x3FFBA9,
    0x3FFB10, 0x3FFA6D, 0x3FF9C0, 0x3FF90A, 0x3FF849, 0x3FF77F, 0x3FF6AB, 0x3FF5CD,
    0x3FF4E5, 0x3FF3F3, 0x3FF2F8, 0x3FF1F2, 0x3FF0E3, 0x3FEFCA, 0x3FEEA6, 0x3FED7A,
    0x3FEC43, 0x3FEB02, 0x3FE9B8, 0x3FE863, 0x3FE705, 0x3FE59D, 0x3FE42B, 0x3FE2AF,
    0x3FE12A, 0x3FDF9A, 0x3FDE01, 0x3FDC5E, 0x3FDAB1, 0x3FD8FA, 0x3FD739, 0x3FD56F,
    0x3FD39A, 0x3FD1BC, 0x3FCFD4, 0x3FCDE2, 0x3FCBE6, 0x3FC9E1, 0x3FC7D1, 0x3FC5B8,
    0x3FC395, 0x3FC168, 0x3FBF31, 0x3FBCF1, 0x3FBAA6, 0x3FB852, 0x3FB5F4, 0x3FB38C,
    0x3FB11A, 0x3FAE9F, 0x3FAC19, 0x3FA98A, 0x3FA6F1, 0x3FA44E, 0x3FA1A2, 0x3F9EEB,
    0x3F9C2B, 0x3F9961, 0x3F968D, 0x3F93AF, 0x3F90C8, 0x3F8DD7, 0x3F8ADB, 0x3F87D7,
    0x3F84C8, 0x3F81AF, 0x3F7E8D, 0x3F7B61, 0x3F782B, 0x3F74EC, 0x3F71A2, 0x3F6E4F,
    0x3F6AF2, 0x3F678B, 0x3F641B, 0x3F60A0, 0x3F5D1C, 0x3F598E, 0x3F55F7, 0x3F5255,
    0x3F4EAA, 0x3F4AF5, 0x3F4736, 0x3F436E, 0x3F3F9C, 0x3F3BC0, 0x3F37DA, 0x3F33EB,
    0x3F2FF1, 0x3F2BEE, 0x3F27E2, 0x3F23CB, 0x3F1FAB, 0x3F1B81, 0x3F174D, 0x3F1310,
    0x3F0EC9, 0x3F0A78, 0x3F061E, 0x3F01B9, 0x3EFD4B, 0x3EF8D4, 0x3EF452, 0x3EEFC7,
    0x3EEB32, 0x3EE694, 0x3EE1EC, 0x3EDD3A, 0x3ED87E, 0x3ED3B9, 0x3ECEEA, 0x3ECA11,
    0x3EC52F, 0x3EC043, 0x3EBB4D, 0x3EB64D, 0x3EB144, 0x3EAC32, 0x3EA715, 0x3EA1EF,
    0x3E9CBF, 0x3E9786, 0x3E9243, 0x3E8CF6, 0x3E87A0, 0x3E8240, 0x3E7CD6, 0x3E7763,
    0x3E71E6, 0x3E6C60, 0x3E66D0, 0x3E6136, 0x3E5B93, 0x3E55E6, 0x3E502F, 0x3E4A6F,
    0x3E44A5, 0x3E3ED2, 0x3E38F5, 0x3E330E, 0x3E2D1E, 0x3E2724, 0x3E2121, 0x3E1B14,
    0x3E14FD, 0x3E0EDD, 0x3E08B3, 0x3E0280, 0x3DFC43, 0x3DF5FD, 0x3DEFAD, 0x3DE953,
    0x3DE2F0, 0x3DDC84, 0x3DD60E, 0x3DCF8E, 0x3DC905, 0x3DC272, 0x3DBBD6, 0x3DB530,
    0x3DAE81, 0x3DA7C8, 0x3DA106, 0x3D9A3A, 0x3D9365, 0x3D8C86, 0x3D859E, 0x3D7EAC,
    0x3D77B1, 0x3D70AC, 0x3D699E, 0x3D6286, 0x3D5B65, 0x3D543A, 0x3D4D06, 0x3D45C9,
    0x3D3E82, 0x3D3731, 0x3D2FD7, 0x3D2874, 0x3D2107, 0x3D1991, 0x3D1212, 0x3D0A89,
    0x3D02F6, 0x3CFB5B, 0x3CF3B5, 0x3CEC07, 0x3CE44F, 0x3CDC8D, 0x3CD4C3, 0x3CCCEE,
    0x3CC511, 0x3CBD2A, 0x3CB53A, 0x3CAD40, 0x3CA53D, 0x3C9D31, 0x3C951B, 0x3C8CFC,
    0x3C84D4, 0x3C7CA2, 0x3C7467, 0x3C6C23, 0x3C63D5, 0x3C5B7E, 0x3C531E, 0x3C4AB4,
    0x3C4241, 0x3C39C5, 0x3C313F, 0x3C28B1, 0x3C2019, 0x3C1777, 0x3C0ECD, 0x3C0619,
    0x3BFD5C, 0x3BF495, 0x3BEBC6, 0x3BE2ED, 0x3BDA0B, 0x3BD120, 0x3BC82B, 0x3BBF2D,
    0x3BB626, 0x3BAD16, 0x3BA3FD, 0x3B9ADA, 0x3B91AF, 0x3B887A, 0x3B7F3C, 0x3B75F4,
    0x3B6CA4, 0x3B634A, 0x3B59E7, 0x3B507B, 0x3B4706, 0x3B3D88, 0x3B3401, 0x3B2A70,
    0x3B20D7, 0x3B1734, 0x3B0D88, 0x3B03D3, 0x3AFA15, 0x3AF04E, 0x3AE67E, 0x3ADCA4,
    0x3AD2C2, 0x3AC8D7, 0x3ABEE2, 0x3AB4E4, 0x3AAADE, 0x3AA0CE, 0x3A96B5, 0x3A8C94,
    0x3A8269, 0x3A7835, 0x3A6DF8, 0x3A63B2, 0x3A5963, 0x3A4F0B, 0x3A44AB, 0x3A3A41,
    0x3A2FCE, 0x3A2552, 0x3A1ACD, 0x3A1040, 0x3A05A9, 0x39FB09, 0x39F061, 0x39E5AF,
    0x39DAF5, 0x39D032, 0x39C565, 0x39BA90, 0x39AFB2, 0x39A4CB, 0x3999DB, 0x398EE3,
    0x3983E1, 0x3978D7, 0x396DC3, 0x3962A7, 0x395782, 0x394C54, 0x39411D, 0x3935DE,
    0x392A95, 0x391F44, 0x3913EA, 0x390887, 0x38FD1C, 0x38F1A7, 0x38E62A, 0x38DAA4,
    0x38CF16, 0x38C37E, 0x38B7DE, 0x38AC35, 0x38A083, 0x3894C9, 0x388906, 0x387D3A,
    0x387165, 0x386588, 0x3859A2, 0x384DB3, 0x3841BC, 0x3835BC, 0x3829B3, 0x381DA1,
    0x381187, 0x380565, 0x37F939, 0x37ED05, 0x37E0C9, 0x37D484, 0x37C836, 0x37BBDF,
    0x37AF80, 0x37A319, 0x3796A9, 0x378A30, 0x377DAF, 0x377125, 0x376492, 0x3757F7,
    0x374B54, 0x373EA8, 0x3731F3, 0x372536, 0x371871, 0x370BA3, 0x36FECC, 0x36F1ED,
    0x36E506, 0x36D816, 0x36CB1D, 0x36BE1C, 0x36B113, 0x36A401, 0x3696E7, 0x3689C5,
    0x367C9A, 0x366F66, 0x36622A, 0x3654E6, 0x36479A, 0x363A45, 0x362CE7, 0x361F82,
    0x361214, 0x36049E, 0x35F71F, 0x35E998, 0x35DC09, 0x35CE71, 0x35C0D1, 0x35B329,
    0x35A578, 0x3597C0, 0x3589FF, 0x357C35, 0x356E64, 0x35608A, 0x3552A8, 0x3544BE,
    0x3536CB, 0x3528D1, 0x351ACE, 0x350CC3, 0x34FEB0, 0x34F094, 0x34E271, 0x34D445,
    0x34C611, 0x34B7D5, 0x34A991, 0x349B45, 0x348CF1, 0x347E94, 0x347030, 0x3461C3,
    0x34534E, 0x3444D2, 0x34364D, 0x3427C0, 0x34192B, 0x340A8E, 0x33FBE9, 0x33ED3C,
    0x33DE87, 0x33CFCA, 0x33C105, 0x33B238, 0x33A363, 0x339486, 0x3385A1, 0x3376B5,
    0x3367C0, 0x3358C3, 0x3349BE, 0x333AB2, 0x332B9E, 0x331C81, 0x330D5D, 0x32FE31,
    0x32EEFD, 0x32DFC1, 0x32D07E, 0x32C132, 0x32B1DF, 0x32A284, 0x329321, 0x3283B6,
    0x327444, 0x3264CA, 0x325547, 0x3245BE, 0x32362C, 0x322693, 0x3216F2, 0x320749,
    0x31F799, 0x31E7E0, 0x31D820, 0x31C859, 0x31B88A, 0x31A8B3, 0x3198D4, 0x3188EE,
    0x317900, 0x31690B, 0x31590D, 0x314909, 0x3138FC, 0x3128E9, 0x3118CD, 0x3108AA,
    0x30F87F, 0x30E84D, 0x30D813, 0x30C7D2, 0x30B789, 0x30A739, 0x3096E1, 0x308682,
    0x30761B, 0x3065AD, 0x305537, 0x3044BA, 0x303436, 0x3023AA, 0x301316, 0x30027B,
    0x2FF1D9, 0x2FE12F, 0x2FD07E, 0x2FBFC6, 0x2FAF06, 0x2F9E3F, 0x2F8D70, 0x2F7C9B,
    0x2F6BBE, 0x2F5AD9, 0x2F49ED, 0x2F38FA, 0x2F2800, 0x2F16FE, 0x2F05F5, 0x2EF4E5,
    0x2EE3CE, 0x2ED2AF, 0x2EC18A, 0x2EB05D, 0x2E9F28, 0x2E8DED, 0x2E7CAA, 0x2E6B61,
    0x2E5A10, 0x2E48B8, 0x2E3758, 0x2E25F2, 0x2E1485, 0x2E0310, 0x2DF194, 0x2DE012,
    0x2DCE88, 0x2DBCF7, 0x2DAB5F, 0x2D99C0, 0x2D881A, 0x2D766D, 0x2D64B9, 0x2D52FE,
    0x2D413C, 0x2D2F73, 0x2D1DA3, 0x2D0BCC, 0x2CF9EE, 0x2CE80A, 0x2CD61E, 0x2CC42B,
    0x2CB232, 0x2CA031, 0x2C8E2A, 0x2C7C1C, 0x2C6A07, 0x2C57EB, 0x2C45C8, 0x2C339E,
    0x2C216E, 0x2C0F37, 0x2BFCF9, 0x2BEAB4, 0x2BD868, 0x2BC616, 0x2BB3BD, 0x2BA15D,
    0x2B8EF7, 0x2B7C8A, 0x2B6A16, 0x2B579B, 0x2B451A, 0x2B3292, 0x2B2003, 0x2B0D6E,
    0x2AFAD2, 0x2AE82F, 0x2AD586, 0x2AC2D6, 0x2AB020, 0x2A9D63, 0x2A8A9F, 0x2A77D5,
    0x2A6504, 0x2A522D, 0x2A3F50, 0x2A2C6B, 0x2A1981, 0x2A068F, 0x29F398, 0x29E099,
    0x29CD95, 0x29BA8A, 0x29A778, 0x299460, 0x298142, 0x296E1D, 0x295AF2, 0x2947C0,
    0x293489, 0x29214A, 0x290E06, 0x28FABB, 0x28E76A, 0x28D412, 0x28C0B4, 0x28AD50,
    0x2899E6, 0x288675, 0x2872FE, 0x285F81, 0x284BFE, 0x283874, 0x2824E4, 0x28114E,
    0x27FDB2, 0x27EA10, 0x27D667, 0x27C2B9, 0x27AF04, 0x279B49, 0x278788, 0x2773C1,
    0x275FF4, 0x274C20, 0x273847, 0x272468, 0x271082, 0x26FC97, 0x26E8A6, 0x26D4AE,
    0x26C0B1, 0x26ACAD, 0x2698A4, 0x268495, 0x267080, 0x265C64, 0x264843, 0x26341C,
    0x261FEF, 0x260BBD, 0x25F784, 0x25E346, 0x25CF01, 0x25BAB7, 0x25A667, 0x259211,
    0x257DB6, 0x256954, 0x2554ED, 0x254080, 0x252C0E, 0x251795, 0x250317, 0x24EE94,
    0x24DA0A, 0x24C57B, 0x24B0E6, 0x249C4C, 0x2487AB, 0x247306, 0x245E5A, 0x2449A9,
    0x2434F3, 0x242036, 0x240B75, 0x23F6AD, 0x23E1E1, 0x23CD0E, 0x23B836, 0x23A359,
    0x238E76, 0x23798D, 0x2364A0, 0x234FAC, 0x233AB4, 0x2325B5, 0x2310B2, 0x22FBA9,
    0x22E69A, 0x22D186, 0x22BC6D, 0x22A74F, 0x22922B, 0x227D02, 0x2267D3, 0x22529F,
    0x223D66, 0x222828, 0x2212E4, 0x21FD9B, 0x21E84D, 0x21D2FA, 0x21BDA1, 0x21A843,
    0x2192E0, 0x217D78, 0x21680B, 0x215298, 0x213D20, 0x2127A4, 0x211222, 0x20FC9B,
    0x20E70F, 0x20D17E, 0x20BBE7, 0x20A64C, 0x2090AC, 0x207B06, 0x20655C, 0x204FAD,
    0x2039F9, 0x20243F, 0x200E81, 0x1FF8BE, 0x1FE2F6, 0x1FCD29, 0x1FB757, 0x1FA180,
    0x1F8BA4, 0x1F75C4, 0x1F5FDE, 0x1F49F4, 0x1F3405, 0x1F1E11, 0x1F0819, 0x1EF21B,
    0x1EDC19, 0x1EC612, 0x1EB006, 0x1E99F6, 0x1E83E0, 0x1E6DC7, 0x1E57A8, 0x1E4185,
    0x1E2B5D, 0x1E1530, 0x1DFEFF, 0x1DE8C9, 0x1DD28F, 0x1DBC50, 0x1DA60C, 0x1D8FC4,
    0x1D7977, 0x1D6326, 0x1D4CD0, 0x1D3675, 0x1D2016, 0x1D09B3, 0x1CF34B, 0x1CDCDF,
    0x1CC66E, 0x1CAFF9, 0x1C997F, 0x1C8301, 0x1C6C7F, 0x1C55F8, 0x1C3F6D, 0x1C28DD,
    0x1C1249, 0x1BFBB1, 0x1BE515, 0x1BCE74, 0x1BB7CF, 0x1BA125, 0x1B8A78, 0x1B73C6,
    0x1B5D10, 0x1B4655, 0x1B2F97, 0x1B18D4, 0x1B020D, 0x1AEB42, 0x1AD473, 0x1ABD9F,
    0x1AA6C8, 0x1A8FEC, 0x1A790C, 0x1A6229, 0x1A4B41, 0x1A3455, 0x1A1D65, 0x1A0671,
    0x19EF79, 0x19D87D, 0x19C17D, 0x19AA79, 0x199371, 0x197C65, 0x196555, 0x194E42,
    0x19372A, 0x19200E, 0x1908EF, 0x18F1CC, 0x18DAA5, 0x18C37A, 0x18AC4B, 0x189519,
    0x187DE2, 0x1866A8, 0x184F6A, 0x183829, 0x1820E3, 0x18099A, 0x17F24D, 0x17DAFD,
    0x17C3A9, 0x17AC51, 0x1794F6, 0x177D96, 0x176634, 0x174ECD, 0x173763, 0x171FF6,
    0x170885, 0x16F110, 0x16D998, 0x16C21C, 0x16AA9D, 0x16931A, 0x167B94, 0x16640B,
    0x164C7E, 0x1634ED, 0x161D59, 0x1605C2, 0x15EE27, 0x15D689, 0x15BEE7, 0x15A742,
    0x158F9A, 0x1577EF, 0x156040, 0x15488E, 0x1530D8, 0x15191F, 0x150164, 0x14E9A4,
    0x14D1E2, 0x14BA1C, 0x14A253, 0x148A87, 0x1472B8, 0x145AE6, 0x144311, 0x142B38,
    0x14135C, 0x13FB7D, 0x13E39C, 0x13CBB7, 0x13B3CF, 0x139BE4, 0x1383F6, 0x136C05,
    0x135410, 0x133C19, 0x13241F, 0x130C22, 0x12F423, 0x12DC20, 0x12C41A, 0x12AC11,
    0x129406, 0x127BF8, 0x1263E6, 0x124BD2, 0x1233BB, 0x121BA2, 0x120385, 0x11EB66,
    0x11D344, 0x11BB1F, 0x11A2F8, 0x118ACD, 0x1172A1, 0x115A71, 0x11423F, 0x112A0A,
    0x1111D2, 0x10F998, 0x10E15B, 0x10C91C, 0x10B0DA, 0x109895, 0x10804E, 0x106804,
    0x104FB8, 0x103769, 0x101F18, 0x1006C4, 0x0FEE6E, 0x0FD615, 0x0FBDBA, 0x0FA55C,
    0x0F8CFC, 0x0F749A, 0x0F5C35, 0x0F43CE, 0x0F2B65, 0x0F12F9, 0x0EFA8B, 0x0EE21A,
    0x0EC9A8, 0x0EB133, 0x0E98BB, 0x0E8042, 0x0E67C6, 0x0E4F48, 0x0E36C8, 0x0E1E46,
    0x0E05C1, 0x0DED3A, 0x0DD4B1, 0x0DBC26, 0x0DA399, 0x0D8B0A, 0x0D7279, 0x0D59E5,
    0x0D4150, 0x0D28B8, 0x0D101F, 0x0CF783, 0x0CDEE6, 0x0CC646, 0x0CADA5, 0x0C9501,
    0x0C7C5C, 0x0C63B5, 0x0C4B0B, 0x0C3260, 0x0C19B3, 0x0C0104, 0x0BE854, 0x0BCFA1,
    0x0BB6ED, 0x0B9E37, 0x0B857F, 0x0B6CC5, 0x0B5409, 0x0B3B4C, 0x0B228D, 0x0B09CC,
    0x0AF10A, 0x0AD846, 0x0ABF80, 0x0AA6B9, 0x0A8DF0, 0x0A7525, 0x0A5C59, 0x0A438B,
    0x0A2ABB, 0x0A11EA, 0x09F918, 0x09E043, 0x09C76E, 0x09AE97, 0x0995BE, 0x097CE4,
    0x096408, 0x094B2B, 0x09324D, 0x09196D, 0x09008B, 0x08E7A9, 0x08CEC4, 0x08B5DF,
    0x089CF8, 0x088410, 0x086B27, 0x08523C, 0x083950, 0x082063, 0x080774, 0x07EE84,
    0x07D593, 0x07BCA1, 0x07A3AE, 0x078AB9, 0x0771C4, 0x0758CD, 0x073FD5, 0x0726DC,
    0x070DE1, 0x06F4E6, 0x06DBEA, 0x06C2EC, 0x06A9EE, 0x0690EE, 0x0677EE, 0x065EEC,
    0x0645EA, 0x062CE6, 0x0613E2, 0x05FADC, 0x05E1D6, 0x05C8CF, 0x05AFC7, 0x0596BE,
    0x057DB4, 0x0564A9, 0x054B9E, 0x053291, 0x051984, 0x050076, 0x04E768, 0x04CE58,
    0x04B548, 0x049C37, 0x048326, 0x046A13, 0x045100, 0x0437ED, 0x041ED8, 0x0405C3,
    0x03ECAE, 0x03D398, 0x03BA81, 0x03A169, 0x038852, 0x036F39, 0x035620, 0x033D07,
    0x0323ED, 0x030AD2, 0x02F1B7, 0x02D89C, 0x02BF80, 0x02A664, 0x028D47, 0x02742A,
    0x025B0D, 0x0241EF, 0x0228D1, 0x020FB2, 0x01F693, 0x01DD74, 0x01C455, 0x01AB35,
    0x019215, 0x0178F5, 0x015FD5, 0x0146B4, 0x012D93, 0x011472, 0x00FB51, 0x00E230,
    0x00C90F, 0x00AFED, 0x0096CB, 0x007DAA, 0x006488, 0x004B66, 0x003244, 0x001922,
};

#include <assert.h>

/* Returns generator momentary output for each single sample. */
dsp_int16_t geni(const dsp_uint16_t k_fo) {
    static dsp_int16_t phase = 0;       /* Current momentary phase, wrapped, in units of 2*pi/Ps radians. */
    const dsp_int16_t phi = phase;      /* Copy of the current momentary phase for postponed calculations. */
    const dsp_uint16_t dPhi = PHASE_INC_PER_SAMPLE(k_fo);   /* Phase increment. */
    assert(0 < dPhi && dPhi <= PHASE_CODE_SPAN/2);          /* Equivalent to (0 < Fo and Fo <= Ps/2), where Ps == Fs. */
    phase += dPhi;
    if (phase > PHASE_CODE_MAX)
        phase -= PHASE_CODE_SPAN;
    return sin_i22(phi) >> (DSP_INT22_WIDTH - DSP_INT16_WIDTH);     /* skeleton */
}
