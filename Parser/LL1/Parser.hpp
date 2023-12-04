#ifndef _PARSER_H_
#define _PARSER_H_

#include <memory>
#include <algorithm>
#include "utilities/Lexer.hpp"
#include "utilities/Token.hpp"
#include "grammar.hpp"
#include "utilities/Node.hpp"

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::string sourceCode;
    int getTag(const std::string&);
    std::string convert(int);

public:
    std::vector<std::shared_ptr<Node>> Parse();
    void SetSourceCode(std::string&);
};

void Parser::SetSourceCode(std::string& sourceCode){
    lexer.SetSourceCode(sourceCode);
}

std::vector<std::shared_ptr<Node>> Parser::Parse(){

    std::cout << "Parsing..." << std::endl;

    std::string word = lexer.GetNextToken().lexeme;
    std::vector<int> stack;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> tree;

    stack.push_back(_eof);
    stack.push_back(_program);
    nodes.push_back(std::make_shared<Node>(Node(convert(_program))));
    while (true) {
        if (stack.back() == _eof && word == "$") {
            break;
        } else if (isTerminal(stack.back()) || word == "$") {
            if (stack.back() == getTag(word)) {
                stack.pop_back();
                nodes.pop_back();
                word = lexer.GetNextToken().lexeme;
                // std::cout << "word: " << word << std::endl;
            } else {
                throw std::runtime_error("Syntax error");
            }
        } else {
            int rule = LL1Table[stack.back()][getTag(word) - NONTERMINAL_COUNT];
            if (rule == -1) {
                throw std::runtime_error("Syntax error");
            } else {
                std::shared_ptr<Node> par = nodes.back();
                std::string parState = convert(stack.back());
                // std::cout << par->GetToken().lexeme << " -> " << parState << std::endl;
                stack.pop_back();
                nodes.pop_back();
                int rulesize = 0;
                while (rulesize < MAX_RULE_LENGTH && rulesTable[rule][rulesize] != -1) {
                    rulesize++;
                }
                std::cout << "ky: " << parState << std::endl;
                std::vector<std::shared_ptr<Node>> children;
                for (int i = rulesize - 1; i >= 0; i--) {
                    std::shared_ptr<Node> node = std::make_shared<Node>(convert(rulesTable[rule][i]));
                    // std::cout << "\t" << convert(rulesTable[rule][i]) << std::endl;
                    // par->addChild(node);
                    children.push_back(node);
                    if (rulesTable[rule][i] != _epsilon){
                        stack.push_back(rulesTable[rule][i]);
                        nodes.push_back(node);
                    }
                }
                reverse(children.begin(), children.end());
                par->setChildren(children);
                tree.push_back(par);

                // std::cout << "ch: " << par->ToString() << std::endl;
                for (auto child : par->GetChildren()) {
                    std::cout << "\t " << child->GetToken().lexeme << std::endl;
                }
            }
        }
    }
    // check use_count
    // std::cout << "trees: " << tree.size() << std::endl;
    // for (int i = 0; i < tree.size(); i++) {
    //     std::cout << tree[i]->ToString() << " with " << tree[i].use_count() << " references" << std::endl;
    //     std::cout << "children: " << tree[i]->GetChildren().size() << std::endl;
    // }
    return tree;
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
    } else if (word == ";") {
        return _semi;
    } else if (word == "E") {
        return _epsilon;
    } else {
        throw std::runtime_error("Unknown token");
    }
}
/*
0: <program>
1: <stmt>
2: <compoundstmt>
3: <stmts>
4: <ifstmt>
5: <whilestmt>
6: <assgstmt>
7: <boolexpr>
8: <boolop>
9: <arithexpr>
10: <arithexprprime>
11: <multexpr>
12: <multexprprime>
13: <simpleexpr>
14: "+"
15: "*"
16: ""
17: "<="
18: ">"
19: "=="
20: ">="
21: "-"
22: "if"
23: ";"
24: "then"
25: "while"
26: "<"
27: "="
28: "("
29: "else"
30: ")"
31: "{"
32: "}"
33: NUM
34: ID
35: "/"
36: eof
0: ['<compoundstmt>']
1: ['<ifstmt>']
2: ['<whilestmt>']
3: ['<assgstmt>']
4: ['"{"', '<stmts>', '"}"']
5: ['<stmt>', '<stmts>']
6: ['""']
7: ['"if"', '"("', '<boolexpr>', '")"', '"then"', '<stmt>', '"else"', '<stmt>']
8: ['"while"', '"("', '<boolexpr>', '")"', '<stmt>']
9: ['ID', '"="', '<arithexpr>', '";"']
10: ['<arithexpr>', '<boolop>', '<arithexpr>']
11: ['"<"']
12: ['">"']
13: ['"<="']
14: ['">="']
15: ['"=="']
16: ['<multexpr>', '<arithexprprime>']
17: ['"+"', '<multexpr>', '<arithexprprime>']
18: ['"-"', '<multexpr>', '<arithexprprime>']
19: ['<simpleexpr>', '<multexprprime>']
20: ['"*"', '<simpleexpr>', '<multexprprime>']
21: ['"/"', '<simpleexpr>', '<multexprprime>']
22: ['ID']
23: ['NUM']
24: ['"("', '<arithexpr>', '")"']
*/
std::string Parser::convert(int index) {
    std::string symbol;
    switch (index)
    {
    case 0:
        return "program";
    case 1:
        return "stmt";
    case 2:
        return "compoundstmt";
    case 3:
        return "stmts";
    case 4:
        return "ifstmt";
    case 5:
        return "whilestmt";
    case 6:
        return "assgstmt";
    case 7:
        return "boolexpr";
    case 8:
        return "boolop";
    case 9:
        return "arithexpr";
    case 10:
        return "arithexprprime";
    case 11:
        return "multexpr";
    case 12:
        return "multexprprime";
    case 13:
        return "simpleexpr";
    case 14:
        return "+";
    case 15:
        return "*";
    case 16:
        return "E";
    case 17:
        return "<=";
    case 18:
        return ">";
    case 19:
        return "==";
    case 20:
        return ">=";
    case 21:
        return "-";
    case 22:
        return "if";
    case 23:
        return ";";
    case 24:
        return "then";
    case 25:
        return "while";
    case 26:
        return "<";
    case 27:
        return "=";
    case 28:
        return "(";
    case 29:
        return "else";
    case 30:
        return ")";
    case 31:
        return "{";
    case 32:
        return "}";
    case 33:
        return "NUM";
    case 34:
        return "ID";
    case 35:
        return "/";
    case 36:
        return "eof";
    default:
        throw std::runtime_error("Unknown token");
    }
}


#endif