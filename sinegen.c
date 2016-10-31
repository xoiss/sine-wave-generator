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
void gen_init(struct gen_descr_t * const pdescr) {
    assert(pdescr != NULL);
    pdescr->freq = 0;
    pdescr->phi = 0;
    pdescr->att = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator frequency. */
void gen_set_freq(struct gen_descr_t * const pdescr, const uq016_t freq) {
    assert(pdescr != NULL);
    assert(freq <= 0x4000);
    pdescr->freq = freq;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator phase. */
void gen_set_phi(struct gen_descr_t * const pdescr, const uq016_t phi) {
    assert(pdescr != NULL);
    pdescr->phi = phi;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator output attenuation. */
void gen_set_att(struct gen_descr_t * const pdescr, const uq016_t att) {
    assert(pdescr != NULL);
    pdescr->att = att;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the generator momentary output. */
sq015_t gen_output(const struct gen_descr_t * const pdescr) {
    assert(pdescr != NULL);
    return msin_sq015(pdescr->phi, pdescr->att);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Propagates the generator state for one sampling step. */
void gen_step(struct gen_descr_t * const pdescr) {
    assert(pdescr != NULL);
    pdescr->phi += pdescr->freq;
}

/*--------------------------------------------------------------------------------------------------------------------*/
