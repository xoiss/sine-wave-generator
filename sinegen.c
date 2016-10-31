/**@file
 * @brief   Implementation of the sine wave generator.
 * @details This file implements the set of functions used to manipulate the sine wave generator.
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

/*--------------------------------------------------------------------------------------------------------------------*/
#include "sinegen.h"
#include "fixtrig.h"
#include <assert.h>
#include <stddef.h>

/*--------------------------------------------------------------------------------------------------------------------*/
/* Initializes a sine wave generator. */
void gen_init(struct gen_descr_t * const pgen) {
    assert(pgen != NULL);
    pgen->freq = 0;
    pgen->phi = 0;
    pgen->att = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator frequency. */
void gen_set_freq(struct gen_descr_t * const pgen, const uq016_t freq) {
    assert(pgen != NULL);
    assert(freq <= 0x4000);
    pgen->freq = freq;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator phase. */
void gen_set_phi(struct gen_descr_t * const pgen, const uq016_t phi) {
    assert(pgen != NULL);
    pgen->phi = phi;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator output attenuation. */
void gen_set_att(struct gen_descr_t * const pgen, const uq016_t att) {
    assert(pgen != NULL);
    pgen->att = att;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the generator momentary output. */
sq015_t gen_output(const struct gen_descr_t * const pgen) {
    assert(pgen != NULL);
    return msin_sq015(pgen->phi, pgen->att);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Propagates the generator state for one sampling step. */
void gen_step(struct gen_descr_t * const pgen) {
    assert(pgen != NULL);
    pgen->phi += pgen->freq;
}

/*--------------------------------------------------------------------------------------------------------------------*/
