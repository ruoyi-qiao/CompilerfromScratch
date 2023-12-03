#include "Parser.hpp"

void Parser::SetSourceCode(const std::string& sourceCode){
    this->sourceCode = sourceCode;
    lexer.SetSourceCode(sourceCode);
}

std::shared_ptr<Node> Parser::Parse(){
    std::string word = lexer.GetNextToken().lexeme;
    std::vector<int> stack;
    stack.push_back(_eof);
    stack.push_back(_program);
    while (true) {
        if (stack.back() == EOF_SYMBOL && word == "$") {
            return std::make_shared<Node>({(Tag)getTag("program"), "program"});
        } else if (isTerminal(stack.back()) || word == "$") {
            if (stack.back() == getTag(word)) {
                stack.pop_back();
                word = lexer.GetNextToken().lexeme;
            } else {
                throw std::runtime_error("Syntax error");
            }
        } else {
            int rule = LL1Table[stack.back()][getTag(word) - NONTERMINAL_COUNT];
            if (rule == -1) {
                throw std::runtime_error("Syntax error");
            } else {
                stack.pop_back();
                int rulesize = rulesTable[rule][0];
                for (int i = rulesize; i >= 1; i--) {
                    if (rulesTable[rule][i] != _epsilon)
                        stack.push_back(rulesTable[rule][i]);
                }
            }
        }
    }
}
/*
{ }
if ( ) then else
while ( )
ID = 
> < >= <= ==
+ -
* /
ID NUM
E 是'空'
*/
/**
 * @brief Returns the corresponding tag for a given word.
 * 
 * This function takes a word as input and returns the corresponding tag based on the word's value.
 * If the word matches any of the predefined tags, the corresponding tag value is returned.
 * If the word does not match any of the predefined tags, an exception is thrown.
 * 
 * @param word The word for which the tag is to be determined.
 * @return The tag value corresponding to the given word.
 * @throws std::runtime_error if the word is unknown.
 */
int Parser::getTag(const std::string& word){
    if (word == "{") {
        return _lbrace;
    } else if (word == "}") {
        return _rbrace;
    } else if (word == "if") {
        return _if;
    } else if (word == "(") {
        return _lparen;
    } else if (word == ")") {
        return _rparen;
    } else if (word == "then") {
        return _then;
    } else if (word == "else") {
        return _else;
    } else if (word == "while") {
        return _while;
    } else if (word == "ID") {
        return _id;
    } else if (word == "=") {
        return _assign;
    } else if (word == ">") {
        return _gt;
    } else if (word == "<") {
        return _lt;
    } else if (word == ">=") {
        return _ge;
    } else if (word == "<=") {
        return _le;
    } else if (word == "==") {
        return _eq;
    } else if (word == "+") {
        return _plus;
    } else if (word == "-") {
        return _minus;
    } else if (word == "*") {
        return _times;
    } else if (word == "/") {
        return _div;
    } else if (word == "NUM") {
        return _num;
    } else if (word == "$") {
        return _eof;
    } else {
        throw std::runtime_error("Unknown token");
    }
}