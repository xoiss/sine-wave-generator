/**@file
 * @brief   Interface to the sine wave generator.
 * @details This file provides declarations for the set of functions used to manipulate the sine wave generator, and
 *  declaration of the generator descriptor data structure.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

#ifndef SINEGEN_H
#define SINEGEN_H

/*--------------------------------------------------------------------------------------------------------------------*/
#include "fixtypes.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Data structure for a sine wave generator descriptor.
 */
struct gen_descr_t {
    /* Oscillator state and attributes. */
    uq016_t freq;       /**< Frequency of the oscillator. */
    uq016_t phi;        /**< Momentary phase of the oscillator. */
    uq016_t att;        /**< Momentary attenuation of the output signal. */
    /* Postprocessor state and attributes. */
    uq016_t phi0;       /**< Momentary phase of the oscillator at the start of the postprocessing interval. */
    sq015_t val0;       /**< Momentary amplitude of the output signal at the start of the postprocessing interval. */
    bool_t  pp;         /**< Equals to 1 if the postprocessing is enabled; 0 otherwise. */
    uq016_t phi1;       /**< Momentary phase of the oscillator at the end of the postprocessing interval. */
    sq015_t val1;       /**< Momentary amplitude of the output signal at the end of the postprocessing interval. */
    ui16_t  steps;      /**< Number of steps between val0 and val1 on the interval from phi0 to phi1; 0 if disabled. */
    ui16_t  sampl;      /**< Number of samples between phi0 to phi1; 0 if disabled. */
    ui16_t  msize;      /**< Size of the main steps of the pattern, in samples. */
    ui16_t  asize;      /**< Size of the additional step of the pattern, in samples. */
    ui16_t  sidx;       /**< Index of the current sample within the interval from phi0 to phi1, starting with 0. */
    ui16_t  ridx;       /**< The first index within the first right-hand step of the pattern. */
    ui16_t  aidx;       /**< The first index within the additional step of the pattern. */
};

/*--------------------------------------------------------------------------------------------------------------------*/
/**@name    Set of functions providing interface to a sine wave generator.
 * @{
 */
/**@brief   Initializes a sine wave generator.
 * @param[in,out]   pgen    -- pointer to the initialized generator descriptor object.
 * @details During initialization the following values assigned to the generator attributes:
 *  - frequency     -- is set to 0, which means that the generation is paused.
 *  - phase         -- is set to 0, just the initial phase.
 *  - attenuation   -- is set to 0, which means no attenuation.
 */
extern void gen_init(struct gen_descr_t * const pgen);

/**@brief   Assigns the generator frequency.
 * @param[in,out]   pgen    -- pointer to a generator descriptor object.
 * @param[in]       freq    -- a new value of the oscillator frequency in terms of the sampling frequency - i.e., the
 *  ratio Fo/Fs, where Fo is the oscillator fequency and Fs is the sampling frequency, both in hertz.
 * @details The allowed values of \p freq are the subset of UQ0.16 values in the discrete range [0.0; 0.25] with
 *  resolution of 1/2^16. This range corresponds to the floating point range [0; 0.25] with resolution of 1/2^16:
 *  | Fo/Fs         | fixed point value  | container code |
 *  |---------------|--------------------|----------------|
 *  | 0.25          | 0.25               | 0x4000         |
 *  | 0.25 - 1/2^16 | 0.2499847412109375 | 0x3FFF         |
 *  | 1/2^16        | 0.0000152587890625 | 0x0001         |
 *  | 0             | 0.0 (*)            | 0x0000         |
 * @note    If \p freq equals 0, the generator is paused and returns the same output signal level after each sampling
 *  step. The momentary phase is not propagated during this period.
 */
extern void gen_set_freq(struct gen_descr_t * const pgen, const uq016_t freq);

/**@brief   Assigns the generator phase.
 * @param[in,out]   pgen    -- pointer to a generator descriptor object.
 * @param[in]       phi     -- a new value of the oscillator current phase.
 * @details The allowed values of \p phi are the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16] with
 *  resolution of 1/2^16. This range is scaled to the floating point range [0; 2*pi) with resolution of pi/2^15 radian:
 *  | phase, radian  | fixed point value  | container code |
 *  |----------------|--------------------|----------------|
 *  | 2*pi           | not allowed (*)    | not allowed    |
 *  | 2*pi - pi/2^15 | 0.9999847412109375 | 0xFFFF         |
 *  | 3*pi/2         | 0.75               | 0xC000         |
 *  | pi             | 0.5                | 0x8000         |
 *  | pi/2           | 0.25               | 0x4000         |
 *  | pi/2^15        | 0.0000152587890625 | 0x0001         |
 *  | 0              | 0.0                | 0x0000         |
 * @note    Values of the phase outside the domain range [0; 2*pi) radians shall be projected into the domain with the
 *  formula phi = phase-2*pi*k, where k is an integer.
 */
extern void gen_set_phi(struct gen_descr_t * const pgen, const uq016_t phi);

/**@brief   Assigns the generator output attenuation.
 * @param[in,out]   pgen    -- pointer to a generator descriptor object.
 * @param[in]       att     -- a new value of the output signal momentary attenuation.
 * @details The allowed values of \p att are the set of UQ0.16 values in the discrete range [0.0; 1.0-1/2^16] with
 *  resolution of 1/2^16. This range corresponds to the floating point range [0; 1) with resolution of 1/2^16:
 *  | attenuation | fixed point value  | container code |
 *  |-------------|--------------------|----------------|
 *  | 1           | not allowed (*)    | not allowed    |
 *  | 1 - 1/2^16  | 0.9999847412109375 | 0xFFFF         |
 *  | 0.75        | 0.75               | 0xC000         |
 *  | 0.5         | 0.5                | 0x8000         |
 *  | 1/2^16      | 0.0000152587890625 | 0x0001         |
 *  | 0           | 0.0                | 0x0000         |
 * @note    The attenuation factor value 1 exactly, which cannot be represented as a UQ0.16 value, shall be substituted
 *  with 1-1/2^16. Indeed, as soon as the returned value of the function is of the type SQ0.15, which has smaller
 *  resolution 1/2^15 than the \p att, the output signal will be constantly 0 when \p att is specified as 1-1/2^16 as
 *  well as it would be specified as 1 exactly.
 */
extern void gen_set_att(struct gen_descr_t * const pgen, const uq016_t att);

/**@brief   Returns the generator momentary output.
 * @param[in]   pgen    -- pointer to a generator descriptor object.
 * @return  Momentary amplitude of the generated signal.
 * @details The output signal produced by the generator is described with the formula:
 *  - u(t) = sin(2*pi*Fo/Fs*t + phi0) * (1-att), where:
 *  - Fo    -- the oscillator frequency, hertz.
 *  - Fs    -- the sampling frequency, hertz.
 *  - phi0  -- the oscillator initial phase, radians.
 *  - att   -- the momentary attenuation.
 *  - t     -- the current moment of time, in terms of sampling frequency periods.
 * @details This function shall be called each time the applications needs to evaluate the current level of the
 *  generator output signal. Generally this function is called each sampling period before the generator state is
 *  propagated with the \c gen_step function, or the generator attributes are reassigned with functions \c gen_set_xxx.
 * @details The codomain of the generator output signal levels is the set of SQ0.15 values in the discrete range
 *  [-1.0; +1.0-1/2^15] with resolution of 1/2^15. This range corresponds to the floating point range [-1; +1) with
 *  resolution of 1/2^15:
 *  | amplitude   | fixed point value  | container code |
 *  |-------------|--------------------|----------------|
 *  | +1          | not reachable (*)  | not reachable  |
 *  | +1 - 1/2^15 | +0.999969482421875 | 0x7FFF         |
 *  | +0.75       | +0.75              | 0x6000         |
 *  | +0.5        | +0.5               | 0x4000         |
 *  | +1/2^15     | +0.000030517578125 | 0x0001         |
 *  |  0          |  0.0               | 0x0000         |
 *  | -1/2^15     | -0.000030517578125 | 0x7FFF         |
 *  | -0.5        | -0.5               | 0xC000         |
 *  | -1 + 1/2^15 | -0.999969482421875 | 0x8001         |
 *  | -1          | -1.0               | 0x8000         |
 * @note    The momentary amplitude value +1 exactly, which cannot be represented as a SQ0.15 value, is substituted with
 *  1-1/2^15.
 */
extern sq015_t gen_output(const struct gen_descr_t * const pgen);

/**@brief   Propagates the generator state for one sampling step.
 * @param[in,out]   pgen    -- pointer to a generator descriptor object.
 * @details Each time this function is called the current phase of the oscillator is incremented by 2*pi*Fo/Fs radians.
 * @note    For the generator to work properly this function shall be called exactly one time per each sampling period.
 */
extern void gen_step(struct gen_descr_t * const pgen);
/**@}*/

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* SINEGEN_H */
