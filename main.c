/**@file
 * @brief   The test application.
 * @details The test application runs the sine wave generator object under the test for the specified number of 
 *  oscillator frequency periods. The generator output signal is saved into the CSV file.
 * @details The format of the output file is the following:
 *  - the file consists of two columns and a number of rows. Values in a row are separated with semicolon symbol.
 *  - the first column contains the momentary phase code. The code belongs to the integer range [0; 65535]. The phase 
 *      value may be recovered from the code with the formula: phi = 2*pi * (code/65536.0) radian.
 *  - the second column contains the momentary amplitude code. The code belongs to the integer range [-32768; +32767]. 
 *      The amplitude value may be recovered from the code with the formula: u = (code/32768.0).
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
 * @param[in,out]   pgen    -- pointer to the generator descriptor object.
 * @param[in]       cycles  -- number of Fo cycles to perform.
 */
void run(FILE * const fo, struct gen_descr_t * const pgen, const unsigned int cycles) {

    unsigned int cnt;       /* Counts periods of Fo. */

    assert(fo != NULL && pgen != NULL);

    gen_init(pgen);
    gen_set_freq(pgen, 1);          /* Fs/Fo = 1/65536. For example, if Fs = 65536 Hz then Fo = 1 Hz. */
    gen_set_phi(pgen, 0);           /* phi = 0. Indeed, it is the default value. */
    gen_set_att(pgen, 65528);       /* att = 1 - 1/8192. Which means u(t) = sin(phi)*(1/8192). */

    cnt = 0;
    do {
        uq016_t phi;        /* Current phase. */
        phi = pgen->phi;
        fprintf(fo, "%u; %i\n", phi, gen_output(pgen));
        gen_step(pgen);
        if (pgen->phi < phi) {
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

    struct gen_descr_t  gen;        /* Sine wave generator descriptor. */

    fo = fopen(FILE_NAME, "wt");
    if (fo == NULL) {
        fprintf(stderr, "\nERROR: Failed to create file: %s\n\n", FILE_NAME);
        return EXIT_FAILURE;
    }

    run(fo, &gen, FO_CYCLES);
    fclose(fo);

    return EXIT_SUCCESS;
}

/*--------------------------------------------------------------------------------------------------------------------*/
