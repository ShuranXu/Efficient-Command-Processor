#include "tone.h"
#include "fp_sin.h"
#include "common.h"
#include <string.h>

#define TPM0_SAMPLE_FREQ		(48000)

/**
 * Local struct to encapsulate information
 * required for statistical printout.
 */
typedef struct {
	uint32_t tone_freq;
	uint32_t samples_per_cycle;
	uint32_t cycles;
	uint32_t total_samples;
}sampling_stats_t;


/* local global variable for holding statistics
 * of generated samples.
 */
static sampling_stats_t sampling_stats = {0,0,0,0};


/*
 * @brief Fill out the input buffer with samples of
 * the specified tone frequency.
 */
static void tone_to_samples(uint32_t tone_freq, uint32_t sampling_freq,
		uint16_t *buff)
{
	uint16_t *buffptr = buff;
	sampling_stats.tone_freq = tone_freq;
	sampling_stats.samples_per_cycle = sampling_freq / tone_freq;
	sampling_stats.cycles = TONE_BUFFER_LENGTH / sampling_stats.samples_per_cycle;
	sampling_stats.total_samples = sampling_stats.samples_per_cycle * \
			sampling_stats.cycles;

	for(int i=0;i < sampling_stats.total_samples; i++) {
		*buffptr++ = fp_sin((i * TWO_PI) / sampling_stats.samples_per_cycle) + \
				TRIG_SCALE_FACTOR;
	}

//	uint8_t msg[64];
//	memset(msg,0,sizeof(msg));
//	sprintf(msg, "%d samples generated at %d Hz,computed period = %d samples\r\n", \
//			sampling_stats.total_samples, tone_freq, sampling_stats.samples_per_cycle);
//	HUFF_PRINT(msg);
}

/**
 * @brief Update tone buffer based on the tone symbol,
 * which corresponds to one supported tone frequency.
 */
void fill_in_tone_buffer(uint32_t tone_freq,
		uint16_t *buff)
{
	tone_to_samples(tone_freq, TPM0_SAMPLE_FREQ, buff);
	return;
}

/**
 * @brief obtain the total samples for the
 * current tone frequency.
 */
int get_tone_sample_amount()
{
	return sampling_stats.total_samples;
}

