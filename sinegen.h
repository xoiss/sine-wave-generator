#ifndef SINEGEN_H
#define SINEGEN_H

#include "dspfix.h"

/**
 * \name    Integer data types for external interface to DSP algorithms.
 * \note    In the case when a DSP integer data type is represented with a wider data type of the simulator host
 *  platform, the higher order bits of such type are left unused. In the case of unsigned integer the unused bits are
 *  padded with zeroes; and in the case of signed integer the sign bit is propagated through them.
 */
/**@{*/
typedef int16_t     dsp_int16_t;        /*!< DSP signed integer, 16-bit. */
typedef uint16_t    dsp_uint16_t;       /*!< DSP unsigned integer, 16-bit. */
/**@}*/

/**
 * \brief   Width of the 16-bit integer data type used for external interface to DSP algorithms, in bits.
 * \details DSP algorithms operate externally with 16-bit integer values which are used either as 16-bit unsigned
 *  integers, or 16-bit signed integers with 15 bits used for value and the 16th bit used for the sign.
 */
#define DSP_INT16_WIDTH     (16)

/**
 * \brief   Width of the integer code representing the momentary phase of the generator (Pw), in bits.
 * \details The momentary phase of the generator wraps within the range [-pi; +pi) radians. The lower boundary is
 *  included into, and the upper boundary is excluded from the range.
 * \details For needs of DSP algorithms the momentary phase is represented as a signed integer value in the range
 *  from -2^(Pw-1) to +2^(Pw-1)-1.
 * \note    The value of Pw must be sufficiently high to achieve the appropriate accuracy, but it must not exceed the
 *  width of the 16-bit integer data type used for external interface to DSP algorithms.
 */
#define PHASE_CODE_WIDTH    (16)

/* Validate the Pw value. */
#if !(8 <= PHASE_CODE_WIDTH && PHASE_CODE_WIDTH <= DSP_INT16_WIDTH)
#   error   Value of Pw is out of the valid range.
#endif

/**
 * \name    Set of derivative parameters of the integer code representing the momentary phase of the generator.
 * \details For needs of DSP algorithms the momentary phase is represented as a signed integer value in the range
 *  from -2^(Pw-1) to +2^(Pw-1)-1, where Pw is the width of the integer code representing the momentary phase.
 * \details Special notation is introduced for this range: Pmin = -2^(Pw-1), Pmax = +2^(Pw-1)-1, Ps = 2^Pw. According to
 *  these the momentary phase code belongs to the range from Pmin to Pmax inclusively and provides Ps different codes.
 * \details The momentary phase code Pmin corresponds to -pi radians; code Pmax+1 corresponds to +pi radians; code Pmax
 *  corresponds to (+pi - 2*pi/Ps) radians; code 0 corresponds to 0 radians.
 * \details The phase code span Ps corresponds to one whole period 2*pi radian; the phase code single unit corresponds
 *  to 2*pi/Ps radians.
 * \details Generator increments the momentary phase code by one unit at each sampling tick when it is generating the
 *  lower supported frequency Fo_min. For higher frequencies Fo = (Fo_min * k) the momentary phase code is incremented
 *  by k units at each sampling tick (the same value k is used between all ticks).
 */
/**@{*/
/** Span of the momentary phase integer code (Ps). */
#define PHASE_CODE_SPAN     (1 << PHASE_CODE_WIDTH)
/** Minimum value of the momentary phase integer code (Pmin). */
#define PHASE_CODE_MIN      (-PHASE_CODE_SPAN/2)
/** Maximum value of the momentary phase integer code (Pmax). */
#define PHASE_CODE_MAX      (+PHASE_CODE_SPAN/2 - 1)
/**@}*/

/**
 * \brief   Sampling rate factor (Ks).
 * \details Sampling rate (Fs) is expressed in Hertz with the following formula:\n
 *  Fs [Hz] = Ps * 2^Ks,\n
 *  where Ps is the number of samples per single wave period, Ks is the sampling rate factor.
 * \note    If expressed not in whole Hertz but in units of 2^Ks Hz, the sampling rate (Fs) numerically equals Ps.
 * \details When Ks is set to zero (i.e., Fs numerically equals to Ps), it allows correct generation of frequencies
 *  starting with 1 Hz and theoretically up to Fs/2 Hz with the step of 1 Hz.
 * \details When Ks is set to a positive value (i.e., Fs numerically equals to a two-in-a-power multiple of Ps), it
 *  allows correct generation of frequencies starting with 2^Ks Hz and theoretically up to Fs/2 Hz with the step of
 *  2^Ks Hz. This configuration extends the generator range to higher frequencies (as soon as Fs grows with 2^Ks), but
 *  reduces the resolution, both by 2^Ks.
 * \details When Ks is set to a negative value (i.e., Fs numerically equals to a two-in-a-power divider of Ps), it
 *  allows correct generation of frequencies starting with 1/2^|Ks| Hz and theoretically up to Fs/2 Hz with the step of
 *  1/2^|Ks| Hz (notice that Ks is negative here). This configuration increases the resolution to fractions of Hertz,
 *  but shrinks the upper boundary of the generator range, both by 2^|Ks|.
 * \details When it is necessary to extend the generator range to higher frequencies but not to loose in the resolution
 *  the number of samples per single wave period (Ps) shall be increased.
 * \note    The generator frequency (Fo) is specified as an integer value expressed in units of 2^Ks Hz.
 */
#define SAMPLING_RATE_FACTOR    (0)

/**
 * \name    Set of macro functions performing multiplication on and division by two in the power of the sampling rate
 *  factor (Ks).
 * \param[in]   x   The first multiplier or dividend.
 * \return  Product or quotient.
 */
/**@{*/
/** Multiplies on two in the power of the sampling rate factor. */
#define MUL_2_POW_KS(x)     (SAMPLING_RATE_FACTOR == 0 ? (x) : SAMPLING_RATE_FACTOR > 0 ?\
                                    (x) << SAMPLING_RATE_FACTOR : (x) >> -SAMPLING_RATE_FACTOR)
/** Divides by two in the power of the sampling rate factor. */
#define DIV_2_POW_KS(x)     (SAMPLING_RATE_FACTOR == 0 ? (x) : SAMPLING_RATE_FACTOR > 0 ?\
                                    (x) >> SAMPLING_RATE_FACTOR : (x) << -SAMPLING_RATE_FACTOR)
/**@}*/

/**
 * \brief   Sampling rate (Fs), Hz.
 * \details Sampling rate (Fs) is expressed in Hertz with the following formula:\n
 *  Fs [Hz] = Ps * 2^Ks,\n
 *  where Ps is the number of samples per single wave period, Ks is the sampling rate factor.
 * \note    The generator hardware shall be configured properly to actually achieve the declared sampling rate.
 */
#define SAMLING_RATE_HZ     (MUL_2_POW_KS(PHASE_CODE_SPAN))

/**
 * \brief   Returns generator momentary output for each single sample.
 * \param[in]   fo      Generator frequency (Fo), integer positive value expressed in units of 2^Ks Hz, where Ks is the
 *  sampling rate factor. Here Ks is integer and may be zero, or either positive, or negative. When Ks equals zero,
 *  Fo is actually expressed in whole Hertz.
 * \return  Momentary amplitude of the generated signal, integer value.
 * \details Function returns integer values in the range from -2^(CI-1) to +2^(CI-1), where CI is the width of the
 *  conventional integer data type used for DSP algorithms, in bits, which corresponds to the range from -Va to +Va,
 *  where Va is the maximum possible amplitude of the generated signal; zero value corresponds to 0.
 * \note    For correct operation Fo must not exceed Fs/2 at least, where Fs is the sampling rate.
 */
extern dsp_int16_t geni(const dsp_uint16_t fo);

#endif /* SINEGEN_H */
