
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void ascii_to_byte(char *buf, uint8_t *byte)
{
    *byte = 0;
    char *ptr = buf;
    int pos =0;
    while(*ptr){
        pos = 7 - (ptr - buf);
        *byte |= (((*ptr++) - '0') << pos);
    }
    printf("byte = 0x%x\n", *byte);
}

void ascii_to_bytes(char *buf, uint8_t *bytes, int size)
{
    int len = strlen(buf) >> 3;
    if(len > size) {
        printf("bytes size is too small, abort\n");
        return;
    }
    char *bufp = buf;
    uint8_t *bytesp = bytes;

    for(int i=0;i<len;i++){
        ascii_to_byte(bufp, bytesp);
        bufp += 8;
        bytesp += 8;
    }
}

void test_ascii_to_byte()
{
    char buf[] = "10101011";
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
    test_ascii_to_bytes();
}