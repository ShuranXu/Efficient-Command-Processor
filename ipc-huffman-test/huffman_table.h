#include <stdlib.h>

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

typedef struct {
    char c;
    int freq;
}huffman_code_t;

huffman_code_t HUFFMAN_TABLE[] = {
    {'\n', 2148},
    {' ' , 28940},
    {'!',  73},
    {'"', 516},
    {'#', 335},
    {'%', 2},
    {'&', 344},
    {'\'', 1372},
    {'(', 1149},
    {')', 1149},
    {'*', 4},
    {'+', 10},
    {',', 164},
    {'-', 2437},
    {'.', 3875},
    {'/', 2935},
    {'0', 4870},
    {'1', 6289},
    {'2', 6300},
    {'3', 3367},
    {'4', 4954},
    {'5', 3433},
    {'6', 2521},
    {'7', 2761},
    {'8', 4023},
    {'9', 3915},
    {':', 9062},
    {';', 20},
    {'<', 424},
    {'=', 605},
    {'>', 573},
    {'?', 3901}, 
    {'@', 291},
    {'A', 798},
    {'B', 223},
    {'C', 987},
    {'D', 2920},
    {'E', 647},
    {'F', 292},
    {'G', 264},
    {'H', 18},
    {'I', 570},
    {'J', 4},
    {'K', 30},
    {'L', 375},
    {'M', 1234},
    {'N', 1700},
    {'O', 409},
    {'P', 246},
    {'Q', 4},
    {'R', 303},
    {'S', 1773},
    {'T', 447},
    {'U', 382},
    {'V', 46},
    {'W', 88},
    {'X', 34},
    {'Y', 1},
    {'Z', 1},
    {'[', 3008},
    {']', 3008},
    {'_', 880},
    {'`', 4},
    {'a', 8134},
    {'b', 2042},
    {'c', 7748},
    {'d', 5901},
    {'e', 17833},
    {'f', 5725},
    {'g', 4257},
    {'h', 2185},
    {'i', 7275},
    {'j', 400},
    {'k', 2169},
    {'l', 3883},
    {'m', 3556},
    {'n', 11481},
    {'o', 10377},
    {'p', 3749},
    {'q', 339},
    {'r', 9403},
    {'s', 8403},
    {'t', 9398},
    {'u', 5676},
    {'v', 1992},
    {'w', 1788},
    {'x', 943},
    {'y', 1594},
    {'z', 17},
    {'\r', 2150},
    {'|', 2}
};   

/*

---------CODE TABLE---------
----------------------------
CHAR  FREQ  CODE
----------------------------
\n    2148
' '   28940
!     73  
"     516 
#     335 
%     2   
&     344 
'     1372
(     1149
)     1149
*     4   
+     10  
,     164 
-     2437
.     3875
/     2935
0     4870
1     6289
2     6300
3     3367
4     4954
5     3433
6     2521
7     2761
8     4023
9     3915
:     9062
;     20  
<     424 
=     605 
>     573 
?     3901
@     291 
A     798 
B     223 
C     987 
D     2920
E     647 
F     292 
G     264 
H     18  
I     570 
J     4   
K     30  
L     375 
M     1234
N     1700
O     409 
P     246 
Q     4   
R     303 
S     1773
T     447 
U     382 
V     46  
W     88  
X     34  
Y     1   
Z     1   

[     3008
]     3008
_     880 
`     4   
a     8134
b     2042
c     7748
d     5901
e     17833
f     5725
g     4257
h     2185
i     7275
j     400 
k     2169
l     3883
m     3556
n     11481
o     10377
p     3749
q     339 
r     9403
s     8403
t     9398
u     5676
v     1992
w     1788
x     943 
y     1594
z     17  
|     2 

*/