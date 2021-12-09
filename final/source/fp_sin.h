#ifndef __FP_SIN_H__
#define __FP_SIN_H__

#include <stdint.h>

#define TRIG_SCALE_FACTOR			(2037)
#define HALF_PI 					(3200) // pi * TRIG_SCALE_FACTOR / 2
#define PI 							(6399) // pi * TRIG_SCALE_FACTOR
#define TWO_PI 						(12799) // 2 * pi * TRIG_SCALE_FACTOR

/**
 * @brief Calculate the sin function of the given
 * frequency using integers only.
 * @param num: input frequency
 * @return sine result of int16_t type.
 */
int16_t fp_sin(int32_t num);

#endif


