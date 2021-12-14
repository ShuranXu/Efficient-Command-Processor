/*
 * @file uart.h
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - header for the UART implementation.
 * @date 2021-12-13
 */

#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

/**
 * @brief initialize UART0 to have the following properties:
 *  - baud rate 38400
 *  - 8 bit data
 *  - No parity bits
 *  - 2 stop bits
 *  - Rx interrupt enabled
 */
void uart0_init();

#endif
