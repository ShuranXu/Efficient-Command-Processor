#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ascii_binary_conv.h"

//input: LSB   MSB
//from   0101000
//output: MSB    LSB
//to      01010000 --> 0x50

// 11010001

void ascii_to_byte(char *buf, int bufsize, uint8_t *byte)
{
    char src[8];
    if(bufsize < 8){
        int residue = 8 - bufsize;
        int i;
        for(i=0;i<residue;i++){
            src[i] = '0';
        }

        for(i=0;i<bufsize;i++){ 
            src[7 - i] = buf[i];
        }
    }
    else{
        int i;
        for(i=0;i<8;i++){
            src[7 - i] = buf[i];
        }
    }

    *byte = 0;
    char *ptr = src;
    int pos =0;
    while(*ptr){
        pos = ptr - src;
        // printf("%d\n", *ptr - '0');
        *byte |= (((*ptr++) - '0') << pos);
    }
}


int ascii_to_bytes(const char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size)
{
    uint8_t *bytesp = bytes;
    char *bufp = (char *)ascii_str;
    int residue = ascii_str_size;

    while(residue >= 8){
        ascii_to_byte(bufp, 8, bytesp);
        residue -= 8;
        bufp += 8;
        bytesp += 1;
    }
    if(residue > 0){
        ascii_to_byte(bufp,residue, bytesp);
    }

    return bytesp - bytes;
}

void byte_to_ascii(char *buf, uint8_t byte)
{
    int i;
    for(i=0;i<8;i++){
        int val = (byte >> (7 - i)) & 0x1;
        // printf("%d\n", val);
        buf[i] = val + '0';
    }
}

int bytes_to_ascii(char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size)
{
    int len = (bytes_size << 2);
    if(len > ascii_str_size){
        return -1;
    }

    for(int i=0;i<bytes_size;i++){
        byte_to_ascii(&ascii_str[i << 3], *bytes++);
    }

    return 0;
}


// #define ASCII_BINARY_CONV_TEST

#ifdef ASCII_BINARY_CONV_TEST

void test_byte_to_ascii()
{
    char buf[8];
    uint8_t byte = 0x28;
    printf("byte to be encoded = 0x%x\n", byte);
    byte_to_ascii(buf, byte);
    printf("Encoded bytes become %s\n", buf);
}

void test_bytes_to_ascii()
{
    uint8_t bytes[4] = {0x11, 0x22, 0x33, 0x44};
    printf("bytes to be decoded:\n");
    for(int i=0;i<4;i++){
        printf("0x%x\t", bytes[i]);
    }
    printf("\r\n");

    char buf[33];
    memset(buf,0,33);
    bytes_to_ascii(buf, 33, bytes, 4);
    printf("Encoded bytes become %s\n", buf);
}

/* The LSM bit is the rightmost character and the MSB bit 
 * is the leftmost bit.
 */ 
void test_ascii_to_byte()
{
    //0xab
    char buf[] = "10101011";
    uint8_t byte;
    ascii_to_byte(buf, 1, &byte);
}

void test_ascii_to_bytes()
{
    //0xab0f8844
    char buf[] = "10101011000011111000100001000100";
    uint8_t bytes[4];
    ascii_to_bytes(buf, strlen(buf), bytes, sizeof(bytes));
    for(int i=0;i<4;i++){
        printf("0x%x\t", bytes[i]);
    }
    printf("\r\n");
}

int main(int argc, char *argv[])
{
    test_ascii_to_bytes();
}

#endif