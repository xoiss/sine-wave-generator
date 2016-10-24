#include "sinegen.h"

/**
 * \name    Integer data types for internal needs of DSP algorithms.
 * \note    In the case when a DSP integer data type is represented with a wider data type of the simulator host
 *  platform, the higher order bits of such type are left unused. In the case of unsigned integer the unused bits are
 *  padded with zeroes; and in the case of signed integer the sign bit is propagated through them.
 */
/**@{*/
typedef int32_t     dsp_int22_t;        /*!< DSP signed integer, 22-bit. */
typedef uint32_t    dsp_uint22_t;       /*!< DSP unsigned integer, 22-bit. */
/**@}*/

/**
 * \brief   Width of the 22-bit integer data type used for internal needs of DSP algorithms, in bits.
 * \details DSP algorithms operate internally with 22-bit integer values which are used either as 22-bit unsigned
 *  integers, or 22-bit signed integers with 21 bits used for value and the 22th bit used for the sign.
 */
#define DSP_INT22_WIDTH     (22)

/**
 * \brief   Returns wave momentary phase increment (dPhi) per single sample depending on the generator frequency (Fo).
 * \param[in]   fo      Generator frequency (Fo), integer value expressed in units of 2^Ks Hz, where Ks is the sampling
 *  rate factor. Here Ks is integer and may be zero, or either positive, or negative. When Ks equals zero, Fo is
 *  actually expressed in whole Hertz.
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
#define PHASE_INC_PER_SAMPLE(fo)    (fo)

/**
 * \brief   Returns value of sinus given a momentary phase.
 * \param[in]   phi     Momentary phase (Phi), integer value expressed in units of 2*pi/Ps radians, where Ps is the
 *  number of samples per single wave period.
 * \return  Momentary amplitude of the function 2^(WI-1) * sin(Phi), integer value, where WI is the width of the
 *  extended integer data type used for DSP algorithms, in bits.
 * \details Valid values for \p Phi are from (-Ps/2) to (+Ps/2 - 1), where Ps is the number of samples per single wave
 *  period, which corresponds to the range from (-pi) to (+pi - 2*pi/Ps) radians; zero value corresponds to 0 radians.
 * \details Function returns values in the range from -2^(WI-1) to +2^(WI-1), which corresponds to the range
 *  from -1 to +1; zero value corresponds to 0.
 */
static dsp_int22_t sini(const dsp_int16_t phi);

#include <assert.h>

/* Returns value of sinus given a momentary phase. */
dsp_int22_t sini(const dsp_int16_t phi) {
    assert(PHASE_CODE_MIN <= phi && phi <= PHASE_CODE_MAX);
    return phi << (DSP_INT22_WIDTH - DSP_INT16_WIDTH);      /* skeleton */
}

/* Returns generator momentary output for each single sample. */
dsp_int16_t geni(const dsp_uint16_t fo) {
    static dsp_int16_t phase = 0;       /* Current momentary phase, wrapped, in units of 2*pi/Ps radians. */
    const dsp_int16_t phi = phase;      /* Copy of the current momentary phase for postponed calculations. */
    const dsp_uint16_t dPhi = PHASE_INC_PER_SAMPLE(fo);     /* Phase increment. */
    assert(0 < dPhi && dPhi <= PHASE_CODE_SPAN/2);       /* Equivalent to (0 < Fo and Fo <= Ps/2), where Ps == Fs. */
    phase += dPhi;
    if (phase > PHASE_CODE_MAX)
        phase -= PHASE_CODE_SPAN;
    return sini(phi) >> (DSP_INT22_WIDTH - DSP_INT16_WIDTH);    /* skeleton */
}
