#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "huffman.h"
#include "ascii_binary_conv.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void test_huffman_encode_and_decode()
{
    const char *srcstr = "dump 0xa0 0x40";
    char buff[128];
    char decodestr[32];
	memset(decodestr,0,sizeof(decodestr));
	memset(buff,0,sizeof(buff));
    CU_ASSERT_EQUAL(encode_message(srcstr,buff, sizeof(buff)), 0);
    decode_message(buff, decodestr);
    CU_ASSERT_STRING_EQUAL(decodestr, srcstr);
}

void test_huffman_decode()
{
    const char *rbuf = "0111000010010101110";
    uint8_t decodestr[128];
    const char *result = "test";
	memset(decodestr,0,sizeof(decodestr));
    decode_message(rbuf, decodestr);
    CU_ASSERT_STRING_EQUAL(decodestr, result);
}

void test_huffman_encode()
{
    const char *srcstr = "\r\nUsage: command [arg1] [arg2]";
    char buff[256];
    char decodestr[32];
    const char *result = "0010001001100001011000000010100111001001000111111101100010101001111001111000111111101000110100001100011101101001001110011010000010100001100011101101001001111000100000";
	memset(decodestr,0,sizeof(decodestr));
	memset(buff,0,sizeof(buff));

    CU_ASSERT_EQUAL(encode_message(srcstr,buff, sizeof(buff)), 0);
    CU_ASSERT_STRING_EQUAL(result, buff);
}

void test_ascii_to_byte()
{
    uint8_t byte;
    uint8_t result = 0xe4;
    const char *ascii_str = "111001";
    ascii_to_byte((uint8_t *)ascii_str, strlen(ascii_str), &byte);  
    CU_ASSERT_EQUAL(byte, result);
}

void test_ascii_to_bytes()
{
    uint8_t bytes[4];
    uint8_t result[] = {0xab, 0xf, 0x88, 0x44};
    const char *ascii_str = "10101011000011111000100001000100";
    ascii_to_bytes((char *)ascii_str, strlen(ascii_str), bytes,sizeof(bytes));
    for(int i=0; i<sizeof(bytes);i++){
        CU_ASSERT_EQUAL(bytes[i], result[i]);
    }
}

void test_byte_to_ascii()
{
    uint8_t byte = 0x28;
    char buf[10];
    const char *result = "00101000";
    byte_to_ascii(buf, byte);
    CU_ASSERT_STRING_EQUAL(buf, result);
}

void test_bytes_to_ascii()
{
    uint8_t bytes[4] = {0x11, 0x22, 0x33, 0x44};
    char buf[33];
    const char *result = "00010001001000100011001101000100";
    memset(buf,0,sizeof(buf));
    bytes_to_ascii(buf, sizeof(buf), bytes, sizeof(bytes));
    CU_ASSERT_STRING_EQUAL(buf, result);
}

/**
 * @brief Main test function entry point.
 * 
 * @return int 
 */
int main (void)
{

    CU_pSuite hf_pSuite;
    CU_pSuite ab_pSuite;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

    // Add hf_pSuite to registry
    hf_pSuite = CU_add_suite("Huffman_Coding_Test_Suite", init_suite, clean_suite);
    if (NULL == hf_pSuite) {

        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test_huffman_encode() to suite
    if ((NULL == CU_add_test(hf_pSuite, "Testing Huffman Encoding\n\n", \
     test_huffman_encode))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    //add test_huffman_decode() to suite
    if ((NULL == CU_add_test(hf_pSuite,"Testing Huffman Decoding\n\n", \
    test_huffman_decode))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add ab_pSuite to registry
    ab_pSuite = CU_add_suite("ASCII_Bytes_Conv_Test_Suite", init_suite, clean_suite);
    if (NULL == ab_pSuite) {

        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test_byte_to_ascii() to suite
    if ((NULL == CU_add_test(ab_pSuite, "Testing byte to ASCII code\n\n", \
     test_byte_to_ascii))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test_bytes_to_ascii() to suite
    if ((NULL == CU_add_test(ab_pSuite, "Testing bytes to ASCII codes\n\n", \
     test_bytes_to_ascii))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test_ascii_to_byte() to suite
    if ((NULL == CU_add_test(ab_pSuite, "Testing ASCII code to byte\n\n", \
     test_ascii_to_byte))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test_ascii_to_bytes() to suite
    if ((NULL == CU_add_test(ab_pSuite, "Testing ASCII codes to bytes\n\n", \
     test_ascii_to_bytes))){

        CU_cleanup_registry();
        return CU_get_error();
    }

    // output the test results to the screen
    CU_basic_run_tests();
    //cleaning the registry
    CU_cleanup_registry();

    return CU_get_error();
}

