#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>

enum class Tag {
    NUM,
    ID,
    PLUS,
    MINUS,
    TIMES,
    DIV,
    LPAREN,
    RPAREN,
    ASSIGN,
    SEMI,
    IF,
    ELSE,
    WHILE,
    DO,
    BREAK,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE,
    AND,
    OR,
    NOT,
    TRUE,
    FALSE,
    INT,
    BOOL,
    VOID,
    RETURN,
    ERROR,
    ENDFILE
};

struct Token
{
    Tag tag;
    std::string lexeme;
};

#endif
