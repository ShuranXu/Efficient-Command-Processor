#ifndef __ASCI_TO_BIN_H__
#define __ASCI_TO_BIN_H__

#include <stdint.h>

void ascii_to_byte(char *buf, int bufsize, uint8_t *byte);

void byte_to_ascii(char *buf, uint8_t byte);

int ascii_to_bytes(const char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size);

int bytes_to_ascii(char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size);

#endif /* __ASCI_TO_BIN_H__ */