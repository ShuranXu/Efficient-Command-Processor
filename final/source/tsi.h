#ifndef __TSI_H_
#define __TSI_H_

#include <stdint.h>

/**
 * @brief calibrate TSI sensor
 */
void TSI_calibrate();

/**
 * @brief initialize TSI sensor
 */

void TSI_init();


/**
 * @brief read the position of the finger on the slider
 * uint8_t: the current position on the slider
 */
uint8_t read_slider_distance();

/**
 * @brief poll TSI slider ISR flag and clear it.
 * Besides, the TSI channel is updated to use the
 * another one.
 */
void poll_and_handle_tsi();


#endif

