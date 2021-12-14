#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ascii_binary_conv.h"

//input: LSB   MSB
//from   0101000
//output: MSB    LSB
//to      01010000 --> 0x50


/**
 * @brief Convert ASCII codes to one byte.
 * @param buf: the buffer holding ASCII codes
 * @param bufsize: the length of the buffer that
 * holds the ASCII codes.
 * @param byte: a pointer to the variable that holds
 * the converted byte value.
 */
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

/**
 * @brief Convert ASCII codes to bytes.
 * @param ascii_str: the buffer holding ASCII codes
 * @param ascii_str_size: the length of the buffer that
 * holds the ASCII codes.
 * @param bytes: the array that holds the converted byte value.
 * @param bytes_size: the capacity of the bytes array.
 */
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

/**
 * @brief Convert one byte to ASCII codes
 * @param buf: the buffer holding ASCII codes
 * @param byte: the variable that holds the byte
 * value to be converted.
 */
void byte_to_ascii(char *buf, uint8_t byte)
{
    int i;
    for(i=0;i<8;i++){
        int val = (byte >> (7 - i)) & 0x1;
        buf[i] = val + '0';
    }
}

/**
 * @brief Convert bytes to ASCII codes
 * @param ascii_str: the buffer holding ASCII codes
 * @param ascii_str_size: the capacity of the buffer that
 * holds the ASCII codes.
 * @param bytes: the array that holds the byte value to be converted.
 * @param bytes_size: the length of the bytes array in use.
 */
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
