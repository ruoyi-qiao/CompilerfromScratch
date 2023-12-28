#pragma once

namespace Tag {
    static const int 
        // keywords
        KW_UNDEF = 0 + (1 << 8),
        KW_INT = 1 + (1 << 8),
        KW_REAL = 2 + (1 << 8),
        KW_IF = 3 + (1 << 8),
        KW_THEN = 4 + (1 << 8),
        KW_ELSE = 5 + (1 << 8),
        // operators
        OP_ASSIGN = 6 + (1 << 9),
        OP_EQ = 7 + (1 << 9),
        OP_LT = 8 + (1 << 9),
        OP_GT = 9 + (1 << 9),
        OP_LE = 10 + (1 << 9),
        OP_GE = 11 + (1 << 9),
        OP_PLUS = 12 + (1 << 9),
        OP_MINUS = 13 + (1 << 9),
        OP_TIMES = 14 + (1 << 9),
        OP_DIVIDE = 15 + (1 << 9),

        // punctuation
        LPAREN = 16 + (1 << 10),
        RPAREN = 17 + (1 << 10),
        LBRACE = 18 + (1 << 10),
        RBRACE = 19 + (1 << 10),
        ID = 20 + (1 << 10),
        NUM = 21 + (1 << 10),
        REAL = 22 + (1 << 10),
        SEMICOLON = 23 + (1 << 10),
        END = 24 + (1 << 10);
};

#include <map>

// reverse lookup

std::map<int, std::string> tagToString = {
    {1 + (1 << 8), "KW_INT"},
    {2 + (1 << 8), "KW_REAL"},
    {3 + (1 << 8), "KW_IF"},
    {4 + (1 << 8), "KW_THEN"},
    {5 + (1 << 8), "KW_ELSE"},
    {6 + (1 << 9), "OP_ASSIGN"},
    {7 + (1 << 9), "OP_EQ"},
    {8 + (1 << 9), "OP_LT"},
    {9 + (1 << 9), "OP_GT"},
    {10 + (1 << 9), "OP_LE"},
    {11 + (1 << 9), "OP_GE"},
    {12 + (1 << 9), "OP_PLUS"},
    {13 + (1 << 9), "OP_MINUS"},
    {14 + (1 << 9), "OP_TIMES"},
    {15 + (1 << 9), "OP_DIVIDE"},
    {16 + (1 << 10), "LPAREN"},
    {17 + (1 << 10), "RPAREN"},
    {18 + (1 << 10), "LBRACE"},
    {19 + (1 << 10), "RBRACE"},
    {20 + (1 << 10), "ID"},
    {21 + (1 << 10), "NUM"},
    {22 + (1 << 10), "REAL"},
    {23 + (1 << 10), "SEMICOLON"},
    {24 + (1 << 10), "END"}
};