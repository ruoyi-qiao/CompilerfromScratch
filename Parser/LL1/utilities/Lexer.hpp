#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <cctype> 
#include <vector>
#include "Token.hpp"


class Lexer {
private:
    std::string sourceCode;
    int index, length;
    std::vector<int> line, column;

    const std::string readToken();
    void unreadToken();
    inline char readChar();
    inline void unreadChar();

public:
    void SetSourceCode(const std::string&);
    Token GetNextToken();
};

#endif // _LEXER_H_