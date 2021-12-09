#ifndef _TONE_H_
#define _TONE_H_

#include <stdint.h>

//define the tone buffer length
#define TONE_BUFFER_LENGTH 		(1024)

/**
 * @brief Update tone buffer based on the tone symbol,
 * which corresponds to one supported tone frequency.
 */

void fill_in_tone_buffer(uint32_t tone_freq,
		uint16_t *buff);

/**
 * @brief display the statistical information of the
 * generate samples.
 */
void display_sampling_stats();

/**
 * @brief obtain the total samples for the
 * current tone frequency.
 */
int get_tone_sample_amount();

#endif

