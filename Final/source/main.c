/*
 * @file main.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - The application entry point.
 * @date 2021-11-07
 */

#include "sysclock.h"
#include <stdint.h>
#include "uart.h"
#include "cmd_proc.h"
#include "cbfifo_test.h"
#include <stdio.h>
#include "huffman.h"


int main(void)
{
	sysclock_init();
	//initialize the UART here
	uart0_init();
	//test cbfifo
//	test_cbfifo();
//	printf("\n\rWelcome to BreakfastSerial!\r\n");
	//run the command processor
	command_processor();
	return 0 ;
}
