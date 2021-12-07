#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

#define QUESTION_MARK_BYTE      		(0xe4)
#define PERIOD_MARK_BYTE        		(0x84)
#define HUFF_PRINT 						huffman_print

int encode_message(const char *s, char *buf, int buf_size);

void decode_message(const char *s, char *buf);

void huffman_print(const char *str);

#endif /* __HUFFMAN_CODE_H__ */
