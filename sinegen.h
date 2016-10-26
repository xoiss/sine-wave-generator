#ifndef SINEGEN_H
#define SINEGEN_H

#include "dspfix.h"

/**
 * \name    Fixed point data type used for representing the momentary phase of the generator and its effective width.
 * \details The momentary phase of the generator wraps within the range [-pi; +pi) radians. The lower boundary is
 *  included into, and the upper boundary is excluded from the range.
 * \details The momentary phase is represented as SQ0.15 signed fixed point value, no integer bits, 15 fractional bits. 
 *  Its effective width is Pw = 15 bits. The code takes values in the range from -1 to +(1 - 1/2^Pw) with resolution 
 *  of 1/2^Pw which correspond to the range from -pi to +(pi - pi/2^Pw) with resolution of pi/2^Pw radians.

 * \note    The value of Pw must be sufficiently high to achieve the appropriate accuracy, but it must not exceed the
 *  width of the 16-bit integer data type used for external interface to DSP algorithms. Also, the lower boundary for
 *  Pw-1 is defined as the binary logarithm of the number of entries in the phase-to-sine lookup table.

 * \details The effective width of a fixed point data type is defined as the sum of quantities of all integer M and all 
 *  fractional N bits. The sign bit (for the case of signed fixed point) is not included in the result.

 */
/**@{*/
/** Fixed point data type used for representing the momentary phase of the generator. */
typedef dsp_sq015_t phase_t;
/** Effective width of the fixed point data type used for representing the momentary phase of the generator, Pw, in 
 *  bits. */
#define PHASE_CODE_WIDTH    (DSP_SQ015_WIDTH)
/**@}*/




/**
 * \brief   The effective width of the fixed point code representing the momentary phase of the generator, Pw, in bits.
 */
#define PHASE_CODE_WIDTH    (16)

/**
 * \brief   Rank of the phase-to-sine lookup table (LUT).
 * \details The lookup table rank is defined as the binary logarithm of the number of entries in the lookup table. In
 *  turn, the number of entries in the lookup table shall be a whole power of two.
 * \details The phase-to-sine lookup table rank is used also as the lower boundary for the phase code width.
 * \details In this implementation of DSP algorithms the phase-to-sine lookup table has 1024 entries.
 */
#define SINE_LUT_RANK       (10)

/* Validate the Pw value. */
#if !(SINE_LUT_RANK <= (PHASE_CODE_WIDTH - 1) && PHASE_CODE_WIDTH <= DSP_INT16_WIDTH)
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
 * \details The phase code span Ps corresponds to one whole period 2*pi radian of the generated frequency Fo; the phase
 *  code single unit corresponds to 2*pi/Ps radians.
 * \details Generator increments the momentary phase code by one unit at each sampling tick when it is generating the
 *  lowest supported frequency Fo_min. For higher frequencies Fo = (Fo_min * k) the momentary phase code is incremented
 *  by k units at each sampling tick - the same value k is used between all ticks, rational values are not used.
 */
/**@{*/
/** Span of the momentary phase integer code, Ps. */
#define PHASE_CODE_SPAN     (1 << PHASE_CODE_WIDTH)
/** Minimum value of the momentary phase integer code, Pmin. */
#define PHASE_CODE_MIN      (-PHASE_CODE_SPAN/2)
/** Maximum value of the momentary phase integer code, Pmax. */
#define PHASE_CODE_MAX      (+PHASE_CODE_SPAN/2 - 1)
/**@}*/

/**
 * \brief   The binary order of magnitude of the generated frequencies series step, Ks.
 * \details Generator is able to produce the series of frequencies Fo_k = (k * Fo_min), where k is a natural number, and
 *  Fo_min is the lowest supported frequency. Value of k is limited only by the chosen sampling frequency.
 * \details From the formula it follows that Fo_min plays also the role of the common difference of successive members
 *  in the Fo_k series - i.e., the step of the frequencies series Fo_min = Fo_step. Assuming this fact the following
 *  holds Fo_k = (k * Fo_step).
 * \details This implementation requires Fo_step be an integer power of two. It may be expressed in terms of Ks as
 *  Fo_step = 2^Ks. Consequently members of the generated frequencies series may be expressed as Fo_k = (k * 2^Ks). And
 *  for the lowest generated frequency it holds Fo_min = 1 * 2^Ks = 2^Ks.
 * \details Theoretically Ks may have arbitrary integer value (zero, positive or negative) and is restricted only by the
 *  actual hardware capabilities. For example, zero value of Ks defines Fo_step and Fo_min both equal to 1 Hz.
 * \details If it is required for the Fo_step to be a fraction of 1 Hz, the appropriate negative value of Ks shall be
 *  taken. However, reducing the Fo_step more than actually required is not expedient as soon as with the phase span
 *  Ps already bounded above with the width of the integer data type it will be necessary to reduce the sampling rate
 *  which in turn will lower the upper limit of the generated frequency (restrict the maximum k in Fo_k expression).
 */
#define FREQ_STEP_ORDER     (0)

/**
 * \name    Set of macro functions performing multiplication on and division by the frequencies series step F_step.
 * \param[in]   x   The first multiplier or dividend.
 * \return  Product or quotient.
 */
/**@{*/
/** Multiplies on the frequencies series step. */
#define MUL_FREQ_STEP(x)    (FREQ_STEP_ORDER == 0 ? (x) : FREQ_STEP_ORDER > 0 ?\
                                    (x) << FREQ_STEP_ORDER : (x) >> -FREQ_STEP_ORDER)
/** Divides by the frequencies series step. */
#define DIV_FREQ_STEP(x)    (FREQ_STEP_ORDER == 0 ? (x) : FREQ_STEP_ORDER > 0 ?\
                                    (x) >> FREQ_STEP_ORDER : (x) << -FREQ_STEP_ORDER)
/**@}*/

/**
 * \brief   Sampling rate (sampling frequency), Fs, in Hertz.
 * \details At the lowest supported frequency Fo_min generator increments the momentary phase namely by one unit at each
 *  sampling tick, so to cover a single period of Fo_min the phase shall be propagated Ps times. Single period of Fo_min
 *  by definition has duration of 1/Fo_min seconds, and this period shall comprise Ps sampling ticks (or periods of Fs).
 *  It means that 1 second embraces exactly Ps*Fo_min periods of Fs, and consequently Fs = (Ps * Fo_min).
 * \details Using equation Fo_min = 2^Ks, finally come with the formula Fs = (Ps * 2^Ks), where Ps is the phase span,
 *  Ks is the binary order of magnitude of the generated frequencies series step, and Fs value is in Hertz.
 * \note    The generator hardware shall be configured properly to actually achieve the declared sampling rate Fs.
 */
#define SAMPLING_RATE_HZ    (MUL_FREQ_STEP(PHASE_CODE_SPAN))

/**
 * \name    Set of derivative restrictions on the generated frequencies range and series.
 * \details The generated frequencies series step is expressed with the formula Fo_step = 2^Ks, where Ks is the binary
 *  order of magnitude of the generated frequencies series step.
 * \details The lowest generated frequency is expressed with the formula Fo_min = Fo_step.
 * \details The highest generated frequency is expressed with the formula Fo_max = Fs/2, where Fs is the sampling rate.
 * \note    These constants shall be used by the application for specifying proper tasks to the generator.
 */
/**@{*/
/** The generated frequencies series step, Fo_step, in Hertz. */
#define FREQ_STEP       (1 << FREQ_STEP_ORDER)
/** The lowest generated frequency, Fo_min, in Hertz. */
#define FREQ_MIN        (FREQ_STEP)
/** The highest generated frequency, Fo_max, in Hertz. */
#define FREQ_MAX        (SAMPLING_RATE_HZ / 2)
/**@}*/

/**
 * \brief   Returns generator momentary output for each single sampling tick and propagates the generator state.
 * \param[in]   k_fo    Natural value of k from the expression Fo = (k * F_step), where Fo is the desired frequency to
 *  be generated, and F_step is the frequency step. Both Fo and Fs are expressed in Hertz.
 * \return  Momentary amplitude of the generated signal, integer value.
 * \details When this function is called it calculates the generator output sample for the current momentary phase, then
 *  propagates the momentary phase, and finally returns the calculated sample value. The very first time this function
 *  is called it returns 0 as soon as the generator phase is initially set to 0 radians.
 * \details Function returns integer values in the range from -(2^15-1) to +(2^15-1), which corresponds to the range
 *  from -Va to +Va, where Va is the maximum possible amplitude of the generated signal; zero value corresponds to 0.
 * \details Application is allowed to change Fo at arbitrary moment when calling this function, generator will produce
 *  phase correct output wave. The new value of Fo is accepted starting from the current moment and affects the value
 *  returned by this function when it is called next times.
 * \note    For correct operation Fo must not exceed Fo_max = Fs/2 at least, where Fs is the sampling rate.
 */
extern dsp_int16_t geni(const dsp_uint16_t k_fo);

#endif /* SINEGEN_H */
