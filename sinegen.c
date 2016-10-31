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
/**@cond false*/
static void gen_pp_restart(struct gen_descr_t * const pgen);
static void gen_pp_lookahead(struct gen_descr_t * const pgen);
static ui16_t sqrt_ui16(const ui16_t x);
/**@endcond*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Initializes a sine wave generator. */
void gen_init(struct gen_descr_t * const pgen) {

    assert(pgen != NULL);

    pgen->freq = 0;
    pgen->phi = 0;
    pgen->att = 0;
    pgen->en = 0;

    gen_pp_restart(pgen);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator frequency. */
void gen_set_freq(struct gen_descr_t * const pgen, const uq016_t freq) {

    assert(pgen != NULL);
    assert(freq <= 0x4000);

    pgen->freq = freq;

    gen_pp_restart(pgen);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator phase. */
void gen_set_phi(struct gen_descr_t * const pgen, const uq016_t phi) {

    assert(pgen != NULL);

    pgen->phi = phi;

    gen_pp_restart(pgen);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Assigns the generator output attenuation. */
void gen_set_att(struct gen_descr_t * const pgen, const uq016_t att) {

    assert(pgen != NULL);

    pgen->att = att;

    gen_pp_restart(pgen);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Enables or disables the postprocessing on the generator output. */
void gen_set_pp(struct gen_descr_t * const pgen, const bool_t en) {

    assert(pgen != NULL);

    pgen->en = en;

    gen_pp_restart(pgen);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Returns the generator momentary output. */
sq015_t gen_output(const struct gen_descr_t * const pgen) {

    ui16_t  midx;       /* Modified index of the current sample within the interval excluding the additional step. */
    ui16_t  istep;      /* Index of the current main step. Valid only if sidx is inside one of main steps. */
    ui16_t  iidx;       /* Relative index of the current sample within the current main step. */
    ui16_t  pidx;       /* Relative index of the current sample within the pattern of the current main step. */

    assert(pgen != NULL);

    if (pgen->pp == 0) {
        return msin_sq015(pgen->phi, pgen->att);
    }

    if (pgen->sidx >= pgen->aidx && pgen->sidx < pgen->ridx) {
        return (pgen->sidx - pgen->aidx) & 1 ? pgen->val0 : pgen->val1;
    }

    midx = pgen->sidx;
    if (pgen->sidx >= pgen->ridx) {
        midx -= pgen->asize;
    }
    istep = midx / pgen->msize;     /* 'istep' gives also the number of 'val1' in the pattern of the current step. */
    iidx = midx % pgen->msize;
    pidx = iidx % pgen->steps;      /* 'steps' stays also for the length of the pattern in each main step. */

    return pidx >= istep ? pgen->val0 : pgen->val1;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Propagates the generator state for one sampling step. */
void gen_step(struct gen_descr_t * const pgen) {

    assert(pgen != NULL);

    if (pgen->freq == 0) {
        return;
    }

    pgen->phi += pgen->freq;
    ++(pgen->sidx);

    if (pgen->pp && pgen->sidx == pgen->sampl) {
        pgen->phi0 = pgen->phi1;
        pgen->val0 = pgen->val1;
        pgen->pp = 0;
    }
    if (pgen->pp == 0) {
        gen_pp_lookahead(pgen);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/**@cond false*/
void gen_pp_restart(struct gen_descr_t * const pgen) {

    assert(pgen != NULL);

    pgen->phi0 = pgen->phi;
    pgen->val0 = msin_sq015(pgen->phi0, pgen->att);
    pgen->pp = 0;

    if (pgen->freq > 0) {
        gen_pp_lookahead(pgen);
    }
}
/**@endcond*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@cond false*/
void gen_pp_lookahead(struct gen_descr_t * const pgen) {

    ui16_t  cnt1, cnt2;     /* Count samples to left- and right-ends of the postprocessing interval. */
    sq015_t dval;           /* Difference between val1 and val0. Valid only if both values are defined. */

    assert(pgen != NULL);
    assert(pgen->freq > 0 && pgen->freq <= 0x4000);
    assert(pgen->pp == 0);

    if (pgen->en == 0) {
        return;
    }

    pgen->phi1 = pgen->phi0;
    cnt1 = 0;
    while (1) {     /* TODO: Use binary search, or use arcsin to find phi1. */
        pgen->phi1 += pgen->freq;
        ++cnt1;
        if (pgen->phi1 - pgen->phi0 >= 0x4000 || cnt1 >= 0x4000) {
            return;
        }
        pgen->val1 = msin_sq015(pgen->phi1, pgen->att);
        if (pgen->val1 != pgen->val0) {
            break;
        }
    }

    dval = pgen->val1 - pgen->val0;
    if (dval < -1 || dval > 1) {
        return;
    }

    pgen->phi2 = pgen->phi1;
    cnt2 = 0;
    while (1) {     /* TODO: Use binary search, or use arcsin to find phi1. */
        pgen->phi2 += pgen->freq;
        ++cnt2;
        if (pgen->phi2 - pgen->phi1 >= 0x4000 || cnt2 >= 0x4000) {
            return;
        }
        pgen->val2 = msin_sq015(pgen->phi2, pgen->att);
        if (pgen->val2 != pgen->val1) {
            break;
        }
    }

    pgen->sampl = cnt1 + cnt2 / 2;
    pgen->steps = sqrt_ui16(pgen->sampl);
    if (pgen->steps >= 2) {
        pgen->pp = 1;
        pgen->phi1 += cnt2 / 2 * pgen->freq;
        pgen->msize = pgen->sampl / pgen->steps;
        pgen->asize = pgen->sampl % pgen->steps;
        pgen->sidx = 0;
        pgen->ridx = pgen->sampl - (pgen->steps / 2) * pgen->msize;
        pgen->aidx = pgen->ridx - pgen->asize;
    }
}
/**@endcond*/

/*--------------------------------------------------------------------------------------------------------------------*/
/**@cond false*/
ui16_t sqrt_ui16(const ui16_t x) {

    static const ui16_t sqr_lut[] = {
        0/*0*/,       1/*1*/,       4/*2*/,       9/*3*/,       16/*4*/,      25/*5*/,      36/*6*/,      49/*7*/,
        64/*8*/,      81/*9*/,      100/*10*/,    121/*11*/,    144/*12*/,    169/*13*/,    196/*14*/,    225/*15*/,
        256/*16*/,    289/*17*/,    324/*18*/,    361/*19*/,    400/*20*/,    441/*21*/,    484/*22*/,    529/*23*/,
        576/*24*/,    625/*25*/,    676/*26*/,    729/*27*/,    784/*28*/,    841/*29*/,    900/*30*/,    961/*31*/,
        1024/*32*/,   1089/*33*/,   1156/*34*/,   1225/*35*/,   1296/*36*/,   1369/*37*/,   1444/*38*/,   1521/*39*/,
        1600/*40*/,   1681/*41*/,   1764/*42*/,   1849/*43*/,   1936/*44*/,   2025/*45*/,   2116/*46*/,   2209/*47*/,
        2304/*48*/,   2401/*49*/,   2500/*50*/,   2601/*51*/,   2704/*52*/,   2809/*53*/,   2916/*54*/,   3025/*55*/,
        3136/*56*/,   3249/*57*/,   3364/*58*/,   3481/*59*/,   3600/*60*/,   3721/*61*/,   3844/*62*/,   3969/*63*/,
        4096/*64*/,   4225/*65*/,   4356/*66*/,   4489/*67*/,   4624/*68*/,   4761/*69*/,   4900/*70*/,   5041/*71*/,
        5184/*72*/,   5329/*73*/,   5476/*74*/,   5625/*75*/,   5776/*76*/,   5929/*77*/,   6084/*78*/,   6241/*79*/,
        6400/*80*/,   6561/*81*/,   6724/*82*/,   6889/*83*/,   7056/*84*/,   7225/*85*/,   7396/*86*/,   7569/*87*/,
        7744/*88*/,   7921/*89*/,   8100/*90*/,   8281/*91*/,   8464/*92*/,   8649/*93*/,   8836/*94*/,   9025/*95*/,
        9216/*96*/,   9409/*97*/,   9604/*98*/,   9801/*99*/,   10000/*100*/, 10201/*101*/, 10404/*102*/, 10609/*103*/,
        10816/*104*/, 11025/*105*/, 11236/*106*/, 11449/*107*/, 11664/*108*/, 11881/*109*/, 12100/*110*/, 12321/*111*/,
        12544/*112*/, 12769/*113*/, 12996/*114*/, 13225/*115*/, 13456/*116*/, 13689/*117*/, 13924/*118*/, 14161/*119*/,
        14400/*120*/, 14641/*121*/, 14884/*122*/, 15129/*123*/, 15376/*124*/, 15625/*125*/, 15876/*126*/, 16129/*127*/,
    };

    ui16_t key;     /* Key into the square lookup table. */

    assert(x < 0x4000);

    for (key = 0; key < ARRAY_SIZE(sqr_lut); ++key) {   /* TODO: Use binary search, or implement direct evaluation. */
        if (sqr_lut[key] > x) {
            break;
        }
    }

    return key - 1;
}
/**@endcond*/

/*--------------------------------------------------------------------------------------------------------------------*/
