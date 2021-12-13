/*
 * @file handlers.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - implements each registered command handler function.
 * @date 2021-11-07
 */

#include "handlers.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "huffman.h"
#include "timer.h"
#include "tpm.h"
#include "tone.h"
#include "common.h"
#include "dsp.h"
#include "tsi.h"


#define MAX_TONE_FREQ 			(1000)
#define DEFAULT_DURATION 		(1)
#define DEFAULT_FREQ 			(400)
/* Define a macro calculting modulo using bitwise operations */
#define MODULO(num, base)       (num & (base - 1))


static uint16_t tone_buff[TONE_BUFFER_LENGTH];



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
		return;
	}

	char str[40];
	memset(str,0,sizeof(str));

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

        if(!MODULO(loc_iter, 8)) {
            /* setup the first column for the current line */
            unsigned_to_hexstr(str, 9, loc + loc_iter, 1);
            str_iter += 9;
            str[str_iter++] = ' '; //ASCII space
        }

        str[str_iter++] = ' '; //ASCII space
        ascii_value = (uint8_t)(*((char *)loc + loc_iter));
        unsigned_to_hexstr(str+str_iter, 2, ascii_value, 0);
        str_iter += 2;

        if(!MODULO((loc_iter + 1), 8)) {
        	HUFF_PRINT("\r\n");
        	HUFF_PRINT(str);
            memset(str,0,sizeof(str));
            str_iter = 0;
        }
        loc_iter++;
    }
    HUFF_PRINT("END");
    return;
}

/**
 * @brief print out the author name.
 */
void handle_author()
{
	HUFF_PRINT("\r\nShuran Xu");
	HUFF_PRINT("END");
}

/**
 * @brief Print messages to help users to use the command processor.
 */
void handle_help()
{
	HUFF_PRINT("\r\nUsage           Command [arg1] [arg2]");
	HUFF_PRINT("\r\nAuthor          Print the author name");
	HUFF_PRINT("\r\nDump addr len   Print hexdump of the requested memory ");
	HUFF_PRINT("\r\nHelp            Display all commands");
	HUFF_PRINT("\r\nTone freq time  Play tone at the given frequency");
	HUFF_PRINT("\r\nTSI time        Play tone at frequency by TSI sliding");
	HUFF_PRINT("END");
}

static void play_tone(int freq, int duration)
{
	//update the tone buffer based on the frequency
	fill_in_tone_buffer(freq, tone_buff);
	//configure DMA buffer
	Configure_DMA_Playback(tone_buff,get_tone_sample_amount(), freq * duration);

	while(1){
		if(ADC0_poll() < 0){
			break;
		}
		audio_analysis();
	}
}

void handle_tone(int argc,char *argv[])
{
	int freq;
	int duration;

	/* set the default frequency and duration if no inputs from the user */
	if(argc < 2){
		freq = DEFAULT_FREQ;
		duration = DEFAULT_DURATION;
	}
	else if(argc < 3){
		freq = atoi(argv[1]);
		duration = DEFAULT_DURATION;
	}
	else{
		freq = atoi(argv[1]);
		duration = atoi(argv[2]);
	}

	if(freq < 0 || duration < 0){
		HUFF_PRINT("Error both frequency and duration have to bigger than 0\r\n");
		return;
	}

	play_tone(freq, duration);
	HUFF_PRINT("END");
}


void handle_tsi(int argc,char *argv[])
{
	uint8_t slider_pos;
	int freq;
	int duration;
	uint32_t initial_timestamp = now();

	if(argc < 2){
		duration = DEFAULT_DURATION;
	}
	else{
		duration = atoi(argv[1]);
	}
	/* reset the timer */
	reset_timer();

	do{
		/* Poll the touch slider */
		poll_and_handle_tsi();
		/* get the latest slider distance for the touch */
		slider_pos = read_slider_distance();
		/* obtain the corresponding freq */
		freq = (slider_pos * MAX_TONE_FREQ) / SLIDER_LENGTH;
		/* play the tone at the associated frequency */
		if(freq){
			play_tone(freq, 1);
		}

	}while(get_elapsed_time(initial_timestamp) < (duration * ONE_SECOND_TICKS));

	HUFF_PRINT("END");
}

