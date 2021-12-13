#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "huffman_table.h"

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

int encode_message(const char *s, char *buf, int buf_size)
{
    char *p = (char *)s;
    char *bufp = buf;
    huffman_code_t *htp = HUFFMAN_TABLE;

    while(*p){
        while(htp){
            if(htp->c == *p){
                int remain_len = buf_size - (bufp - buf);
                if(remain_len < htp->bits){
                    printf("Error: buff size exceeded, at least %d more bytes needed\n", htp->bits);
                    return -1;
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

// #define HUFFMAN_TEST

#ifdef HUFFMAN_TEST

void test1()
{
	printf("Test1:\n");
    const char *str = "\r\nUsage: command [arg1] [arg2]";
    // const char *str = "dump 0xa0 0x64";
    char buff[256];
    char decodestr[32];
	memset(decodestr,0,sizeof(decodestr));
	memset(buff,0,sizeof(buff));

    if(encode_message(str,buff, sizeof(buff)) < 0){
        return -1;
    }
    printf("original str: %s\n", str);
    printf("encoded str: %s - %d bytes\n",buff, strlen(buff));
    // decode_message(buff, strlen(buff),decodestr);
    decode_message(buff, decodestr);
    printf("decoded str : %s\n", decodestr);
}

void test4()
{
	printf("Test4:\n");
    const char *str = "dump 0xa0 0x40";
	printf("str length = %d\n", strlen(str));
    char buff[256];
    char decodestr[32];
	memset(decodestr,0,sizeof(decodestr));
	memset(buff,0,sizeof(buff));

    if(encode_message(str,buff, sizeof(buff)) < 0){
        return -1;
    }
    printf("original str: %s\n", str);
    printf("encoded str: %s - %d bytes\n",buff, strlen(buff));
    // decode_message(buff, strlen(buff),decodestr);
    decode_message(buff, decodestr);
    printf("decoded str : %s\n", decodestr);
}

void test2()
{
	printf("Test2:\n");
    const char *rbuf = "011100001001010111000000";
    uint8_t decodestr[64];
    printf("rbuf len = %d\n", strlen(rbuf));
	memset(decodestr,0,sizeof(decodestr));
    // decode_message(rbuf, strlen(rbuf), decodestr);
    decode_message(rbuf, decodestr);
    printf("decoded str : %s\n", decodestr);

}

void test3()
{
	printf("Test3:\n");
    const char *rbuf = "011110100101111010110011011000101011110001111100010101011001101011010000100111100001101110011101001110011001001001010110011110011010110101011111011001100010111000000010110110010010100100011010101100111011110011011000110110100101001110111101";
    printf("rbuf leng = %d\n",strlen(rbuf));
	uint8_t decodestr[256];
	memset(decodestr,0,sizeof(decodestr));
    decode_message(rbuf, decodestr);
    printf("\n");
	printf("decodestr leng = %d\n",strlen(decodestr));
    printf("decoded str : %s\n", decodestr);

}


int main(void)
{
	test3();

	return 0;
}

#endif 



