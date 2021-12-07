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
        *byte |= (((*ptr++) - '0') << pos);
    }
}


int ascii_to_bytes(char *buf, int buf_size, uint8_t *bytes, int bytes_size)
{
    uint8_t *bytesp = bytes;
    char *bufp = buf;
    int residue = buf_size;

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
        buf[i] = val + '0';
    }
}

void bytes_to_ascii(char *buf, int buf_size, uint8_t *bytes, int byte_size)
{
    int len = (byte_size << 2);
    if(len > buf_size){
        return;
    }

    for(int i=0;i<byte_size;i++){
        byte_to_ascii(&buf[i << 3], *bytes++);
    }
}
