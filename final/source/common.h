#ifndef __COMMON_H__
#define __COMMON_H__

#include "huffman.h"

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

#define HUFF_PRINT 						huffman_print

/**
 * @brief delay in milliseconds
 * @param milliseconds: the target time interval for delay
 */
void delay_ms(int milliseconds);


#endif
