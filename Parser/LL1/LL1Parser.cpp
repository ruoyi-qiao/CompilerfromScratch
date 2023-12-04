#include <string>
#include <iostream>

struct Token
{
    std::string lexeme;
};


#include <string>
#include <memory>
#include <vector>


class Node {
private:
    int childCount;
    Token t;
    std::vector<std::shared_ptr<Node>> children;

public:
    Node();
    Node(const Token&);
    Node(const std::string&);
    Node(const Token&, const std::vector<std::shared_ptr<Node>>&);

    const Token& GetToken();
    std::vector<std::shared_ptr<Node>> GetChildren();
    void addChild(const std::shared_ptr<Node>&);
    void setChild(int, const std::shared_ptr<Node>&);
    void setChildren(const std::vector<std::shared_ptr<Node>>&);

    std::string ToString();
    void PrintTree(int, bool, std::vector<std::shared_ptr<Node>>&);
};



Node::Node(){
    t = {};
    childCount = 0;
    children = {};
}

Node::Node(const Token& token) {
    t = token;
    childCount = 0;
    children = {};
}

Node::Node(const std::string& lexeme) {
    t.lexeme = lexeme;
    childCount = 0;
    children = {};
}

Node::Node(const Token& token, const std::vector<std::shared_ptr<Node>>& children) {
    t = token;
    childCount = children.size();
    this->children = children;
}

const Token& Node::GetToken() {
    return t;
}

void Node::addChild(const std::shared_ptr<Node>& child) {
    children.push_back(child);
    childCount++;
}

void Node::setChild(int index, const std::shared_ptr<Node>& child) {
    children[index] = child;
}

void Node::setChildren(const std::vector<std::shared_ptr<Node>>& children) {
    this->children = children;
    childCount = children.size();
}

std::string Node::ToString(){
    return t.lexeme;
}

std::vector<std::shared_ptr<Node>> Node::GetChildren(){
    return children;
}

void Node::PrintTree(int depth, bool last, std::vector<std::shared_ptr<Node>>& tree){
    for (int i = 0; i < depth; i++) {
        std::cout << "\t";
    }
    std::cout << ToString();
    if (!(childCount == 0 && last)) {
        std::cout << std::endl;
    }
    for (int i = 0; i < childCount; i++) {
        children[i]->PrintTree(depth + 1, (i == childCount - 1) && last, tree);
    }
}

#include <string>
#include <cctype> 
#include <vector>


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
    Token GetNextToken();
    void unreadToken();
    void reportError(const std::string&);
    std::pair<int, int> GetLineAndColumn();
};

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

Token Lexer::GetNextToken(){
    Token token;
    std::string lexeme = readToken();
    token.lexeme = lexeme;
    return token;
}

const std::string Lexer::readToken() {
    // tokens are splited by space
    std::string token;
    lastTokenIndex.push_back(index);
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
    index = lastTokenIndex.back();
    lastTokenIndex.pop_back();
}

inline char Lexer::readChar()
{
    if (index < sourceCode.length()) {
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


#include <array>
#include <vector>

constexpr int MAX_RULE_LENGTH = 8;
constexpr int MAX_RULE_COUNT = 25;
constexpr int TERMINAL_COUNT = 23;
constexpr int NONTERMINAL_COUNT = 14;
constexpr int EOF_SYMBOL = 36;
/*
SYMBOL MAPPING:

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

Rules Mapping:

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

constexpr int   _program = 0, _stmt = 1, _compoundstmt = 2, 
                _stmts = 3, _ifstmt = 4, _whilestmt = 5, 
                _assgstmt = 6, _boolexpr = 7, _boolop = 8, 
                _arithexpr = 9, _arithexprprime = 10, _multexpr = 11, 
                _multexprprime = 12, _simpleexpr = 13;

constexpr int   _plus = 14, _times = 15, _epsilon = 16, 
                _le = 17, _gt = 18, _eq = 19, 
                _ge = 20, _minus = 21, _if = 22, 
                _semi = 23, _then = 24, _while = 25, 
                _lt = 26, _assign = 27, _lparen = 28, 
                _else = 29, _rparen = 30, _lbrace = 31, 
                _rbrace = 32, _num = 33, _id = 34, 
                _div = 35, _eof = 36;

int rulesTable[25][8] = {
	{2, -1, -1, -1, -1, -1, -1, -1, },
	{4, -1, -1, -1, -1, -1, -1, -1, },
	{5, -1, -1, -1, -1, -1, -1, -1, },
	{6, -1, -1, -1, -1, -1, -1, -1, },
	{31, 3, 32, -1, -1, -1, -1, -1, },
	{1, 3, -1, -1, -1, -1, -1, -1, },
	{16, -1, -1, -1, -1, -1, -1, -1, },
	{22, 28, 7, 30, 24, 1, 29, 1, },
	{25, 28, 7, 30, 1, -1, -1, -1, },
	{34, 27, 9, 23, -1, -1, -1, -1, },
	{9, 8, 9, -1, -1, -1, -1, -1, },
	{26, -1, -1, -1, -1, -1, -1, -1, },
	{18, -1, -1, -1, -1, -1, -1, -1, },
	{17, -1, -1, -1, -1, -1, -1, -1, },
	{20, -1, -1, -1, -1, -1, -1, -1, },
	{19, -1, -1, -1, -1, -1, -1, -1, },
	{11, 10, -1, -1, -1, -1, -1, -1, },
	{14, 11, 10, -1, -1, -1, -1, -1, },
	{21, 11, 10, -1, -1, -1, -1, -1, },
	{13, 12, -1, -1, -1, -1, -1, -1, },
	{15, 13, 12, -1, -1, -1, -1, -1, },
	{35, 13, 12, -1, -1, -1, -1, -1, },
	{34, -1, -1, -1, -1, -1, -1, -1, },
	{33, -1, -1, -1, -1, -1, -1, -1, },
	{28, 9, 30, -1, -1, -1, -1, -1, },
};
//    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
int LL1Table[14][23] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1,  1, -1, -1,  2, -1, -1, -1, -1, -1,  0, -1, -1,  3, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, },
	{-1, -1,  6, -1, -1, -1, -1, -1,  5, -1, -1,  5, -1, -1, -1, -1, -1,  5,  6, -1,  5, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  9, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1, 10, 10, -1, -1, },
	{-1, -1, -1, 13, 12, 15, 14, -1, -1, -1, -1, -1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, -1, -1, -1, -1, 16, 16, -1, -1, },
	{17, -1,  6,  6,  6,  6,  6, 18, -1,  6, -1, -1,  6, -1, -1, -1,  6, -1, -1, -1, -1, -1, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 19, -1, -1, -1, -1, 19, 19, -1, -1, },
	{ 6, 20,  6,  6,  6,  6,  6,  6, -1,  6, -1, -1,  6, -1, -1, -1,  6, -1, -1, -1, -1, 21, -1, },
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1, 23, 22, -1, -1, },
};

bool isTerminal(int symbol) {
    return symbol >= 14;
}

bool isNonTerminal(int symbol) {
    return symbol < 14;
}


#include <memory>
#include <algorithm>

#define FOR_SUBMISSION 1



class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::string sourceCode;
    int getTag(const std::string&);
    std::string convert(int);
    void error_handler(std::string&, int top);

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

    std::string word = lexer.GetNextToken().lexeme;
    std::vector<int> stack;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> tree;

    stack.push_back(_eof);
    stack.push_back(_program);
    nodes.push_back(std::make_shared<Node>(Node(convert(_program))));
    while (true) {
        if (stack.back() == _eof && word == "$") { /* accept */
            break;
        } else if (isTerminal(stack.back()) || word == "$") { /* terminal */
            if (stack.back() == getTag(word)) {
                stack.pop_back();
                nodes.pop_back();
                word = lexer.GetNextToken().lexeme;
            } else {
                throw std::runtime_error("Syntax error");
            }
        } else { /* non-terminal */
            int rule = LL1Table[stack.back()][getTag(word) - NONTERMINAL_COUNT];
            if (rule == -1) { 
#if FOR_SUBMISSION
                error_handler(word, stack.back());
                rule = LL1Table[stack.back()][getTag(word) - NONTERMINAL_COUNT];
                lexer.unreadToken();
                goto skip;
#else
                lexer.reportError("Syntax error");
#endif
            } else {
            skip:
                std::shared_ptr<Node> par = nodes.back();
                std::string parState = convert(stack.back());
                stack.pop_back();
                nodes.pop_back();
                int rulesize = 0;
                while (rulesize < MAX_RULE_LENGTH && rulesTable[rule][rulesize] != -1) {
                    rulesize++;
                }
                std::vector<std::shared_ptr<Node>> children;
                for (int i = rulesize - 1; i >= 0; i--) {
                    std::shared_ptr<Node> node = std::make_shared<Node>(convert(rulesTable[rule][i]));
                    children.push_back(node);
                    if (rulesTable[rule][i] != _epsilon){
                        stack.push_back(rulesTable[rule][i]);
                        nodes.push_back(node);
                    }
                }
                reverse(children.begin(), children.end());
                par->setChildren(children);
                tree.push_back(par);
            }
        }
    }
    return tree;
}

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
/**
 * @brief Represents a sequence of characters.
 * 
 * The std::string class is used to store and manipulate strings in C++.
 * It provides various member functions for string operations such as concatenation, substring extraction, and comparison.
 * 
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

/**
 * Handles errors in the parser.
 * 
 * @param word The current word being processed.
 * @param top The top of the stack.
 */
void Parser::error_handler(std::string& word, int top) {

    {
        std::cout << "语法错误,第" << lexer.GetLineAndColumn().first - 1 << "行,缺少\"";
    }
    if (top == _program) {
        word = "$";
    } else if (top == _stmt){
        word = "}";
    } else if (top == _compoundstmt) {
        word = "}";
    } else if (top == _stmts) {
        word = "}";
    } else if (top == _ifstmt) {
        word = "}";
    } else if (top == _whilestmt) {
        word = "}";
    } else if (top == _assgstmt) {
        word = "}";
    } else if (top == _boolexpr) {
        word = ")";
    } else if (top == _boolop) {
        word = "(";
    } else if (top == _arithexpr) {
        word = ";";
    } else if (top == _arithexprprime) {
        word = ";";
    } else if (top == _multexpr) {
        word = ";";
    } else if (top == _multexprprime) {
        word = ";";
    } else if (top == _simpleexpr) {
        word = ";";
    } else {
        lexer.reportError("Unknown token");
    }
    std::cout << word << "\"" << std::endl;
}


void read_prog(std::string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

void Analysis()
{
	std::string _prog, prog;
	read_prog(_prog);
    _prog += " $";
    for (int i = 0; i < (int)_prog.length(); i++) {
        if (i == _prog.length() || !(_prog[i] == '\n' && _prog[i + 1] == '\n')) {
            prog.push_back(_prog[i]);
        }
    }

    Parser parser;
    parser.SetSourceCode(prog);
    std::vector<std::shared_ptr<Node>> tree = parser.Parse();
    std::shared_ptr<Node> root = tree[0];
    root->PrintTree(0, true, tree);
    
}

int main () {
    Analysis();
    return 0;
}