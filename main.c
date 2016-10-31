/**@file
 * @brief   The test application.
 * @details The test application runs a couple of sine wave generator objects under the test for the specified number of
 *  oscillator frequency periods. Output signals of both the generators are saved into the CSV file. Generators have the
 *  same parameters except the postprocessing status: it is enabled only on the second generator.
 * @details The format of the output file is the following:
 *  - the file consists of three columns and a number of rows. Values in a row are separated with semicolon symbol.
 *  - the first column contains the momentary phase code. The code belongs to the integer range [0; 65535]. The phase
 *      value may be recovered from the code with the formula: phi = 2*pi * (code/65536.0) radian.
 *  - the second and the third columns contain the momentary amplitude codes of two generators respectively. The code
 *      belongs to the integer range [-32768; +32767]. The amplitude value may be recovered from the code with the
 *      formula: u = (code/32768.0).
 *
 * @author  Alexander A. Strelets
 * @version 1.0
 * @date    October, 2016
 * @copyright   GNU Public License
 */

/*--------------------------------------------------------------------------------------------------------------------*/
#include "sinegen.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   The name of the file to save the generator output signal.
 */
#define FILE_NAME   "sine.csv"

/**@brief   The number of periods of Fo to produce.
 */
#define FO_CYCLES   1

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   Performs the sine wave generation and saves the data.
 * @param[in,out]   fo      -- file stream to save the generator output.
 * @param[in]       cycles  -- number of Fo cycles to perform.
 */
void run(FILE * const fo, const unsigned int cycles) {

    struct gen_descr_t  gen1;       /* Descriptor of the sine wave generator with disabled postprocessing. */
    struct gen_descr_t  gen2;       /* Descriptor of the sine wave generator with enabled postprocessing. */

    unsigned int cnt;       /* Counts periods of Fo. */

    assert(fo != NULL && cycles > 0);

    gen_init(&gen1);
    gen_set_freq(&gen1, 4);         /* Fs/Fo = 4/65536. For example, if Fs = 65536 Hz then Fo = 4 Hz. */
    gen_set_phi(&gen1, 0);          /* phi = 0. Indeed, it is the default value. */
    gen_set_att(&gen1, 65528);      /* att = 1 - 1/8192. Which means u(t) = sin(phi)*(1/8192). */
    gen_set_pp(&gen1, 0);           /* Postprocessing is disabled. */

    gen_init(&gen2);
    gen_set_freq(&gen2, gen1.freq);
    gen_set_phi(&gen2, gen1.phi);
    gen_set_att(&gen2, gen1.att);
    gen_set_pp(&gen2, 1);           /* Postprocessing is enabled. */

    cnt = 0;
    do {
        uq016_t phi;        /* Current phase. */
        phi = gen1.phi;
        fprintf(fo, "%u; %i; %i\n", phi, gen_output(&gen1), gen_output(&gen2));
        gen_step(&gen1);
        gen_step(&gen2);
        if (gen1.phi < phi) {
            ++cnt;
            fflush(fo);
        }
    } while (cnt < cycles);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/**@brief   The main application function.
 * @return  The status: 0 if finished successfully, non-zero if there was a failure.
 */
int main(void) {

    FILE * fo;      /* File stream to save the generator output. */

    fo = fopen(FILE_NAME, "wt");
    if (fo == NULL) {
        fprintf(stderr, "\nERROR: Failed to create file: %s\n\n", FILE_NAME);
        return EXIT_FAILURE;
    }

    run(fo, FO_CYCLES);

    fclose(fo);

    return EXIT_SUCCESS;
}

/*--------------------------------------------------------------------------------------------------------------------*/
