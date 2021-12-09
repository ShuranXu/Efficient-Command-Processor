#include "common.h"
#include "board.h"

/**
 * @brief delay in milliseconds
 * @param milliseconds: the target time interval for delay
 */
void delay_ms(int milliseconds)
{
	 //number of instructions in 1 ms
	const uint32_t INST_NUMBERS_IN_MS = BOARD_BOOTCLOCKRUN_CORE_CLOCK/1000;
	//total number of instructions to be executed in 'milliseconds' msec
	uint32_t inst_nums_ms = milliseconds * INST_NUMBERS_IN_MS;
	//by experimentation, there are around 6 instructions for a for-loop C code
	const uint32_t LOOP_INST_NUMS = 6;
	//calculate the number of loops needed
	uint32_t loops = inst_nums_ms / LOOP_INST_NUMS;

	while(loops-- != 0)
	{
		__asm volatile("NOP");
	}
}
