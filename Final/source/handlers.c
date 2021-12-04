/*
 * @file handlers.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - implements each registered command handler function.
 * @date 2021-11-07
 */

#include "handlers.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* Define a macro calculting modulo using bitwise operations */
#define MODULO(num, base)       (num & (base - 1))

/**
 * @brief local helper function to convert int number
 * to hexdecimal string.
 *
 * The main difference from int unsigned_to_hexstr
 * (char *str,
 * size_t size, uint32_t num, uint8_t nbits) is that
 * the function has the flexibility to selectively
 * add '0x' prefix to str, based on the parameter
 * 'prefix'. Addtionally, no boundary check is needed
 * as this is a local function with the clear usage.
 *
 * @param str char array used to be manipulated
 * @param size the size of the parameter str
 * @param num the number to be converted
 * @param prefix flag to determine if '0x' prefix is needed.
 */

static void unsigned_to_hexstr(char *str, size_t size,
uint8_t num, int prefix)
{
    int i = size - 1;
    int j = 0;
    int tmp = 0;

    while(i >= 0) {

        /**
         * Mask the lower 4 bits of the right-shifted
         * num to get an intermediate value 'tmp'.
         * Based on the value of tmp, either 'A' or '0'
         * is used as the base for the ASCII number
         * transformation.
         *
         * j is incremented by 4 each time in order to
         * mask 4 bits for each while loop iteration.
         */
        tmp = ((num >> j) & 0xf);
        str[i--] = tmp < 10 ? (tmp + '0') : (tmp - 10 + 'A');
        j += 4;
    }

    if(prefix)
    	str[4] = '_';
}

/**
 * @brief prints a string representing a 'dump' of
 * the nbytes of memory starting at loc. Bytes are
 * printed up to 16 bytes per line, separated by newlines.
 *
 * @param argc the total number of arguments combined with the function name
 * @param argv the array of arguments, which include source address and the length
 */
void hex_dump(int argc,char *argv[])
{
	uint8_t loc = (uint8_t)strtol(argv[1], NULL, 16);
	int nbytes;

	if((argv[2][0] == '0') && (argv[2][1] == 'x'))
		sscanf(argv[2], "%x", &nbytes);
	else
		sscanf(argv[2], "%d", &nbytes);

	if(nbytes < 0 || nbytes > 640){
		printf("\r\nError: %d is invalid length, length should be in [0,640]", nbytes);
		return;
	}

	char str[64];

    /* determine the numbers of lines needed */
    int lines = nbytes >> 4;
    lines += (MODULO(nbytes,4) > 0) ? 1 : 0;

    /**
     * @brief Calculate the total bytes needed from str
     * based on nbytes parameter:
     *
     * Regarding the column number, for each line we need:
     * 1 + 6 = 7,
     * where 1 byte is for newline character and 6
     * bytes are for the hex number representing the
     * column number.
     *
     * For each byte we need 3 bytes from str:
     * 1 byte is for space character and 2 bytes are for
     * hex representation of a ASCII value.
     *
     * 1 byte for the '\0' character for str
     */

    int loc_iter = 0;
    int str_iter = 0;
    uint8_t ascii_value;

    while(loc_iter < nbytes) {

        if(!MODULO(loc_iter, 16)) {
            /* setup the first column for the current line */
            unsigned_to_hexstr(str, 9, loc + loc_iter, 1);
            str_iter += 9;
            str[str_iter++] = ' '; //ASCII space
        }

        str[str_iter++] = ' '; //ASCII space
        ascii_value = (uint8_t)(*((char *)loc + loc_iter));
        unsigned_to_hexstr(str+str_iter, 2, ascii_value, 0);
        str_iter += 2;

        if(!MODULO((loc_iter + 1), 16)) {
            printf("\r\n%s",str);
            memset(str,0,sizeof(str));
            str_iter = 0;
        }
        loc_iter++;
    }
    return;
}

/**
 * @brief print out the author name.
 */
void handle_author()
{
	printf("\r\nShuran Xu");
}


/**
 * @brief Print out the current build information
 * of the executable.
 */
void handle_info()
{
	printf("\r\n Version= ");
	printf(VERSION_TAG);
	printf("\r\n Build= ");
	printf(BUILD_INFO);
	printf("\r\n Machine = ");
	printf(BUILD_MACHINE);
	printf("\r\n Date = ");
	printf(BUILD_DATE);
}

/**
 * @brief Print messages to help users to use the command processor.
 */
void handle_help()
{
	printf("\r\n\r\nBreakfastSerial");
	printf("\r\nUsage: command [arg1] [arg2]");
	printf("\r\n Author       : Print the author name");
	printf("\r\n Dump         : Print a hexdump of the memory requested");
	printf("\r\n Info         : Print the build information of the program");
	printf("\r\n Help         : Print the help message");
}
