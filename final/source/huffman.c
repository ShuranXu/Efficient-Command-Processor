#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "huffman_table.h"
#include "ascii_binary_conv.h"
#include "huffman.h"
#include "MKL25Z4.h"

#define COMMON_BUFF_SIZE        (32)

/**
 * @brief local helpr function to find all indice of the
 * huffman table entries whose bitnum match the input bitnum.
 * @param bitnum: the number of bits in interest
 * @param buff: the array that holds the indice found
 * @return int: the total number of indice found.
 */
static int find_table_index_based_on_bitnum(int bitnum, int *buff)
{
    huffman_code_t *htp = (huffman_code_t *)HUFFMAN_TABLE;
    int *bufp = buff;
    int size = 0;
    while(htp->bits){
        if(htp->bits == bitnum){
            size++;
            *bufp = htp - HUFFMAN_TABLE;
            bufp++;
        }
        htp++;
    }
    return size;
}

/**
 * @brief decode the encoded message using the huffman table.
 * @param s: the message to be encoded.
 * @param buf: the buffer used to store the encoded characters.
 */
void decode_message(const char *s, char *buf)
{
    char *p = (char *)s;
    char code[COMMON_BUFF_SIZE];
    int indices[COMMON_BUFF_SIZE];
    char *bufp = buf;
    int n;
    int brk;

    while(p){
        for(int i = MIN_HUFFMAN_CODE_LEN; i < MAX_HUFFMAN_CODE_LEN; i++){
            memcpy(code, p, i);
            n = find_table_index_based_on_bitnum(i,indices);
            for(int j=0;j<n;j++){
                int idx = indices[j];
                int bits = HUFFMAN_TABLE[idx].bits;
                if(strncmp(code, HUFFMAN_TABLE[idx].code, bits) == 0){
                    *bufp = HUFFMAN_TABLE[idx].c;
                    bufp++;
                    p += bits;
                    brk = 1;
                    break;
                }
            }
            if(brk){
                break;
            }
        }
        if(brk){
            brk = 0;
        }
        else{
            return;
        }
    }
}

/**
 * @brief encode the message using the huffman table.
 * @param s: the message to be encoded.
 * @param buf: the buffer used to store the encoded characters.
 * @param buf_size: the capacity of buf.
 * @return int: return 0 if success;return the number of bits
 * that buf at least needs if failure.
 */
int encode_message(const char *s, char *buf, int buf_size)
{
	char *p = (char *)s;
    char *bufp = buf;
    huffman_code_t *htp = (huffman_code_t *)HUFFMAN_TABLE;

    while(*p){
        while(htp){
            if(htp->c == *p){
                int remain_len = buf_size - (bufp - buf);
                if(remain_len < htp->bits){
                    return (htp->bits - remain_len);
                }
                memcpy(bufp, htp->code, htp->bits);
                bufp += htp->bits;
                break;
            }
            htp++;
        }
        //reset htp
        htp = (huffman_code_t *) HUFFMAN_TABLE;
        p++;
    }
    return 0;
}

/**
 * @brief print the message with the huffman encoding and ASCII-Byte
 * conversion to maxmimize the system throughput.
 * @param str: the message to be printed.
 */
void huffman_print(const char *str)
{
	char wbuf[512];
	uint8_t data[64];
    int data_written;
    int res;

    memset(wbuf,0,sizeof(wbuf));
    memset(data,0,sizeof(data));

	/* encode the command using the huffman coding */
    res = encode_message((const char *)str, wbuf, sizeof(wbuf));
	if(res < 0){
		return;
	}

	/* convert the ascii code to raw bytes */
	data_written = ascii_to_bytes(wbuf, strlen(wbuf), \
	data, sizeof(data));

	// append PERIOD_MARK_BYTE to data
	data[++data_written] = PERIOD_MARK_BYTE;
	//increment data_written
	data_written++;
	//send data
	for(int i=0;i<data_written;i++){
		putchar(data[i]);
	}
}
