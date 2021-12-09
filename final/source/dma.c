#include <tone.h>
#include "dma.h"
#include "fp_sin.h"
#include "MKL25Z4.h"
#include "common.h"
#include "handlers.h"

/* Reference global variables defined in tpm.c */
static uint16_t tone_buffer[TONE_BUFFER_LENGTH];
static int sample_amount = 0;


/**
 * @brief Initialize DMA0 to enable interrupt and
 * TPM0 triggering.
 */
void DMA0_init()
{
	// Gate clocks to DMA and DMAMUX
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	// Disable DMA channel to allow configuration
	DMAMUX0->CHCFG[0] = 0;
	DMA0->DMA[0].DCR = 0;

	// Generate DMA interrupt when done
	// Increment source
	// Enable peripheral request
	// Set Cycle steal mode

	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_CS, 1);
	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_SINC, 1);
	// transfer words (16 bits)
	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_SSIZE, 2);
	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_DSIZE, 2);

	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_EINT, 1);
	MODIFY_FIELD(DMA0->DMA[0].DCR, DMA_DCR_ERQ, 1);

	// Enable DMA MUX channel with TPM0 overflow as trigger
	// Here we enable a source without periodic triggering
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_SOURCE(54);

	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);
}

void Configure_DMA_Playback(uint16_t *src, int len)
{
	memcpy(tone_buffer, src, len);
	sample_amount = len;

	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) tone_buffer);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));

	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(sample_amount * 2);
	// set enable flag
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}

/**
 * @brief Configure DMA0 registers for the new DMA request
 */
static void Start_DMA_Playback()
{
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) tone_buffer);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));

	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(sample_amount * 2);
	// set enable flag
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}


/**
 * @brief Overwrite the default handler and reconfigure DMA
 * engine for the next DMA request issued by TPM0
 */

void DMA0_IRQHandler(void)
{
	/* clear all bits */
	MODIFY_FIELD(DMA0->DMA[0].DSR_BCR, DMA_DSR_BCR_DONE, 1);

	/* reconfigure DMA engine */
	Start_DMA_Playback();
}

