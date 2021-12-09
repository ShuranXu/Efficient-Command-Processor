#ifndef __TIMER_H_
#define __TIMER_H_

#include <stdint.h>
#include "common.h"

#define ONE_SECOND_TICKS 		(16)
typedef uint32_t ticktime_t;

/**
 * @brief: initialize the timing system
 */
void systick_init();

/**
 * @brief: returns the time since startup, in 16th of a second
 */
ticktime_t now();

/**
 * @brief: resets timer to 0; doesn't affect now() values
 */
void reset_timer();

/**
 * @brief: returns the ticks since the last call to reset_timer()
 */
ticktime_t get_timer();

/**
 * @brief obtain the elapsed time, this is to be used to check
 * if the predefined time period has been passed. In case where
 * timer_counter is less than the target ticks, the overflow
 * of timer_counter might happened and we need to compensate it.
 */
uint32_t get_elapsed_time(uint32_t ticks);

#endif
