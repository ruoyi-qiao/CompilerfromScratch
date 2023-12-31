#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include <array>
#include <vector>

constexpr int MAX_RULE_LENGTH = 8;
constexpr int MAX_RULE_COUNT = 25;
constexpr int TERMINAL_COUNT = 23;
constexpr int NONTERMINAL_COUNT = 14;
constexpr int EOF_SYMBOL = 36;
/*
SYMBOL MAPPING:

    0: <program>
    1: <stmt>
    2: <compoundstmt>
    3: <stmts>
    4: <ifstmt>
    5: <whilestmt>
    6: <assgstmt>
    7: <boolexpr>
    8: <boolop>
    9: <arithexpr>
    10: <arithexprprime>
    11: <multexpr>
    12: <multexprprime>
    13: <simpleexpr>
    14: "+"
    15: "*"
    16: ""
    17: "<="
    18: ">"
    19: "=="
    20: ">="
    21: "-"
    22: "if"
    23: ";"
    24: "then"
    25: "while"
    26: "<"
    27: "="
    28: "("
    29: "else"
    30: ")"
    31: "{"
    32: "}"
    33: NUM
    34: ID
    35: "/"
    36: eof

Rules Mapping:

    0: ['<compoundstmt>']
    1: ['<ifstmt>']
    2: ['<whilestmt>']
    3: ['<assgstmt>']
    4: ['"{"', '<stmts>', '"}"']
    5: ['<stmt>', '<stmts>']
    6: ['""']
    7: ['"if"', '"("', '<boolexpr>', '")"', '"then"', '<stmt>', '"else"', '<stmt>']
    8: ['"while"', '"("', '<boolexpr>', '")"', '<stmt>']
    9: ['ID', '"="', '<arithexpr>', '";"']
    10: ['<arithexpr>', '<boolop>', '<arithexpr>']
    11: ['"<"']
    12: ['">"']
    13: ['"<="']
    14: ['">="']
    15: ['"=="']
    16: ['<multexpr>', '<arithexprprime>']
    17: ['"+"', '<multexpr>', '<arithexprprime>']
    18: ['"-"', '<multexpr>', '<arithexprprime>']
    19: ['<simpleexpr>', '<multexprprime>']
    20: ['"*"', '<simpleexpr>', '<multexprprime>']
    21: ['"/"', '<simpleexpr>', '<multexprprime>']
    22: ['ID']
    23: ['NUM']
    24: ['"("', '<arithexpr>', '")"']
*/

constexpr int   _program = 0, _stmt = 1, _compoundstmt = 2, 
                _stmts = 3, _ifstmt = 4, _whilestmt = 5, 
                _assgstmt = 6, _boolexpr = 7, _boolop = 8, 
                _arithexpr = 9, _arithexprprime = 10, _multexpr = 11, 
                _multexprprime = 12, _simpleexpr = 13;

constexpr int   _plus = 14, _times = 15, _epsilon = 16, 
                _le = 17, _gt = 18, _eq = 19, 
                _ge = 20, _minus = 21, _if = 22, 
                _semi = 23, _then = 24, _while = 25, 
                _lt = 26, _assign = 27, _lparen = 28, 
                _else = 29, _rparen = 30, _lbrace = 31, 
                _rbrace = 32, _num = 33, _id = 34, 
                _div = 35, _eof = 36;

int rulesTable[25][8] = {
	{2, -1, -1, -1, -1, -1, -1, -1, },
	{4, -1, -1, -1, -1, -1, -1, -1, },
	{5, -1, -1, -1, -1, -1, -1, -1, },
	{6, -1, -1, -1, -1, -1, -1, -1, },
	{31, 3, 32, -1, -1, -1, -1, -1, },
	{1, 3, -1, -1, -1, -1, -1, -1, },
	{16, -1, -1, -1, -1, -1, -1, -1, },
	{22, 28, 7, 30, 24, 1, 29, 1, },
	{25, 28, 7, 30, 1, -1, -1, -1, },
	{34, 27, 9, 23, -1, -1, -1, -1, },
	{9, 8, 9, -1, -1, -1, -1, -1, },
	{26, -1, -1, -1, -1, -1, -1, -1, },
	{18, -1, -1, -1, -1, -1, -1, -1, },
	{17, -1, -1, -1, -1, -1, -1, -1, },
	{20, -1, -1, -1, -1, -1, -1, -1, },
	{19, -1, -1, -1, -1, -1, -1, -1, },
	{11, 10, -1, -1, -1, -1, -1, -1, },
	{14, 11, 10, -1, -1, -1, -1, -1, },
	{21, 11, 10, -1, -1, -1, -1, -1, },
	{13, 12, -1, -1, -1, -1, -1, -1, },
	{15, 13, 12, -1, -1, -1, -1, -1, },
	{35, 13, 12, -1, -1, -1, -1, -1, },
	{34, -1, -1, -1, -1, -1, -1, -1, },
	{33, -1, -1, -1, -1, -1, -1, -1, },
	{28, 9, 30, -1, -1, -1, -1, -1, },
};
//    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
int LL1Table[14][23] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1,  1, -1, -1,  2, -1, -1, -1, -1, -1,  0, -1, -1,  3, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, },
	{-1, -1,  6, -1, -1, -1, -1, -1,  5, -1, -1,  5, -1, -1, -1, -1, -1,  5,  6, -1,  5, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  9, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1, 10, 10, -1, -1, },
	{-1, -1, -1, 13, 12, 15, 14, -1, -1, -1, -1, -1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, -1, -1, -1, -1, 16, 16, -1, -1, },
	{17, -1,  6,  6,  6,  6,  6, 18, -1,  6, -1, -1,  6, -1, -1, -1,  6, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 19, -1, -1, -1, -1, 19, 19, -1, -1, },
	{ 6, 20,  6,  6,  6,  6,  6,  6, -1,  6, -1, -1,  6, -1, -1, -1,  6, -1, -1, -1, -1, 21, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1, 23, 22, -1, -1, },
};

bool isTerminal(int symbol) {
    return symbol >= 14;
}

bool isNonTerminal(int symbol) {
    return symbol < 14;
}

#endif // _GRAMMAR_H_