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

#include "Lexer.hpp"
#include <stdexcept>



void Lexer::SetSourceCode(const std::string& l){
    this->sourceCode = l;
    index = 0;
    length = l.length();
    column.resize(length);
    line.resize(length);
    int currentLine = 1;
    int currentColumn = 1;
    for (int i = 0; i < length; i++) {
        if (l[i] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        line[i] = currentLine;
        column[i] = currentColumn;
    }
}

Token Lexer::GetNextToken(){
    Token token;
    std::string lexeme = readToken();
    token.lexeme = lexeme;
    return token;
}

const std::string Lexer::readToken() {
    //tokens are splited by space
    std::string token;
    while (index < sourceCode.length()) {
        char c = readChar();
        if (c == ' ' || c == '\t' || c == '\n') {
            if (token.length() > 0) {
                unreadChar();
                break;
            }
        } else {
            token += c;
        }
    }
    return token;
}

void Lexer::unreadToken() {
    throw std::runtime_error("unReadToken() is not implemented");
}

inline char Lexer::readChar()
{
    if (index < sourceCode.length()) {
        return sourceCode[index++];
    }
    return 0;
}

inline void Lexer::unreadChar()
{
    if (index < 0){
        throw std::runtime_error("unReadChar() is not implemented");
    }
    index--;
}

#endif // _LEXER_H_