#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

#define QUESTION_MARK_BYTE      		(0xe4)
#define PERIOD_MARK_BYTE        		(0x84)

/**
 * @brief encode the message using the huffman table.
 * @param s: the message to be encoded.
 * @param buf: the buffer used to store the encoded characters.
 * @param buf_size: the capacity of buf.
 * @return int: return 0 if success;return the number of bits
 * that buf at least needs if failure.
 */
int encode_message(const char *s, char *buf, int buf_size);

/**
 * @brief decode the encoded message using the huffman table.
 * @param s: the message to be encoded.
 * @param buf: the buffer used to store the encoded characters.
 */
void decode_message(const char *s, char *buf);


/**
 * @brief print the message with the huffman encoding and ASCII-Byte
 * conversion to maxmimize the system throughput.
 * @param str: the message to be printed.
 */
void huffman_print(const char *str);

#endif /* __HUFFMAN_CODE_H__ */
