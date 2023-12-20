#ifndef _PARSER_H_
#define _PARSER_H_

#include <memory>
#include <algorithm>
#include "utilities/Lexer.hpp"
#include "utilities/Token.hpp"
#include "grammar.hpp"
#include "utilities/Node.hpp"
#include <assert.h>

struct symbol {
    bool isTerminal;
    std::string name;
};

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::string sourceCode;

    int getTag(const std::string&);
    std::string convert(int);
    void error_handle();

public:
    std::string errorToken;

    std::vector<std::shared_ptr<Node>> Parse();
    void SetSourceCode(std::string&);
};

void Parser::SetSourceCode(std::string& sourceCode){
    errorToken = "";
    lexer.SetSourceCode(sourceCode);
}

std::vector<std::shared_ptr<Node>> Parser::Parse(){
    int startState = 0;
    std::vector<int> stateStack;
    std::vector<symbol> symbolStack;

    stateStack.push_back(startState);

    std::cerr << "Parsing..." << std::endl;

    for (;;) {
        std::cerr << "Current state: " << stateStack.back() << std::endl;
        currentToken = lexer.GetNextToken();
        std::cerr << "Current token: " << currentToken.lexeme << std::endl;
        int action = LR1ActionTable[stateStack.back()][getTag(currentToken.lexeme)];
        if (action & (1 << 8)) { // reduce
            int production = action & ((1 << 8) - 1);
            std::cerr << "Reduce by production " << production << std::endl;
            

            lexer.unreadToken();
        } else { // shift
            stateStack.push_back(action);
            symbolStack.push_back({true, currentToken.lexeme});
        }
    }

}

int Parser::getTag(const std::string& lexeme) {
    assert(Terminal2Index.find(lexeme) != Terminal2Index.end());
    return Terminal2Index[lexeme];
}

std::string Parser::convert(int tag) {
    assert(Index2Terminal.find(tag) != Index2Terminal.end());
    return Index2Terminal[tag];
}

void Parser::error_handle() {
    exit(1);
}

#endif