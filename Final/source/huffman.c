#include <stdio.h>
#include <string.h>
#include "huffman_table.h"

typedef struct node_t {
	struct node_t *left, *right;
	int freq;
	char c;
} *node;

/* pool acts as heap basically */
struct node_t pool[512] = {{0}};
/* q acts as the heap tree */
node qqq[511], *q = qqq + 1;
int n_nodes = 0, qend = 1;
char *code[128] = {0}, buf[1024];


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
	static char *out = buf;
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

static void decode(const char *s, node t)
{
	node n = t;
	while (*s) {
		if (*s++ == '0')
            n = n->left;
		else
            n = n->right;

		if (n->c) {
            putchar(n->c);
            n = t;
        }
	}

	putchar('\n');
	if (t != n)
        printf("garbage input\n");
}

static void dump_code()
{
    for (int i = 0; i < 128; i++){
        if (code[i])
            printf("'%c': %s\n", i, code[i]);
    }
}

/*********************************************
 * Public User Interface
 *********************************************/

void import_huffman_table()
{

    int freq[128] = {0};
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

void decode_message(const char *s)
{
    decode(s, q[1]);
}

