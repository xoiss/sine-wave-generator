/**@file
 * @brief   Definitions of C language extensions.
 * @details This file provides definitions for different useful utilities, mostly macros, used as an ISO/ANSI C language
 *  extensions. These extensions are compatible (I hope) with language standards through C89, C90, C99, C11 including
 *  their GNU versions and compiler options -ansi, -pedantic, -Wall.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef CEXT_H
#define CEXT_H

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Macros for concatenation of tokens and identifiers.
 * @{
 */
/**@brief   Composes a single contiguous identifier from two tokens.
 * @param[in]   pt  -- a token that will be used as the prefix of the composed identifier.
 * @param[in]   st  -- a token that will be used as the suffix of the composed identifier.
 * @return  Identifier composed as the literal concatenation of two tokens: \p pt and \p st.
 * @details The \p pt must be a valid identifier. The \p st must be either a valid identifier, or a trailing part of a
 *  valid identifier -- i.e., it may be a numeric, or a single token composed from a numeric followed by a valid
 *  identifier, for example: \c _foo, \c 123, \c 777_bar.
 * @note    If a macro is specified either for \p pt or \p st (which in turn returns an allowed token), it is not
 *  expanded -- i.e., namely the macro's own identifier will be used but not the token string which that macro is
 *  associated to. Use \c CONCAT in this case instead of \c CONCAT_TOKENS.
 */
#define CONCAT_TOKENS(pt, st)       pt##st

/**@brief   Composes a single contiguous identifier from two tokens or macros associated to tokens.
 * @param[in]   pm  -- a token or a macro associated to a token that will be used as the prefix of the composed
 *  identifier.
 * @param[in]   sm  -- a token or a macro associated to a token that will be used as the suffix of the composed
 *  identifier.
 * @return  Identifier composed as the literal concatenation of tokens associated with \p pm and \p sm.
 * @details The \p pm must be a valid identifier or must be associated directly or indirectly with a valid identifier.
 *  The \p sm must itself be or must be associated either with a valid identifier, or a trailing part of a valid
 *  identifier -- i.e., it may be associated with a numeric, or a single token composed from a numeric followed by a
 *  valid identifier, for example: \c _foo, \c 123, \c 777_bar.
 * @details Different macros may be used directly both for \p pm and \p sm, for example \c \__COUNTER__ or \c \__LINE__
 *  may be used for \p sm.
 * @note    If a macro is specified either for \p pm or \p sm, it may be associated directly or indirectly with an
 *  allowed token, and the expansion will be followed through the whole chain of definitions until the final token.
 *  However, the final and all the intermediate associations must be defined in the translation unit above the place
 *  where the \c CONCAT macro is instantiated; otherwise the macro expansion process will stop in the middle and \p pm
 *  or \p sm respectively will be assigned with the intermediate macro's identifier instead of the final token.
 */
#define CONCAT(pm, sm)      CONCAT_TOKENS(pm, sm)
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Macros for validation of conditions during the compile time.
 * @{
 */
/**@brief   Validates the asserted condition during the compile time and puts the error message.
 * @param[in]   cond    -- an asserted condition.
 * @param[in]   msg     -- an error message identifier.
 * @details The asserted condition \p cond shall be computable at the compile time and evaluate to an integer constant.
 *  It shall evaluate to a non-zero value if the condition holds, and to zero otherwise.
 * @details The \p msg must be specified with a valid identifier. Note, it must not be a sting literal.
 * @details If the \p cond does not hold, the compiler logs a compile time error with the message \p msg. The exact
 *  format of the error message depends on the compiler and the language standard. However, the \p msg will be included
 *  into it in quotes. If \p msg is not needed, the \c static_assert shall be used instead of \c static_assert_msg.
 * @details This macro may be used both outside and inside a function definition block. The number of occurrences in the
 *  same translation unit and even on the same line of code is not limited.
 * @note    Under the ANSI C89 / ISO C90 standard, this macro shall be used before the first line of code in a code
 *  block. Note that even an empty instruction (i.e., an alone ';' token) or empty code block (i.e., a couple of braces)
 *  forms the line of code. If it is necessary to use this macro in the middle of the code block, it shall be surrounded
 *  with braces forming a nested code block.
 * @sa  Some inspiration was taken from these sources:\n
 *  http://stackoverflow.com/questions/3385515/static-assert-in-c/4815532#4815532\n
 *  http://www.pixelbeat.org/programming/gcc/static_assert.html
 */
#if (__STDC_VERSION__ >= 201112L)
#define static_assert_msg(cond, msg)        _Static_assert(cond, #msg)
#else
#define static_assert_msg(cond, msg)        struct CONCAT(__uid, __COUNTER__) { int msg : !!(cond); }
#endif

/**@brief   Validates the asserted expression during the compile time and puts an error.
 * @param[in]   cond    -- an asserted condition.
 * @details The asserted condition \p cond shall be computable at the compile time and evaluate to an integer constant.
 *  It shall evaluate to a non-zero value if the condition holds, and to zero otherwise.
 * @details If the \p cond does not hold, the compiler logs a compile time error.
 * @details This macro may be used both outside and inside a function definition block. The number of occurrences in the
 *  same translation unit and even in the same line of code is not limited.
 * @note    Under the ANSI C89 / ISO C90 standard, this macro shall be used before the first line of code in a code
 *  block. Note that even an empty instruction (i.e., an alone ';' token) or empty code block (i.e., a couple of braces)
 *  forms the line of code. If it is necessary to use this macro in the middle of the code block, it shall be surrounded
 *  with braces forming a nested code block.
 */
#define static_assert(cond)     static_assert_msg(cond, _)
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Macros for working with binary magnitude.
 * @{
 */
/**@brief   Returns the binary exponent of the given value.
 * @param[in]   x   -- the power.
 * @return  The value of two in the power of the given exponent.
 */
#define POW2(x)         (1uL << (x))

/**@brief   Returns the binary logarithm of the given value.
 * @param[in]   x   -- the exponent.
 * @return  The value of binary logarithm of the given exponent rounded down to the nearest integer.
 */
#define LOG2(x)         (\
    x <= POW2(0)  ? 0  : x <= POW2(1)  ? 1  : x <= POW2(2)  ? 2  : x <= POW2(3)  ? 3  :\
    x <= POW2(4)  ? 4  : x <= POW2(5)  ? 5  : x <= POW2(6)  ? 6  : x <= POW2(7)  ? 7  :\
    x <= POW2(8)  ? 8  : x <= POW2(9)  ? 9  : x <= POW2(10) ? 10 : x <= POW2(11) ? 11 :\
    x <= POW2(12) ? 12 : x <= POW2(13) ? 13 : x <= POW2(14) ? 14 : x <= POW2(15) ? 15 :\
    x <= POW2(16) ? 16 : x <= POW2(17) ? 17 : x <= POW2(18) ? 18 : x <= POW2(19) ? 19 :\
    x <= POW2(20) ? 20 : x <= POW2(21) ? 21 : x <= POW2(22) ? 22 : x <= POW2(23) ? 23 :\
    x <= POW2(24) ? 24 : x <= POW2(25) ? 25 : x <= POW2(26) ? 26 : x <= POW2(27) ? 27 :\
    x <= POW2(28) ? 28 : x <= POW2(29) ? 29 : x <= POW2(30) ? 30 : x <= POW2(31) ? 31 : 32)
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Macros for working with bit masks.
 * @{
 */
/**@brief   Composes a single bit mask having only one given bit selected.
 * @param[in]   nr  -- index (starting with 0) of the bit to be selected.
 * @return  The requested bit mask, unsigned long integer.
 * @note    The \p nr must be less than the number of bits in the native unsigned long integer data type of the host
 *  simulator platform to avoid the undefined behavior.
 */
#define BIT(nr)         (POW2(nr))

/**@brief   Composes a multiple bit mask having the given number of contiguous lower order bits selected.
 * @param[in]   nr  -- number of the lower order bits to be selected.
 * @return  The requested bit mask, unsigned long integer.
 * @note    The \p nr must be less than the number of bits in the native unsigned long integer data type of the host
 *  simulator platform to avoid the undefined behavior.
 */
#define BIT_MASK(nr)    (BIT(nr) - 1)
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Macros for working with arrays.
 * @{
 */
/**\brief   Returns the number of elements in the given array.
 * \param[in]   x   -- identifier of the array variable.
 * \return  Number of elements in the array \p x.
 * \note    This macro cannot be used for arrays passed to a function through its parameters list as soon as such
 *  parameters are considered as pointers by the compiler (namely pointers, but not arrays), and the information about
 *  the number of elements is ignored by the compiler even it is explicitly specified in the function declaration.
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* CEXT_H */
