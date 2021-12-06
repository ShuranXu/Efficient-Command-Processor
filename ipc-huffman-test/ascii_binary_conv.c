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
            // printf("src[%d] = %c\n",7-i,src[i]);
        }

        for(i=0;i<bufsize;i++){ 
            src[7 - i] = buf[i];
            // printf("src[%d] = %c\n", i,src[i]);
        }
    }
    else{
        int i;
        for(i=0;i<8;i++){
            src[7 - i] = buf[i];
            // printf("src[%d] = %c\n",i,src[i]);
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
    printf("ascii_to_byte(): byte = 0x%x\n", *byte);
}


int ascii_to_bytes(char *buf, int buf_size, uint8_t *bytes, int bytes_size)
{
    uint8_t *bytesp = bytes;
    char *bufp = buf;
    int residue = buf_size;

    while(residue >= 8){
        ascii_to_byte(bufp, 8, bytesp);
        printf("data[%d] = 0x%x\n", bytesp - bytes, *bytesp);
        residue -= 8;
        bufp += 8;
        bytesp += 1;
    }
    if(residue > 0){
        ascii_to_byte(bufp,residue, bytesp);
        printf("data[%d] = 0x%x\n", bytesp - bytes, *bytesp);
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

    printf("byte_to_ascii(): buf = %s\n", buf);
}

void bytes_to_ascii(char *buf, int buf_size, uint8_t *bytes, int byte_size)
{
    int len = (byte_size << 2);
    if(len > buf_size){
        printf("Error: bytes size = %d and buf size = %d, abort\n", byte_size, buf_size);
        return;
    }

    for(int i=0;i<byte_size;i++){
        byte_to_ascii(&buf[i << 3], *bytes++);
    }
}


// #define TEST

#ifdef TEST

void test_byte_to_ascii()
{
    char buf[8];
    // uint8_t byte = 0xab;
    uint8_t byte = 0x28;
    printf("byte to be encoded = 0x%x\n", byte);
    byte_to_ascii(buf, byte);
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
}

/* The LSM bit is the rightmost character and the MSB bit 
 * is the leftmost bit.
 */ 
void test_ascii_to_byte()
{
    //0xab
    char buf[] = "10101011";
    // char buf[] = "0101000";
    uint8_t byte;
    ascii_to_byte(buf, &byte);
}

void test_ascii_to_bytes()
{
    //0xab0f8844
    char buf[] = "10101011000011111000100001000100";
    uint8_t bytes[4];
    ascii_to_bytes(buf, bytes, 4);
}

int main(int argc, char *argv[])
{
    test_byte_to_ascii();
}

#endif