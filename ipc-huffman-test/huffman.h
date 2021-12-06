#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

void import_huffman_table();

void encode_message(const char *s, char *out);

void decode_message(const char *s, char *buf);

#endif /* __HUFFMAN_CODE_H__ */