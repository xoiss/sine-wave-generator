#ifndef SINEGEN_H
#define SINEGEN_H

#include <stdint.h>

/**
 * \name    Integer data types for external interface to DSP algorithms.
 * \details In the case when a DSP integer data type is represented with a wider data type of the simulator host
 *  platform, the higher order bits of such type are left unused. In the case of unsigned integer the unused bits are
 *  filled with zeroes; and in the case of signed integer the sign bit is propagated through them.
 */
/**@{*/
typedef int16_t     dsp_int16_t;        /*!< Signed integer, 16-bit. */
typedef uint16_t    dsp_uint16_t;       /*!< Unsigned integer, 16-bit. */
/**@}*/

/**
 * \brief   Width of the conventional integer data type used for external interface to DSP algorithms, in bits.
 * \details DSP algorithms operate externally with 16-bit integer values which are used either as 16-bit unsigned
 *  integers or 16-bit signed integers with 15 bits used for value and the 16th bit used for the sign.
 */
#define DSP_CONV_INT_WIDTH      (16)

/**
 * \brief   Number of samples per single wave period (Ns).
 * \note    This value must be a power of two, and it must be in the range from 256 to 65536.
 */
#define SAMPLES_PER_PERIOD      (65536)
#if (256 > SAMPLES_PER_PERIOD || SAMPLES_PER_PERIOD > 65536)
#   error   Number of samples per single wave period must be in the range from 256 to 65536.
#endif
#if ((SAMPLES_PER_PERIOD & (SAMPLES_PER_PERIOD - 1)) != 0)
#   error   Number of samples per single wave period must be a power of two.
#endif

/**
 * \brief   Sampling rate factor (Ks).
 * \details Sampling rate (Fs) is expressed in Hertz with the following formula:\n
 *  Fs [Hz] = Ns * 2^Ks,\n
 *  where Ns is the number of samples per single wave period, Ks is the sampling rate factor.
 * \note    If expressed not in whole Hertz but in units of 2^Ks Hz, the sampling rate (Fs) numerically equals Ns.
 * \details When Ks is set to zero (i.e., Fs numerically equals to Ns), it allows correct generation of frequencies
 *  starting with 1 Hz and theoretically up to Fs/2 Hz with the step of 1 Hz.
 * \details When Ks is set to a positive value (i.e., Fs numerically equals to a two-in-a-power multiple of Ns), it
 *  allows correct generation of frequencies starting with 2^Ks Hz and theoretically up to Fs/2 Hz with the step of
 *  2^Ks Hz. This configuration extends the generator range to higher frequencies (as soon as Fs grows with 2^Ks), but
 *  reduces the resolution, both by 2^Ks.
 * \details When Ks is set to a negative value (i.e., Fs numerically equals to a two-in-a-power divider of Ns), it
 *  allows correct generation of frequencies starting with 1/2^|Ks| Hz and theoretically up to Fs/2 Hz with the step of
 *  1/2^|Ks| Hz (notice that Ks is negative here). This configuration increases the resolution to fractions of Hertz,
 *  but shrinks the upper boundary of the generator range, both by 2^|Ks|.
 * \details When it is necessary to extend the generator range to higher frequencies but not to loose in the resolution
 *  the number of samples per single wave period (Ns) shall be increased.
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
 *  Fs [Hz] = Ns * 2^Ks,\n
 *  where Ns is the number of samples per single wave period, Ks is the sampling rate factor.
 * \note    The generator hardware shall be configured properly to actually achieve the declared sampling rate.
 */
#define SAMLING_RATE_HZ     (MUL_2_POW_KS(SAMPLES_PER_PERIOD))

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
