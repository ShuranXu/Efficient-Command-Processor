/**
 * @file    final.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
//#include "fsl_debug_console.h"
#include <stdint.h>
#include "uart.h"
#include "cmd_proc.h"
#include <stdio.h>
#include "huffman.h"
#include "fp_sin.h"
#include "dsp.h"
#include "tpm.h"
#include "dma.h"
#include "common.h"
#include "timer.h"
#include "tsi.h"

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

	/* initialize Timer */
	systick_init();
	/* initialize the UART0 */
	uart0_init();
	/* initialize TSI */
	TSI_init();
	/* Calibrate TSI */
	TSI_calibrate();
	/* initialize DAC */
	DAC_init();
	/* Calibrate ADC */
	ADC_calibrate();
	/* Configure ADC0 for DMA */
	ADC0_configure_for_DMA();
	/* initialize DMA0 */
	DMA0_init();
	/* enable TPM0 and TPM1 */
	TPM0_init();
	TPM1_init();
	//run the command processor
	command_processor();

    return 0 ;
}
