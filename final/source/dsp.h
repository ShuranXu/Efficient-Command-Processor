#ifndef __DSP_H__
#define __DSP_H__

#include <stdlib.h>

/**
 * @brief Configure ADC0 to sample DMA data
 */
void ADC0_configure_for_DMA();

/**
 *@brief Initialize DAC0
 */
void DAC_init();

/**
 * @brief Calibrate the ADC0.
 * @return return the calibration result. 0 if succeed
 * and -1 otherwise.
 */
int32_t ADC_calibrate();

/**
 * @brief calculate the fundamental period of the sampled
 * waveform and obtain the statistical information
 * regarding the collected samples.
 */
int audio_analysis();

/**
 * @brief Poll ADC0 for sampled data and store
 * the sampled data into the sample buffer
 */
int ADC0_poll();



#endif

