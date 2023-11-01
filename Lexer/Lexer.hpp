#include "DFAConverter.hpp"
#include "LexerError.hpp"
#include <stack>
#include <fstream>
#include <iomanip>

struct Token {
    std::string value;
    std::string type;
};

#define dbg(x...) do { std::cerr << #x << "  -> "; err(x); } while (0);
void err() { std::cerr << std::endl; }
template<class T, class... Ts> void err(const T& arg, const Ts&... args) {std::cerr << arg << " "; err(args...); }

using DFATable = std::vector<std::vector<int>>;

class Lexer {
public:
    Lexer(
        DFAConverter& converter,
        const std::string& sourceCode
    ) : acceptedStates_(converter.accepted),
        transitionTable_(converter.goTo),
        tokenLabels_(converter.tokens),
        sourceCode_(sourceCode), 
        input(sourceCode),
        error(sourceCode){

        currentState_ = 0;
        currentTokenStart_ = 0;
        // output table for debug out.txt
        std::ofstream out("out.txt");
        // header setw(3) for debug out.txt
        out << std::setw(4) << " ";
        for (int i = 0; i <= 128; i++) {
            out << std::setw(4) << i;
        }
        out << std::endl;
        for (int i = 0; i < transitionTable_.size(); i++) {
            out << std::setw(4) << i;
            for (int j = 0; j < transitionTable_[i].size(); j++) {
                out << std::setw(4) << transitionTable_[i][j];
            }
            out << std::endl;
        }
    }

    Token getNextToken();
    bool isAcceptedState(int state) const;
    int transition(int state, int c) const;


private:
    StringInput input;
    LexerError error;
    std::string sourceCode_;
    const std::vector<bool>& acceptedStates_;
    const DFATable& transitionTable_;
    const std::vector<std::string>& tokenLabels_;
    int currentState_;
    size_t currentTokenStart_;
    const int ErrorState = -1;
    const int BadState = -2;
};

bool Lexer::isAcceptedState(int state) const {
    if (state < 0 || state >= acceptedStates_.size()) 
        return false;
    return acceptedStates_[state];
}

int Lexer::transition(int state, int c) const {
    if (state < 0 || state >= transitionTable_.size()) {
        std::cout << "error: " << state << std::endl;
        throw LexerError("Invalid state: " + std::to_string(state));
    }
    if (c < 0 || c >= transitionTable_[state].size()) {
        std::cout << "error: " << state << std::endl;
        std::cout << "error: " << (int)c << std::endl;
        throw LexerError("Invalid input: " + std::to_string(c));
    }
    return transitionTable_[state][c];
}

Token Lexer::getNextToken() {
    if (!input.available()) {
        return {"", "EOF"};
    }
    std::string lexeme = "";
    std::vector<int> stateStack;
    
    currentState_ = 0;
    stateStack.push_back(BadState);

    while (currentState_ != ErrorState && input.available()) {
        int c = input.readOrigin();
        error.handle(c);
        lexeme += c;
        if (isAcceptedState(currentState_)) {
            stateStack.clear();
        }
        stateStack.push_back(currentState_);
        currentState_ = transition(currentState_, c < 0 ? 128 : c);
    }
        

    while (!isAcceptedState(currentState_) && currentState_ != BadState) {
        lexeme.pop_back();
        input.retrace();
        error.retrace();
        currentState_ = stateStack.back();
        stateStack.pop_back();
    }
    if (isAcceptedState(currentState_)) {
        return {lexeme, tokenLabels_[currentState_]};
    } else {
        dbg("?")
        // input.retrace(); // neglect the char that caused the error
        int c = input.read();
        // std::cout << "lexeme: " << lexeme << std::endl;
        std::cout << "c: " << c << std::endl;
        // error.reportError("Unexpected character: " + std::to_string(c));
        return {"", ""};
    }
}