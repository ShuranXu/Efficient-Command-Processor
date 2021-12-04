/**
 * @file cbfifo.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief This file implements all public functions
 * declared in cbfifo.h
 * @version 0.1
 * @date 2021-11-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "cbfifo.h"
#include <stdio.h>
#include <string.h>


/**
 * @brief definitions of typedefs needed
 * for the program.
 */
typedef enum {
    NONE=0,
    READ,
    WRITE
}last_move_t;

typedef enum
{
    EMPTY = 0,
    DIRTY,
    FULL
}cbfifo_stats;


typedef struct
{
    uint8_t fifo[CBFIFO_CAPACITY];
    int write_idx;
    int read_idx;
last_move_t recent_turnover_type;
}cbfifo_t;


static cbfifo_t cbfifos[TOTAL_FIFOS];

/**
 * @brief Declaration of local helper
 * functions.
 */
static cbfifo_stats cbfifo_status(int fifo_index);

/**
 * @brief return the current status of cbfifo.
 * It determines the status by checking write_idx,
 * read_idx and recent_turnover_type values.
 *
 * The distinction between full and empty is to
 * check if an enqueue operation has led to a
 * turnover of the write index. When write_idx
 * matches read_idx and a recent write turnover
 * happened then the fifo is guaranteed to be
 * full.
 *
 * @param fifo_index the index of the cbfifo array
 * @return cbfifo_stats
 */
static cbfifo_stats cbfifo_status(int fifo_index)
{
    cbfifo_stats ret;
    if(cbfifos[fifo_index].write_idx != cbfifos[fifo_index].read_idx)
        ret = DIRTY;
    else if(cbfifos[fifo_index].recent_turnover_type == WRITE)
        ret = FULL;
    else
        ret = EMPTY;

    return ret;
}

/**
 * @brief Enqueues data onto the FIFO, up to the limit
 * of the available FIFO capacity.
 *
 * @param fifo_index the index of the cbfifo array
 * @param buf Pointer to the data
 * @param nbyte Max number of bytes to enqueue
 * @return size_t The number of bytes actually enqueued,
 * which could be 0. In case of an error, returns -1.
 */
size_t cbfifo_enqueue(int fifo_index, void *buf, size_t nbyte)
{
    if(!buf)
        return -1;

    int length;
    int iter = 0;
    cbfifo_stats stats = cbfifo_status(fifo_index);
    switch(stats) {
        case FULL:
            return 0;
        case DIRTY:
            length = CBFIFO_CAPACITY - cbfifo_length(fifo_index);
            length = (length > nbyte) ? nbyte : length;
            break;
        case EMPTY:
            length = (nbyte > CBFIFO_CAPACITY) ? CBFIFO_CAPACITY : nbyte;
            break;
    }

    /**
     * @brief the for-loop starts with i= 0
     * indicates that the enqueue operation
     * writes data to the current slot and
     * then move forward to the next slot.
     */
    for(int i=0;i<length;i++){
        iter = (cbfifos[fifo_index].write_idx + i) & (CBFIFO_CAPACITY - 1);
        *(cbfifos[fifo_index].fifo + iter) = *((uint8_t *)(buf + i));
    }

    /* mark recent_turnover_type to WRITE if exceed the fifo length */
    if(cbfifos[fifo_index].write_idx + length >= CBFIFO_CAPACITY)
    	cbfifos[fifo_index].recent_turnover_type = WRITE;
    /* advance write_idx to the next writable slot */
    cbfifos[fifo_index].write_idx = (cbfifos[fifo_index].write_idx + length) & (CBFIFO_CAPACITY - 1);

    return length;
}

/**
 * @brief Attempts to remove ("dequeue") up to nbyte bytes
 * of data from the FIFO. Removed data will be copied into
 * the buffer pointed to by buf.
 *
 * @param fifo_index the index of the cbfifo array
 * @param buf Destination for the dequeued data
 * @param nbyte Bytes of data requested
 * @return size_t  The number of bytes actually copied,
 * which will be between 0 and nbyte. Returns -1 on error.
 *
 * To further explain the behavior: If the FIFO's current
 * length is 24 bytes, and the caller requests 30 bytes,
 * cbfifo_dequeue should return the 24 bytes it has, and
 * the new FIFO length will be 0. If the FIFO is empty
 * (current length is 0 bytes), a request to dequeue
 * any number of bytes will result in a return of 0 from
 * cbfifo_dequeue.
 */

/* first read and then move */
size_t cbfifo_dequeue(int fifo_index, void *buf, size_t nbyte)
{
    if(!buf)
        return -1;

    cbfifo_stats stats = cbfifo_status(fifo_index);
    int length;
    int iter = 0;
    switch(stats) {
        case EMPTY:
            return 0;
        case DIRTY:
            length = cbfifo_length(fifo_index);
            length = (length > nbyte) ? nbyte : length;
            break;
        case FULL:
            length = (nbyte > CBFIFO_CAPACITY) ? CBFIFO_CAPACITY : nbyte;
            break;
    }

    /**
     * @brief the for-loop starts with i= 0
     * indicates that the dequeue operation
     * reads data at the current slot and
     * then move forward to the next slot.
     */
    for(int i=0;i<length;i++){
        iter = (cbfifos[fifo_index].read_idx + i) & (CBFIFO_CAPACITY - 1);
        *((uint8_t *)(buf + i)) = *(cbfifos[fifo_index].fifo + iter);
        /* clear the slot */
        *(cbfifos[fifo_index].fifo + iter) = 0;
    }

    /* mark recent_turnover_type to READ if exceed the fifo length */
    if(cbfifos[fifo_index].read_idx + length >= CBFIFO_CAPACITY)
    	cbfifos[fifo_index].recent_turnover_type = READ;
    /* advance write_idx to the next readable slot */
    cbfifos[fifo_index].read_idx = (cbfifos[fifo_index].read_idx + length) & (CBFIFO_CAPACITY - 1);

    return length;
}

/**
 * @brief Returns the number of bytes currently
 * on the FIFO.
 *
 * @param fifo_index the index of the cbfifo array
 * @return size_t Number of bytes currently available
 * to be dequeued from the FIFO
 */

size_t cbfifo_length(int fifo_index)
{
    if(cbfifos[fifo_index].recent_turnover_type == WRITE)
        return cbfifos[fifo_index].write_idx + CBFIFO_CAPACITY - cbfifos[fifo_index].read_idx;
    else
        return(cbfifos[fifo_index].write_idx - cbfifos[fifo_index].read_idx) & (CBFIFO_CAPACITY - 1);
}


/**
 * @brief Returns the FIFO's capacity
 *
 * @return size_t The capacity, in bytes,
 * for the FIFO
 */
size_t cbfifo_capacity()
{
    return CBFIFO_CAPACITY;
}
