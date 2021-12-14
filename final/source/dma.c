#include <tone.h>
#include <string.h>
#include "dma.h"
#include "fp_sin.h"
#include "MKL25Z4.h"
#include "common.h"
#include "handlers.h"

/* Reference global variables defined in tpm.c */
static uint16_t tone_buffer[TONE_BUFFER_LENGTH];
static int sample_amount = 0;
static uint32_t DMA_cycles;
static int dma_action = 0;
static uint32_t DMA_counter = 0;

/**
 * @brief Initialize DMA0 to enable interrupt and
 * TPM0 triggering.
 */
void DMA0_init()
{
	dma_action = 0;

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

/**
 * @brief Configure DMA0 based on teh samples and cycles required.
 * @param src: array holding samples
 * @param sample_mnt: the total sample amount
 * @param dma_cycles: the number of DMA cycles required
 */
void Configure_DMA_Playback(uint16_t *src, int sample_mnt, uint32_t dma_cycles)
{
	DMA_cycles = dma_cycles;
	dma_action = 1;
	memcpy(tone_buffer, src, sample_mnt);
	sample_amount = sample_mnt;

	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) tone_buffer);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));

	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(sample_amount);

	// enable a source without periodic triggering
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_SOURCE(54);

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
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(sample_amount);
	// set enable flag
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}

/**
 * @brief testing if the DMA is performing the
 * transactions.
 * @return int: return 1 if running; return 0 if not.
 */
int is_DMA_running()
{
	return dma_action;
}

/**
 * @brief Overwrite the default handler and reconfigure DMA
 * engine for the next DMA request issued by TPM0
 */
void DMA0_IRQHandler(void)
{
	/* clear all bits */
	MODIFY_FIELD(DMA0->DMA[0].DSR_BCR, DMA_DSR_BCR_DONE, 1);

	if(dma_action){
		DMA_counter++;
		if(DMA_counter > DMA_cycles){
			DMA_counter = 0;
			dma_action = 0;
			// disable a source without periodic triggering
			DMAMUX0->CHCFG[0] &= ~DMAMUX_CHCFG_SOURCE(54);
			//disable the enable flag
			DMAMUX0->CHCFG[0] &= ~DMAMUX_CHCFG_ENBL_MASK;
			return;
		}
		/* reconfigure DMA engine */
		Start_DMA_Playback();
	}
}

