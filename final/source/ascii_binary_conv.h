#ifndef __ASCI_TO_BIN_H__
#define __ASCI_TO_BIN_H__

#include <stdint.h>

/**
 * @brief Convert ASCII codes to one byte.
 * @param buf: the buffer holding ASCII codes
 * @param bufsize: the length of the buffer that
 * holds the ASCII codes.
 * @param byte: a pointer to the variable that holds
 * the converted byte value.
 */
void ascii_to_byte(char *buf, int bufsize, uint8_t *byte);

/**
 * @brief Convert one byte to ASCII codes
 * @param buf: the buffer holding ASCII codes
 * @param byte: the variable that holds the byte
 * value to be converted.
 */
void byte_to_ascii(char *buf, uint8_t byte);

/**
 * @brief Convert ASCII codes to bytes.
 * @param ascii_str: the buffer holding ASCII codes
 * @param ascii_str_size: the length of the buffer that
 * holds the ASCII codes.
 * @param bytes: the array that holds the converted byte value.
 * @param bytes_size: the capacity of the bytes array.
 */
int ascii_to_bytes(const char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size);

/**
 * @brief Convert bytes to ASCII codes
 * @param ascii_str: the buffer holding ASCII codes
 * @param ascii_str_size: the capacity of the buffer that
 * holds the ASCII codes.
 * @param bytes: the array that holds the byte value to be converted.
 * @param bytes_size: the length of the bytes array in use.
 */
int bytes_to_ascii(char *ascii_str, int ascii_str_size, \
uint8_t *bytes, int bytes_size);

#endif /* __ASCI_TO_BIN_H__ */
