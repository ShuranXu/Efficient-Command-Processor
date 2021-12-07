#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

int encode_message(const char *s, char *buf, int buf_size);

void decode_message(const char *s, char *buf);

#endif /* __HUFFMAN_CODE_H__ */