/**
 * @file timer.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief This source file configures the systick
 * with interrupts and provides interfaces for
 * resetting the timer and retrieving time information.
 * @version 0.1
 * @date 2021-12-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "timer.h"
#include "common.h"
#include "MKL25Z4.h"

/**
 * @brief redefine the bit-manipulation macros to work
 * with on-chip SysTick.
 */
#define systick_MODIFY_FIELD(reg, field, value) ((reg) = ((reg) & ~(field##_Msk)) | \
(((value) << field##_Pos) & field##_Msk))
//configure the clock to be 3MHz so that the period is 1/16th second.
#define F_SYS_CLK (24000000L / 8)

/**
 * @brief define a global timer counter
 * for use in now(), reset_timer() and
 * get_timer().
 */
static uint32_t timer_counter = 0;

/**
 * @brief define a timer count reference
 * to calculate get_timer(). This variable
 * gets updated only in reset_timer().
 */
static uint32_t timer_count_ref = 0;

/**
 * @brief Custom interrupt handler for SysTick
 */
void SysTick_Handler(void)
{
	/* Clear COUNTFLAG */
	SysTick->CTRL &= ~(1 << SysTick_CTRL_COUNTFLAG_Pos);
	/* Update the global timer counter */
	timer_counter++;
}

/**
 * @brief: initialize the timing system
 */
void systick_init()
{
	/* Set the reload field to get 1/16 seconds */
	systick_MODIFY_FIELD(SysTick->LOAD, SysTick_LOAD_RELOAD, F_SYS_CLK - 1);
	/* Set interrupt priority */
	NVIC_SetPriority(SysTick_IRQn, 3);
	/* Clear COUNTFLAG by reading the field */
	SysTick->CTRL &= ~(1 << SysTick_CTRL_COUNTFLAG_Pos);
	/* Write to Current value register to clear to 0 */
	systick_MODIFY_FIELD(SysTick->VAL,SysTick_VAL_CURRENT, 0);
	/* Enable the interrupt and the timer */
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | \
					SysTick_CTRL_ENABLE_Msk | \
					SysTick_CTRL_CLKSOURCE_Msk;
}

/**
 * @brief: returns the time since startup, in
 * 16th of a second.
 */
ticktime_t now()
{
	return timer_counter;
}

/**
 * @brief: resets timer to 0; doesn't affect now() values
 */
void reset_timer()
{
	/* Reset the timer by writing to SYST_CVR */
	systick_MODIFY_FIELD(SysTick->VAL,SysTick_VAL_CURRENT, 0);
	/* Update timer_count_ref */
	timer_count_ref = timer_counter;
}

/**
 * @brief: returns the ticks since the last call
 * to reset_timer().
 */
ticktime_t get_timer()
{
	return timer_counter - timer_count_ref;
}

/**
 * @brief obtain the elapsed time, this is to be used to check
 * if the predefined time period has been passed. In case where
 * timer_counter is less than the target ticks, the overflow
 * of timer_counter might happened and we need to compensate it.
 */
uint32_t get_elapsed_time(uint32_t ticks)
{
	int64_t elapsed_time = timer_counter - ticks;
	if(elapsed_time)
		return elapsed_time;
	else
		return (UINT32_MAX + 1 - ticks) + timer_counter;
}
