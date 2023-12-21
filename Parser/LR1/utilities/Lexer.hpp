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
    std::vector<int> lastTokenIndex;

    const std::string readToken();
    inline char readChar();
    inline char unreadChar();

public:
    void SetSourceCode(const std::string&);
    Token consumeToken();
    Token checkToken();
    void unreadToken();
    void reportError(const std::string&);
    std::pair<int, int> GetLineAndColumn();
};

#include "Lexer.hpp"
#include <stdexcept>



void Lexer::SetSourceCode(const std::string& l){
    this->sourceCode = l;
    index = 0;
    lastTokenIndex.clear();
    lastTokenIndex.push_back(0);
    length = l.length();
    column.resize(length);
    line.resize(length);
    int currentLine = 1;
    int currentColumn = 1;
    for (int i = 0; i < length; i++) {
        line[i] = currentLine;
        column[i] = currentColumn;
        if (l[i] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
    }
}

Token Lexer::consumeToken(){
    Token token;
    std::string lexeme = readToken();
    token.lexeme = lexeme;
    return token;
}

const std::string Lexer::readToken() {
    // tokens are splited by space
    std::string token;
    lastTokenIndex.push_back(index);
    while (index < int(sourceCode.length())) {
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

Token Lexer::checkToken() {
    int old = index;
    std::string token = readToken();
    lastTokenIndex.pop_back();
    index = old;
    return {token};
}

void Lexer::unreadToken() {
    index = lastTokenIndex.back();
    lastTokenIndex.pop_back();
}

inline char Lexer::readChar()
{
    if (index < int(sourceCode.length())) {
        return sourceCode[index++];
    }
    return 0;
}

inline char Lexer::unreadChar()
{
    if (index < 0){
        throw std::runtime_error("unReadChar() is not implemented");
    }
    index--;
    return sourceCode[index];
}

void Lexer::reportError(const std::string& msg) {
    std::cerr << "Error: " << msg << " at line " << line[index - 1] << " column " << column[index - 1] << std::endl;
    throw std::runtime_error(msg);
}

std::pair<int, int> Lexer::GetLineAndColumn() {
    return std::make_pair(line[index - 1], column[index - 1]);
}

#endif // _LEXER_H_