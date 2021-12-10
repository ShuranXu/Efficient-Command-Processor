#include "tpm.h"
#include "common.h"
#include "tone.h"
#include "MKL25Z4.h"
#include "handlers.h"

// define MOD value for TPM0 and TPM1
#define TPM0_PERIOD 			(125)
#define TPM1_PERIOD 			(125)
#define ONE_SEC_THRESHOLD    	(48000)

/**
 *@brief initialize TPM0 to operate at 48 KHz
 */
void TPM0_init()
{
	/* Enable TPM0 clock */
	MODIFY_FIELD(SIM->SCGC6, SIM_SCGC6_TPM0, 1);
	/* Configure the TPM0 clock source to 24 MHz using PLLFLL */
	MODIFY_FIELD(SIM->SOPT2, SIM_SOPT2_TPMSRC, 1);
	/* MCGPLLCLK clock with fixed divide by two */
	MODIFY_FIELD(SIM->SOPT2, SIM_SOPT2_PLLFLLSEL, 1);
	/* initialize CNT to 0 */
	TPM0->CNT = 0;
	/* Set the modulo register */
	MODIFY_FIELD(TPM0->MOD, TPM_MOD_MOD, TPM0_PERIOD - 1);
	/* Set TPM count direction to up with a divide by 4 prescaler */
	MODIFY_FIELD(TPM0->SC, TPM_SC_PS, 2);
	/* Configure the up-counting mode */
	CLEAR_FIELD(TPM0->SC, TPM_SC_CPWMS);
	/* Provide clock to TPM0 to start */
	MODIFY_FIELD(TPM0->SC, TPM_SC_CMOD, 1);
	/* Enable overflow interrupt */
//	TPM0->SC |= TPM_SC_DMA_MASK | TPM_SC_TOIE_MASK;
	TPM0->SC |= TPM_SC_DMA_MASK;

	// Enable interrupts in NVIC
//	NVIC_SetPriority(TPM0_IRQn, 3);
//	NVIC_ClearPendingIRQ(TPM0_IRQn);
//	NVIC_EnableIRQ(TPM0_IRQn);
}

/**
 * @brief initialize TPM1 to operate at 96 KHz
 */
void TPM1_init()
{
	/* Enable TPM1 clock */
	MODIFY_FIELD(SIM->SCGC6, SIM_SCGC6_TPM1, 1);
	/* Configure the TPM0 clock source to 24 MHz using PLLFLL */
	MODIFY_FIELD(SIM->SOPT2, SIM_SOPT2_TPMSRC, 1);
	/* MCGPLLCLK clock with fixed divide by two */
	MODIFY_FIELD(SIM->SOPT2, SIM_SOPT2_PLLFLLSEL, 1);
	/* initialize CNT to 0 */
	TPM1->CNT = 0;
	/* Set the modulo register */
	MODIFY_FIELD(TPM1->MOD, TPM_MOD_MOD, TPM1_PERIOD - 1);
	/* Set TPM count direction to up with a divide by 2 prescaler */
	MODIFY_FIELD(TPM1->SC, TPM_SC_PS, 1);
	/* Configure the up-counting mode */
	CLEAR_FIELD(TPM1->SC, TPM_SC_CPWMS);

	/* Enable overflow interrupt */
//	TPM1->SC |= TPM_SC_TOIE_MASK;
	/* Provide clock to TPM1 to start */
	MODIFY_FIELD(TPM1->SC, TPM_SC_CMOD, 1);

	// Enable interrupts in NVIC
//	NVIC_SetPriority(TPM1_IRQn, 3);
//	NVIC_ClearPendingIRQ(TPM1_IRQn);
//	NVIC_EnableIRQ(TPM1_IRQn);
}

/**
 * @brief Overwrite the default handler and update the
 * tone buffer and the tone state per second.
 */
//void TPM0_IRQHandler() {
//
//	TPM0->SC |= TPM_SC_TOIE_MASK; // reset overflow flag
//	TPM0->STATUS |= TPM_STATUS_CH0F_MASK; //clear CH0F bit
//}
//
///**
// * @brief Overwrite the default handler.
// */
//void TPM1_IRQHandler() {
//
//	TPM1->SC |= TPM_SC_TOIE_MASK; // reset overflow flag
//	TPM1->STATUS |= TPM_STATUS_CH1F_MASK; //clear CH1F bit
//}
