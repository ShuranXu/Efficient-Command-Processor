#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "huffman_table.h"
#include "huffman.h"
#include "MKL25Z4.h"

#define COMMON_BUFF_SIZE        (32)

static int find_table_index_based_on_bitnum(int bitnum, int *buff)
{
    huffman_code_t *htp = HUFFMAN_TABLE;
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

void decode_message(const char *s, char *buf)
{
    char *p = s;
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

int encode_message(const char *s, char *buf, int buf_size)
{
    char *p = s;
    char *bufp = buf;
    huffman_code_t *htp = HUFFMAN_TABLE;

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
        htp = HUFFMAN_TABLE;
        p++;
    }
    return 0;
}

void huffman_print(const char *str)
{
	uint8_t wbuf[512];
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
