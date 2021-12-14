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
	/* enable DMA */
	TPM0->SC |= TPM_SC_DMA_MASK;
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
	/* Provide clock to TPM1 to start */
	MODIFY_FIELD(TPM1->SC, TPM_SC_CMOD, 1);
}

