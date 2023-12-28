#pragma once

#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <cctype> 
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Token.hpp"
#include "Word.hpp"

class Lexer {
private:
    std::string sourceCode;
    int index, length;
    std::vector<int> line, column;
    std::vector<int> lastTokenIndex;
    std::unordered_map<std::string, Word> words;

    const std::string readLexeme();
    inline char readChar();
    inline char unreadChar();
    const int lexemeToTag(const std::string& lexeme);

public:
    void SetSourceCode(const std::string&);
    Word consumeToken();
    Word checkToken();
    void unreadToken();
    void printWords();
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

Word Lexer::consumeToken(){
    std::string lexeme = readLexeme();
    if (words.find(lexeme) != words.end()) {
        return words[lexeme];
    } else {
        std::cout << "lexeme: " << lexeme << std::endl;
        std::cout << "tag: " << lexemeToTag(lexeme) << std::endl;
        words[lexeme] = Word(lexeme, lexemeToTag(lexeme));
        return words[lexeme];
    }
}

const std::string Lexer::readLexeme() {
    // tokens are splited by space
    std::string lexeme = "";
    lastTokenIndex.push_back(index);
    while (index < int(sourceCode.length())) {
        char c = readChar();
        if (c == ' ' || c == '\t' || c == '\n') {
            if (lexeme.length() > 0) {
                unreadChar();
                break;
            }
        } else {
            lexeme += c;
        }
    }
    return lexeme;
}

Word Lexer::checkToken() {
    int old = index;
    std::string lexeme = readLexeme();
    lastTokenIndex.pop_back();
    index = old;
    return Word(lexeme, lexemeToTag(lexeme));
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

const int Lexer::lexemeToTag(const std::string& lexeme) {
    // std::cout << "lexeme: " << lexeme << std::endl;
    if (lexeme == "int") {
        return Tag::KW_INT;
    } else if (lexeme == "float") {
        return Tag::KW_REAL;
    } else if (lexeme == "if") {
        return Tag::KW_IF;
    } else if (lexeme == "then") {
        return Tag::KW_THEN;
    } else if (lexeme == "else") {
        return Tag::KW_ELSE;
    } else if (lexeme == "=") {
        return Tag::OP_ASSIGN;
    } else if (lexeme == "==") {
        return Tag::OP_EQ;
    } else if (lexeme == "<") {
        return Tag::OP_LT;
    } else if (lexeme == ">") {
        return Tag::OP_GT;
    } else if (lexeme == "+") {
        return Tag::OP_PLUS;
    } else if (lexeme == "-") {
        return Tag::OP_MINUS;
    } else if (lexeme == "*") {
        return Tag::OP_TIMES;
    } else if (lexeme == "/") {
        return Tag::OP_DIVIDE;
    } else if (lexeme == "(") {
        return Tag::LPAREN;
    } else if (lexeme == ")") {
        return Tag::RPAREN;
    } else if (lexeme == ";") {
        return Tag::SEMICOLON;
    } else if (lexeme == "{") {
        return Tag::LBRACE;
    } else if (lexeme == "}") {
        return Tag::RBRACE;
    } else if (lexeme == "$") {
        return Tag::END;
    } else if (std::isdigit(lexeme[0])) {
        if (lexeme.find('.') != std::string::npos) {
            return Tag::REAL;
        } else {
            return Tag::NUM;
        }
    } else if (std::isalpha(lexeme[0])) {
        return Tag::ID;
    } else {
        reportError("Unknown lexeme: " + lexeme);
    }

}

void Lexer::printWords() {
    // format output like [lexeme, tag]
    for (auto it = words.begin(); it != words.end(); it++) {
        std::cout << "[" << it->first << ", " << (it->second.tag & ((1 << 8) - 1)) << "]" << std::endl;
    }
}
#endif // _LEXER_H_