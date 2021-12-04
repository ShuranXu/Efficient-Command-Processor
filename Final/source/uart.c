/*
 * @file uart.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - implements the init funciton, the ISR as well as of the UART
 * peripheral. In addition, two system calls are overwritten to redirect
 * the data to UART.
 * @date 2021-11-07
 * @Reference: the UART0 interrupt handler implementation is based upon
 * the example code provided on the lecture 20.
 */

#include "uart.h"
#include "cbfifo.h"
#include "MKL25Z4.h"
#include "sysclock.h"


/* The macros ease one's work to update the field
 * and get field values.MODIFY_FIELD() macro basically
 * clears out the target field first and then set
 * the field with value.
 *
 * CLEAR_FIELD() clears out the target field;GET_FIELD()
 * returns the target field value.
 */
#define MODIFY_FIELD(reg, field, value) ((reg) = ((reg) & ~(field##_MASK)) | \
(((value) << field##_SHIFT) & field##_MASK))
#define CLEAR_FIELD(reg, field) ((reg) = (reg) & ~(field##_MASK))
#define GET_FIELD(reg, field) ((reg & field##_MASK) >> field##_SHIFT)
#define BAUD_RATE 						(38400)
#define OVERSAMPLE_RATE 				(16)
#define STOP_BITS 						(2)


/**
 * @brief initialize UART0 to have the following properties:
 *  - baud rate 38400
 *  - 8 bit data
 *  - No parity bits
 *  - 2 stop bits
 *  - Rx interrupt enabled
 */
void uart0_init()
{
	uint16_t SBR;

	/* Enable clock gating for UART0 and Port A */
	MODIFY_FIELD(SIM->SCGC4, SIM_SCGC4_UART0, 1);
	MODIFY_FIELD(SIM->SCGC5, SIM_SCGC5_PORTA, 1);

	/* Disable transmitter and receiver before init */
	UART0->C2 &= (~UART0_C2_TE_MASK) & (~UART0_C2_RE_MASK);

	/* Set UART clock to 48 MHz clock */
	MODIFY_FIELD(SIM->SOPT2, SIM_SOPT2_UART0SRC, 1);

	/* Set pins to UART0 Rx */
	MODIFY_FIELD(PORTA->PCR[1], PORT_PCR_ISF, 1);
	MODIFY_FIELD(PORTA->PCR[1], PORT_PCR_MUX, 2);

	/* Set pins to UART0 Tx */
	MODIFY_FIELD(PORTA->PCR[2], PORT_PCR_ISF, 1);
	MODIFY_FIELD(PORTA->PCR[2], PORT_PCR_MUX, 2);

	/* Set baud rate */
	SBR = (uint16_t)((SYSCLOCK_FREQUENCY)/(BAUD_RATE * OVERSAMPLE_RATE));
	CLEAR_FIELD(UART0->BDH, UART0_BDH_SBR);
	MODIFY_FIELD(UART0->BDH, UART0_BDH_SBR, (SBR >> 8));
	MODIFY_FIELD(UART0->BDL, UART0_BDL_SBR, SBR);

	/* Set the over-sampling ratio */
	MODIFY_FIELD(UART0->C4, UART0_C4_OSR, OVERSAMPLE_RATE - 1);

	/* Disable interrupts for LIN Break detect
	 * and RX Input Active Edge,  */
	CLEAR_FIELD(UART0->BDH, UART0_BDH_RXEDGIE);
	CLEAR_FIELD(UART0->BDH, UART0_BDH_LBKDIE);

	/* Disable loopback mode */
	CLEAR_FIELD(UART0->C1, UART0_C1_LOOPS);
	/* Use 8 data bit mode */
	CLEAR_FIELD(UART0->C1, UART0_C1_M);
	/* Don't use parity */
	CLEAR_FIELD(UART0->C1, UART0_C1_PE);
	/* Use 2-bit stop bit */
	MODIFY_FIELD(UART0->BDH, UART0_BDH_SBNS, (STOP_BITS >> 1));
	/* Don't invert transmit data */
	CLEAR_FIELD(UART0->C3, UART0_C3_TXINV);
	/* Enable Overrun interrupt */
	MODIFY_FIELD(UART0->C3, UART0_C3_ORIE, 1);
	/* Enable Noise Error interrupt */
	MODIFY_FIELD(UART0->C3, UART0_C3_NEIE, 1);
	/* Enable Framing Error interrupt */
	MODIFY_FIELD(UART0->C3, UART0_C3_FEIE, 1);
	/* Enable Parity Error interrupt */
	MODIFY_FIELD(UART0->C3, UART0_C3_PEIE, 1);
	/* Clear all error flags */
	CLEAR_FIELD(UART0->S1, UART0_S1_OR);
	CLEAR_FIELD(UART0->S1, UART0_S1_NF);
	CLEAR_FIELD(UART0->S1, UART0_S1_FE);
	CLEAR_FIELD(UART0->S1, UART0_S1_PF);
	/* Enable receive interrupts */
	MODIFY_FIELD(UART0->C2, UART_C2_RIE, 1);
	/* Enable UART receiver */
	MODIFY_FIELD(UART0->C2, UART0_C2_RE, 1);
	/* Enable UART transmitter */
	MODIFY_FIELD(UART0->C2, UART0_C2_TE, 1);
	/* Clear the UART RDRF flag */
	uint8_t temp = UART0->D;
	MODIFY_FIELD(UART0->S1, UART0_S1_TDRE, 1);

	/* Configure the corresponding NVIC attributes */
	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}

/**
 * @brief handles various error cases in a dummy manner; sent out
 * the data byte to UART0 by dequeuing data from the TX circular buffer;
 * enqueuing received data byte from UART0.
 */
void UART0_IRQHandler(void)
{
	uint8_t byte;
	const uint8_t error_flags = UART_S1_OR_MASK | UART_S1_NF_MASK |
	UART_S1_FE_MASK | UART_S1_PF_MASK;

	if (UART0->S1 & error_flags) {
		// We're in error. We should probably increment an error counter. For now
		// clear the flags, and also read the data register to clear RDRF

		UART0->S1 |= error_flags;
		byte = UART0->D;
	}

	if (UART0->S1 & UART0_S1_RDRF_MASK) {    // byte ready to be read; enqueue it
		byte = UART0->D;
		// If there was no room on the FIFO, this byte just got silently
		// dropped.
		cbfifo_enqueue(RX, &byte, 1);
	}

	if ((UART0->C2 & UART0_C2_TIE_MASK) &&       // transmitter interrupt enabled
	  (UART0->S1 & UART0_S1_TDRE_MASK)) {      // tx buffer empty

		if (cbfifo_dequeue(TX, &byte, 1)) {     // a byte is waiting to tx
		  UART0->D = byte;
		} else {                // FIFO was empty, so disable transmitter interrupt

		  CLEAR_FIELD(UART0->C2, UART0_C2_TIE);
		}
	}
}

/**
 * @brief override __sys_write() to redirect data to UART0 via interrupt,
 * Data is extracted from the TX circular buffer with interrupt disabled
 * to prevent the race condition.
 */
int __sys_write(int handle, char *buf, int size)
{
	if(!buf || size < 0)
		return -1;

	/* disable interrupts prior to entering the critical section */
	uint32_t masking_state = __get_PRIMASK();

	for(int i=0;i<size;i++){
		cbfifo_enqueue(TX, (void *)buf + i, 1);
		MODIFY_FIELD(UART0->C2, UART0_C2_TIE, 1);
	}
	/* enable interrupts */
	__set_PRIMASK(masking_state);

	return 0;
}


/**
 * @brief override __sys_readc() to redirect data received from UART0
 * to the RX circular buffer with interrupt disabled to prevent
 * the race condition.
 */
int __sys_readc(void)
{
	if(!cbfifo_length(RX))
		return -1;

	uint8_t buf;
	/* disable interrupts prior to entering the critical section */
	uint32_t masking_state = __get_PRIMASK();
	__disable_irq();
	cbfifo_dequeue(RX, (void *) &buf, 1);
	/* enable interrupts */
	__set_PRIMASK(masking_state);
	return buf;
}

