#include <stdio.h>
#include <string.h>
#include "huffman_table.h"
#include "ascii_binary_conv.h"
 
int ASCII_FREQ_TABLE[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2148, 0, 0, 2150, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28940, 73, 516, 
335, 0, 2, 344, 1372, 1149, 1149, 4, 10, 164, 2437, 3875, 
2935, 4870, 6289, 6300, 3367, 4954, 3433, 2521, 2761, 4023, 
3915, 9062, 20, 424, 605, 573, 3901, 291, 798, 223, 987, 2920, 
647, 292, 264, 18, 570, 4, 30, 375, 1234, 1700, 409, 246, 4, 
303, 1773, 447, 382, 46, 88, 34, 1, 1, 3008, 0, 3008, 0, 880, 
4, 8134, 2042, 7748, 5901, 17833, 5725, 4257, 2185, 7275, 400, 
2169, 3883, 3556, 11481, 10377, 3749, 339, 9403, 8403, 9398, 
5676, 1992, 1788, 943, 1594, 17, 0, 2, 0, 0, 0
};

typedef struct node_t {
	struct node_t *left, *right;
	int freq;
	char c;
} *node;
 
/* pool acts as heap basically */ 
struct node_t pool[256] = {{0}};
/* q acts as the heap tree */
node qqq[255], *q = qqq + 1;
int n_nodes = 0, qend = 1;
char *code[128] = {0}, codebuf[128];

/*********************************************
 * Internal Helper Functions  
 *********************************************/

static node new_node(int freq, char c, node a, node b)
{
	node n = pool + n_nodes++;
	if (freq) n->c = c, n->freq = freq;
	else {
		n->left = a;
        n->right = b;
		n->freq = a->freq + b->freq;
	}
	return n;
}
 
/* priority queue */
static void qinsert(node n)
{
	/* higher freq has lower priority
	   move up lower freq */
	int j;
    int i = qend++;
	while ((j = i / 2)) {
		/* compare freq of the new node with the parent's freq */
		if (q[j]->freq <= n->freq) 
            break;
		q[i] = q[j];
        i = j;
	}
	q[i] = n;
}

/* remove the top element(q[1]), 
   and moving up other elements */
static node qremove()
{
	int i, l;
    i = 1;
	node n = q[i];
 
	if (qend < 2) return 0;
	qend--;
	while ((l = i * 2) < qend) {
		if ((l + 1 < qend) && (q[l + 1]->freq < q[l]->freq))
             l++;
		q[i] = q[l];
        i = l;
	}
	q[i] = q[qend];
	return n;
}


/* walk the tree and put 0s and 1s */
static void build_code(node n, char *s, int len)
{
	static char *out = codebuf;
    /* when we reach the leaf nodes, which are nodes with
    * character, we fill out code[n->c] with the accumulated s
    */
	if (n->c) {
        // printf("build_code(): n->c = %c, n->f = %d\n", n->c, n->freq);
		s[len] = 0;
		strcpy(out, s);
		code[n->c] = out;
		out += len + 1;
		return;
	}
 
	s[len] = '0'; 
    build_code(n->left,  s, len + 1);
	s[len] = '1'; 
    build_code(n->right, s, len + 1);
}

static void encode(const char *s, char *out)
{
	while (*s) {
		strcpy(out, code[*s]);
		out += strlen(code[*s++]);
	}
}

// static void decode(const char *s, node t, char *buf)
// {
//     char *bufp = buf;
// 	node n = t;
// 	while (*s) {
// 		if (*s++ == '0') 
//             n = n->left;
// 		else 
//             n = n->right;
 
// 		if (n->c) {
//             *bufp = n->c;
//             bufp++;
//             n = t;
//         }
// 	}

// 	if (t != n) 
//         printf("garbage input\n");
// }

static void decode(const char *s, node t, char *buf)
{
    char *bufp = buf;
	node n = t;
	while (*s) {
		// printf("%c\n", *s);
		if (*s++ == '0') 
            n = n->left;
		else 
            n = n->right;
 
		if (n->c) {
            *bufp = n->c;
            bufp++;
            n = t;
        }
	}
}

static int freq[128] = {0};

// static void dump_code()
// {
//     for (int i = 0; i < 128; i++){
//         if (code[i]) 
//             printf("{'%c',\"%s\",%d,},\n", i, code[i], strlen(code[i]));
//     }   
// }
 
static void dump_code()
{
    for (int i = 0; i < 128; i++){
        if (code[i]) 
            printf("{'%c',\"%s\",%d,freq %d},\n", i, code[i], strlen(code[i]), freq[i]);
    }   
}
 



/*********************************************
 * Public User Interface 
 *********************************************/

void import_huffman_table()
{

    // int freq[128] = {0};
    int i = 0;
	char c[16];
    int *fp = ASCII_FREQ_TABLE;

    /* get the values from the table to update the huffman tree */
	for (i = 0; i < 128; i++){
        freq[i] = *(fp + i);
	}
    
    /* construct the heap tree */
	for (i = 0; i < 128; i++){
        if (freq[i]){
            /* insert new nodes into the que if there is a frequency */
            qinsert(new_node(freq[i], i, 0, 0));	
        }
    }
		
    /* complete heap while merging node staring from the lower frequency nodes */
	/* This is done in the following steps
	   (1) remove top two nodes which have the highest priority (lowest freq)
	   (2) make a new one with the two removed nodes while adding the two freqs
	   (3) when we make the new node, it remembers its children as left/right nodes
	   (4) keep merging the nodes until there is only one node left
	*/

	while (qend > 2){
        //build the tree
        qinsert(new_node(0, 0, qremove(), qremove()));
    }

    /* (1) Traverse the constructed tree from root to leaves 
	   (2) Assign and accumulate 
	   a '0' for left branch and a '1' for the right branch at each node. 
	   (3) The accumulated zeros and ones at each leaf constitute a Huffman encoding
	*/
	build_code(q[1], c, 0);
}

void encode_message(const char *s, char *out)
{
    encode(s, out);
} 

void decode_message(const char *s, char *buf)
{
    decode(s, q[1], buf);
} 

#define TEST

#ifdef TEST

static void test0()
{
	import_huffman_table();			
    dump_code();
}

//LSB    MSB
//0101000 

static void test1(const char *str)
{
    char buf[256];
    char characters[256];
	memset(characters,0,sizeof(characters));
    // char buf[8];
    // char characters;

    printf("original: %s\n", str);
    //import the file and fills frequency array
    import_huffman_table();			
 
	encode_message(str, buf);
	printf("encoded: %s\n", buf);
	printf("length of raw bytes = %d\n", strlen(buf));
	
	printf("decoded: ");
	decode_message(buf, characters);
	printf("buf = %s\n", buf);
    printf("%s\n", characters);
}

static void test2()
{
	char buf[256];
    char characters[256];

    import_huffman_table();			

	char input[] = "0100";
	uint8_t data;

	ascii_to_byte(input, strlen(input), &data);
	printf("data = 0x%x\n", data);

	memset(buf, 0, sizeof(buf));
	byte_to_ascii(buf, data);
	printf("buf = %s\n", buf);
 
	printf("decoded: ");
	decode_message(buf, characters);
    printf("%s\n", characters);
}

// 010100001000001111111010001000
static void test3()
{
	char buf[256];
    char characters[256];

    import_huffman_table();			

	// char input[7] = {'0','1','0','1','0','0','0'};

	//01010000  10000011 11111010 001000
	//0x50      0x83     0xfa     0x20 
	char input[] = "010100001000001111111010001000";
	int input_len = strlen(input);
	uint8_t data[4];

	memset(buf, 0, sizeof(buf));
	memset(data,0,sizeof(data));

	ascii_to_bytes(input, strlen(input), data, sizeof(data));
	bytes_to_ascii(buf, input_len, data, sizeof(data));
	printf("buf = %s\n", buf);
 
	printf("decoded: ");
	decode_message(buf, characters);
    printf("%s\n", characters);
}

static void test4()
{
	char buf1[256];
	char buf2[256];
    char characters[256];
	uint8_t data[64];
	int data_len = 0;
	const char *str = "author\r";

    import_huffman_table();			

	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	memset(data,0,sizeof(data));

	printf("original: %s\n", str);
	encode_message(str, buf1);
	printf("encoded: %s\n", buf1);
	printf("length of raw bytes = %d\n", strlen(buf1));

	data_len = ascii_to_bytes(buf1, strlen(buf1), data, sizeof(data));
	for(int i=0; i<data_len; i++){
		printf("0x%x\n", data[i]);
	}
	bytes_to_ascii(buf2, sizeof(buf2), data, data_len);
	printf("buf = %s\n", buf2);
 
	printf("decoded: ");
	decode_message(buf2, characters);
    printf("%s\n", characters);
}

int main(void)
{
	// test0();
	test1("dump 0xa0 0x64\n");
	return 0;
}
#endif



// huffman_code_t HUFFMAN_TABLE[] = {
//     {'\n', 2148},
//     {' ' , 28940},
//     {'!',  73},
//     {'"', 516},
//     {'#', 335},
//     {'%', 2},
//     {'&', 344},
//     {'\'', 1372},
//     {'(', 1149},
//     {')', 1149},
//     {'*', 4},
//     {'+', 10},
//     {',', 164},
//     {'-', 2437},
//     {'.', 3875},
//     {'/', 2935},
//     {'0', 4870},
//     {'1', 6289},
//     {'2', 6300},
//     {'3', 3367},
//     {'4', 4954},
//     {'5', 3433},
//     {'6', 2521},
//     {'7', 2761},
//     {'8', 4023},
//     {'9', 3915},
//     {':', 9062},
//     {';', 20},
//     {'<', 424},
//     {'=', 605},
//     {'>', 573},
//     {'?', 3901}, 
//     {'@', 291},
//     {'A', 798},
//     {'B', 223},
//     {'C', 987},
//     {'D', 2920},
//     {'E', 647},
//     {'F', 292},
//     {'G', 264},
//     {'H', 18},
//     {'I', 570},
//     {'J', 4},
//     {'K', 30},
//     {'L', 375},
//     {'M', 1234},
//     {'N', 1700},
//     {'O', 409},
//     {'P', 246},
//     {'Q', 4},
//     {'R', 303},
//     {'S', 1773},
//     {'T', 447},
//     {'U', 382},
//     {'V', 46},
//     {'W', 88},
//     {'X', 34},
//     {'Y', 1},
//     {'Z', 1},
//     {'[', 3008},
//     {']', 3008},
//     {'_', 880},
//     {'`', 4},
//     {'a', 8134},
//     {'b', 2042},
//     {'c', 7748},
//     {'d', 5901},
//     {'e', 17833},
//     {'f', 5725},
//     {'g', 4257},
//     {'h', 2185},
//     {'i', 7275},
//     {'j', 400},
//     {'k', 2169},
//     {'l', 3883},
//     {'m', 3556},
//     {'n', 11481},
//     {'o', 10377},
//     {'p', 3749},
//     {'q', 339},
//     {'r', 9403},
//     {'s', 8403},
//     {'t', 9398},
//     {'u', 5676},
//     {'v', 1992},
//     {'w', 1788},
//     {'x', 943},
//     {'y', 1594},
//     {'z', 17},
//     {'\r', 2150},
//     {'|', 2}
// };   