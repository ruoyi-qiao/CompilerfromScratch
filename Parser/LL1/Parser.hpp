#ifndef _PARSER_H_
#define _PARSER_H_

#include <memory>
#include "utilities/Token.hpp"
#include "utilities/Node.hpp"
#include "utilities/Lexer.hpp"

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::string sourceCode;

public:
    std::shared_ptr<Node> Parse();
    void SetSourceCode(const std::string&);
};
#endif