#ifndef __TPM_H__
#define __TPM_H__

/**
 *@brief initialize TPM0 to operate at 48 KHz
 */
void TPM0_init();

/**
 * @brief initialize TPM1 to operate at 96 KHz
 */
void TPM1_init();

void enable_TPMs();

void disable_TPMs();


#endif
