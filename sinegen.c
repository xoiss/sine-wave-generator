#if 0
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
#endif
extern int f(void) {
    return 1;
}
