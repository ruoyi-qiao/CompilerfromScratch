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
    std::vector<std::vector<std::string>> snapshots;

    int getTerminalIndex(const std::string&);
    int getNonTerminalIndex(const std::string&);
    std::vector<std::string> backDeducing(std::vector<std::string>, std::string, int, int);
    std::string convert(int);
    
    void error_handle();

public:
    std::string errorToken;

    std::vector<std::vector<std::string>> Parse();
    void SetSourceCode(std::string&);
};

void Parser::SetSourceCode(std::string& sourceCode){
    errorToken = "";
    lexer.SetSourceCode(sourceCode);

    std::string word;
    std::vector<std::string> words;

    for (const auto & c : sourceCode) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (word != "") {
                words.push_back(word);
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (word != "$") {
        words.push_back(word);
    }
    snapshots.push_back(words);
}

std::vector<std::vector<std::string>> Parser::Parse(){
    int startState = 0, endpos = -1;
    std::vector<int> stateStack;
    std::vector<symbol> symbolStack;

    stateStack.push_back(startState);

    std::cerr << "Parsing..." << std::endl;

    for (;;) {
        std::cerr << "Current state: " << stateStack.back() << std::endl;
        currentToken = lexer.GetNextToken();
        if (currentToken.lexeme == "$") {
            currentToken.lexeme = "eof";
        }
        std::cerr << "Current token: " << currentToken.lexeme << " (" << getTerminalIndex(currentToken.lexeme) <<  ")" << std::endl;
        int action = LR1ActionTable[stateStack.back()][getTerminalIndex(currentToken.lexeme)];
        if (action == 0) { // accept
            std::cerr << "Accept" << std::endl;
            return snapshots;
        }
        if (action == -1) {
            std::cerr << "Error" << std::endl;
            errorToken = currentToken.lexeme;
            error_handle();
        }
        if (action & (1 << 8)) { // reduce
            int production = action & ((1 << 8) - 1);
            int currentProductionLength = productionLength[production];
            std::vector<std::string> current = snapshots.back();

            std::cerr << "Reduce by production " << production << std::endl;
            std::vector<std::string>  snapshot = backDeducing(current, productionName[production], endpos, currentProductionLength);
            
            for (auto s : snapshot) {
                std::cerr << s << " ";
            } std::cerr << std::endl;

            for (int i = 0; i < currentProductionLength; i++) {
                stateStack.pop_back();
                symbolStack.pop_back();
                endpos--;
            }


            std::cerr << "prodcutionName: " << productionName[production] << std::endl;
            int gotoState = LR1GotoTable[stateStack.back()][getNonTerminalIndex(productionName[production])];
            stateStack.push_back(gotoState);
            symbolStack.push_back({false, productionName[production]});
            endpos++;
            lexer.unreadToken();
        } else { // shift
            stateStack.push_back(action);
            symbolStack.push_back({true, currentToken.lexeme});
            endpos++;
        }
    }

}

std::vector<std::string> Parser::backDeducing(std::vector<std::string> current, std::string symbol, int endpos, int productionLength) {
    std::vector<std::string> result;
    int startpos = endpos - productionLength + 1;
    for (int index = 0; index < startpos; index++) {
        result.push_back(current[index]);
    }
    result.push_back(symbol);
    for (int index = endpos + 1; index < int(current.size()); index++) {
        result.push_back(current[index]);
    }
    snapshots.push_back(result);
    return result;
}

int Parser::getTerminalIndex(const std::string& lexeme) {
    assert(Terminal2Index.find(lexeme) != Terminal2Index.end());
    return Terminal2Index[lexeme];
}

int Parser::getNonTerminalIndex(const std::string& name) {
    assert(NonTerminal2Index.find(name) != NonTerminal2Index.end());
    return NonTerminal2Index[name];
}

std::string Parser::convert(int tag) {
    assert(Index2Terminal.find(tag) != Index2Terminal.end());
    return Index2Terminal[tag];
}

void Parser::error_handle() {
    exit(1);
}

#endif