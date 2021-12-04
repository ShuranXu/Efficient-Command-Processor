/*
 * @file cbfifo.h
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - a fixed-size FIFO implemented via a circular buffer
 * @date 2021-11-07
 */

#ifndef _CBFIFO_H_
#define _CBFIFO_H_

#include <stdlib.h>
#include <stdint.h>

/* Define the cbfifo capacity in .h file since the macro
 * value is also used in cbfifo_test.c
 */
#define CBFIFO_CAPACITY         		(512U)

/**
 * Define the enum that acts as an index to the 2-slot global
 * circular buffer array.
 */
enum {
	RX = 0,
	TX,
	TOTAL_FIFOS
};


/*
 * Enqueues data onto the FIFO, up to the limit of the available FIFO
 * capacity.
 *
 * Parameters:
 *   fifo_index  	The index of the static circular buffer array
 *   buf      Pointer to the data
 *   nbyte    Max number of bytes to enqueue
 *
 * Returns:
 *   The number of bytes actually enqueued, which could be 0. In case
 * of an error, returns -1.
 */
size_t cbfifo_enqueue(int fifo_index, void *buf, size_t nbyte);

/*
 * Attempts to remove ("dequeue") up to nbyte bytes of data from the
 * FIFO. Removed data will be copied into the buffer pointed to by buf.
 *
 * Parameters:
 *   fifo_index  	The index of the static circular buffer array
 *   buf      Destination for the dequeued data
 *   nbyte    Bytes of data requested
 *
 * Returns:
 *   The number of bytes actually copied, which will be between 0 and
 * nbyte.
 *
 * To further explain the behavior: If the FIFO's current length is 24
 * bytes, and the caller requests 30 bytes, cbfifo_dequeue should
 * return the 24 bytes it has, and the new FIFO length will be 0. If
 * the FIFO is empty (current length is 0 bytes), a request to dequeue
 * any number of bytes will result in a return of 0 from
 * cbfifo_dequeue.
 */
size_t cbfifo_dequeue(int fifo_index, void *buf, size_t nbyte);


/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   fifo_index  	The index of the static circular buffer array
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
size_t cbfifo_length(int fifo_index);


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
size_t cbfifo_capacity();



#endif // _CBFIFO_H_
