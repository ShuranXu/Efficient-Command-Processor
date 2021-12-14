#include "tone.h"
#include "dsp.h"
#include "MKL25Z4.h"
#include "autocorrelate.h"
#include "common.h"
#include "huffman.h"
#include "timer.h"
#include "handlers.h"
#include "dma.h"
#include <string.h>
#include <stdio.h>

// local macro for fundamental period calculation
#define ADC_SAMPLE_FREQ				(96000)
/** local global variables used by polling function
 * and analysis function
 */
static uint16_t sample_buffer[TONE_BUFFER_LENGTH];
static uint32_t buffer_iter = 0;

/**
 * @brief Configure ADC0 to sample DMA data
 */
void ADC0_configure_for_DMA()
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

	// clear the calibration settings
	CLEAR_FIELD(ADC0->CFG1, ADC_CFG1_ADIV);
	CLEAR_FIELD(ADC0->SC3, ADC_SC3_AVGE);
	CLEAR_FIELD(ADC0->SC3, ADC_SC3_AVGS);

	// bus clock input
	CLEAR_FIELD(ADC0->CFG1, ADC_CFG1_ADICLK);
	// Low power configuration
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_ADLPC, 1);
	// long sample time
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_ADLSMP, 1);
	// 16 bit single-ended conversion
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_MODE, 3);
	// Hardware trigger select
	MODIFY_FIELD(ADC0->SC2, ADC_SC2_ADTRG, 1);
	// hardware trigger from TPM1
	MODIFY_FIELD(SIM->SOPT7, SIM_SOPT7_ADC0ALTTRGEN,1);
	// ADC0 trigger select to TPM1 overflow
	MODIFY_FIELD(SIM->SOPT7,SIM_SOPT7_ADC0TRGSEL, 9);
	// Set ADC0 pre-trigger select to pre-trigger A
	CLEAR_FIELD(SIM->SOPT7, SIM_SOPT7_ADC0PRETRGSEL);
	// Set channel 23 as the input channel for DAC loopback mode
	MODIFY_FIELD(ADC0->SC1[0], ADC_SC1_ADCH, 23);
}

/**
 *@brief Initialize DAC0
 */
void DAC_init(void) {

	// Enable clock to DAC
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;

	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;

	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}

/**
 * @brief Calibrate the ADC0.
 * @return return the calibration result. 0 if succeed
 * and -1 otherwise.
 */
int32_t ADC_calibrate()
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

	ADC0->CFG1 = 0;
	ADC0->SC2 = 0;
	ADC0->SC1[0] = 0;

	uint16_t calib = 0; // calibration variable
	//16bit single ended mode, bus_clk/2, clk divide by 8
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_MODE, 3);
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_ADICLK, 1);
	MODIFY_FIELD(ADC0->CFG1, ADC_CFG1_ADIV, 3);

	//HW avg enable, 32 sample avg, calibration enable
	MODIFY_FIELD(ADC0->SC3, ADC_SC3_AVGE, 1);
	MODIFY_FIELD(ADC0->SC3, ADC_SC3_AVGS, 3);
	MODIFY_FIELD(ADC0->SC3, ADC_SC3_CAL, 1);

	// Wait for calibration to end
	while(ADC0->SC3 & ADC_SC3_CAL_MASK);
	// Check for successful calibration
	if(ADC0->SC3 & ADC_SC3_CALF_MASK)
	{
		return -1;
	}
	// Add all plus-side calibration value registers' values
	calib += ADC0->CLPS + ADC0->CLP4 + ADC0->CLP3 + ADC0->CLP2 + ADC0->CLP1 + ADC0->CLP0;
	calib /= 2;
	calib |= 0x8000; 	// Set MSB
	// update PG register
	ADC0->PG = calib;
	// Add all minus-side calibration value registers' values
	calib = 0;
	calib += ADC0->CLMS + ADC0->CLM4 + ADC0->CLM3 + ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0;
	calib /= 2;
	calib |= 0x8000;	// Set MSB
	// update MG register
	ADC0->MG = calib;

	return 0;
}

/**
 * @brief Poll ADC0 for sampled data and store
 * the sampled data into the sample buffer
 */
int ADC0_poll()
{
	while(buffer_iter < TONE_BUFFER_LENGTH) {
		if(is_DMA_running()){
			while(! (ADC0->SC1[0] & ADC_SC1_COCO_MASK));
			sample_buffer[buffer_iter++] = ADC0->R[0];
		}
		else{
			return -1;
		}
	}
	return 0;
}


/**
 * @brief calculate the fundamental period of the sampled
 * waveform and obtain the statistical information
 * regarding the collected samples.
 */
int audio_analysis()
{
	/* Determine the fundamental period of a
	 * waveform using autocorrelation
	 */
	int fund_period = autocorrelate_detect_period((void *)sample_buffer,
			buffer_iter, kAC_16bps_unsigned);

	// reset buffer_iter
	memset(sample_buffer, 0, buffer_iter);
	buffer_iter = 0;

	if(fund_period < 0){
		return -1;
	}

	int fund_freq = ADC_SAMPLE_FREQ / fund_period;

	// reset buffer_iter
	memset(sample_buffer, 0, buffer_iter);
	buffer_iter = 0;

	/* print the result */
	char msg[40];
	memset(msg,0,sizeof(msg));
	sprintf(msg, "period=%d samples freq=%d Hz\r\n", \
			fund_period, fund_freq);

	HUFF_PRINT(msg);
	return 0;
}



