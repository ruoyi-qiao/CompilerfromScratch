#ifndef _PARSER_H_
#define _PARSER_H_

#include <memory>
#include "utilities/Token.hpp"
#include "utilities/Node.hpp"
#include "utilities/Lexer.hpp"
#include "grammar.hpp"

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::string sourceCode;

    int getTag(const std::string&);

public:
    std::shared_ptr<Node> Parse();
    void SetSourceCode(const std::string&);
};
#endif