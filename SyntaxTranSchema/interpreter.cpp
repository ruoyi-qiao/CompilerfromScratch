#include <string>
#include <string>
#include <cctype> 
#include <vector>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <sstream>

class Token {
public:
    int tag;
    
    Token() : tag(-1) {}

    Token(int t) : tag(t) {}
    
    std::string toString() {
        return std::to_string(static_cast<int>(tag));
    }
};

namespace Tag {
    static const int 
        // keywords
        KW_UNDEF = 0 + (1 << 8),
        KW_INT = 1 + (1 << 8),
        KW_REAL = 2 + (1 << 8),
        KW_IF = 3 + (1 << 8),
        KW_THEN = 4 + (1 << 8),
        KW_ELSE = 5 + (1 << 8),
        // operators
        OP_ASSIGN = 6 + (1 << 9),
        OP_EQ = 7 + (1 << 9),
        OP_LT = 8 + (1 << 9),
        OP_GT = 9 + (1 << 9),
        OP_LE = 10 + (1 << 9),
        OP_GE = 11 + (1 << 9),
        OP_PLUS = 12 + (1 << 9),
        OP_MINUS = 13 + (1 << 9),
        OP_TIMES = 14 + (1 << 9),
        OP_DIVIDE = 15 + (1 << 9),

        // punctuation
        LPAREN = 16 + (1 << 10),
        RPAREN = 17 + (1 << 10),
        LBRACE = 18 + (1 << 10),
        RBRACE = 19 + (1 << 10),
        ID = 20 + (1 << 10),
        NUM = 21 + (1 << 10),
        REAL = 22 + (1 << 10),
        SEMICOLON = 23 + (1 << 10),
        END = 24 + (1 << 10);
};

#include <map>

// reverse lookup

std::map<int, std::string> tagToString = {
    {1 + (1 << 8), "KW_INT"},
    {2 + (1 << 8), "KW_REAL"},
    {3 + (1 << 8), "KW_IF"},
    {4 + (1 << 8), "KW_THEN"},
    {5 + (1 << 8), "KW_ELSE"},
    {6 + (1 << 9), "OP_ASSIGN"},
    {7 + (1 << 9), "OP_EQ"},
    {8 + (1 << 9), "OP_LT"},
    {9 + (1 << 9), "OP_GT"},
    {10 + (1 << 9), "OP_LE"},
    {11 + (1 << 9), "OP_GE"},
    {12 + (1 << 9), "OP_PLUS"},
    {13 + (1 << 9), "OP_MINUS"},
    {14 + (1 << 9), "OP_TIMES"},
    {15 + (1 << 9), "OP_DIVIDE"},
    {16 + (1 << 10), "LPAREN"},
    {17 + (1 << 10), "RPAREN"},
    {18 + (1 << 10), "LBRACE"},
    {19 + (1 << 10), "RBRACE"},
    {20 + (1 << 10), "ID"},
    {21 + (1 << 10), "NUM"},
    {22 + (1 << 10), "REAL"},
    {23 + (1 << 10), "SEMICOLON"},
    {24 + (1 << 10), "END"}
};

#include <iostream>
class Word : public Token {
public:
    std::string lexeme;

    Word() : Token(0) {} 
    Word(const std::string& s, int tag) : Token(tag), lexeme(s) {}
    Word(const Word& w);

    Word operator=(const Word& w) {
        this->lexeme = w.lexeme;
        this->tag = w.tag;
        return *this;
    }

    bool operator<(const Word& w) const {
        return lexeme < w.lexeme;
    }

    std::string toString() const {
        return lexeme;
    }
};


namespace std {
    template<>
    struct hash<Word> {
        size_t operator()(const Word& w) const {
            int hash = 0;
            hash += std::hash<std::string>()(w.lexeme);
            hash += std::hash<int>()(w.tag);
            return hash;
        }
    };
}

bool operator==(const Word& w1, const Word& w2) {
    return w1.lexeme == w2.lexeme && w1.tag == w2.tag;
}

bool operator!=(const Word& w1, const Word& w2) {
    return !(w1 == w2);
}

Word::Word(const Word& w) : Token(w.tag), lexeme(w.lexeme) {}

class Type : public Word {
public:
    int width;

    Type() : Word(), width(0) {}
    Type(const std::string& s, int tag, int w) : Word(s, tag), width(w) {}

    static Type Int;
    static Type Real;
    static Type Undef;

    std::string toString() const {
        return lexeme;
    }
};

Type Type::Int("int", Tag::KW_INT, 4);
Type Type::Real("real", Tag::KW_REAL, 4);
Type Type::Undef("undef", Tag::KW_UNDEF, 0);

class Node {
public:
    static int labels;
    int newlabel() { return ++labels; }

    std::vector<std::shared_ptr<Node>> children;
};

int Node::labels = 0;

class Expr : public Node {
public:
    Token op;
    Type type;

    union Value 
    {
        int ival;
        float fval;
    } IdValue;    

    Expr(){}

    Expr(Token tok, Type p, int ival) : op(tok), type(p) {
        IdValue.ival = ival;
    }

    Expr(Token tok, Type p, float fval) : op(tok), type(p) {
        IdValue.fval = fval;
    }
    
    Expr(const Expr& expr) : op(expr.op), type(expr.type) {
        if (expr.type == Type::Int) {
            IdValue.ival = expr.IdValue.ival;
        } else {
            IdValue.fval = expr.IdValue.fval;
        }
    }

    Expr operator = (const Expr& expr) {
        this->op = expr.op;
        this->type = expr.type;
        if (expr.type == Type::Int) {
            this->IdValue.ival = expr.IdValue.ival;
        } else {
            this->IdValue.fval = expr.IdValue.fval;
        }
        return *this;
    }


    std::string toString() {
        return op.toString();
    }
};

class ExprPrime : public Expr {

public:
    int opcode;
    static bool arith_error_flag;

    ExprPrime() : opcode(0) {
        this->type = Type::Undef;
    }

    ExprPrime(Token tok, Type p, int ival, int opcode) : Expr(tok, p, ival), opcode(opcode) {}
    ExprPrime(Expr expr, int opcode) : Expr(expr), opcode(opcode) {}


    std::string toString() {
        return op.toString();
    }
};

void yield (Expr &expr, ExprPrime e) {
    Expr result;
    if (e.type == Type::Undef) {
        assert(expr.type == Type::Int || expr.type == Type::Real);
        return;
    }
    if (e.type != expr.type) {
        if (e.type == Type::Int) {
            e.IdValue.fval = static_cast<float>(e.IdValue.ival);
            e.type = Type::Real;
        } else {
            expr.IdValue.fval = static_cast<float>(expr.IdValue.ival);
            expr.type = Type::Real;
        }
    }
    if (expr.type == Type::Int) {

        switch (e.opcode) {
            case Tag::OP_PLUS:
                result = Expr(e.op, e.type, expr.IdValue.ival + e.IdValue.ival);
                break;
            case Tag::OP_MINUS:
                result = Expr(e.op, e.type, expr.IdValue.ival - e.IdValue.ival);
                break;
            case Tag::OP_TIMES:
                result = Expr(e.op, e.type, expr.IdValue.ival * e.IdValue.ival);
                break;
            case Tag::OP_DIVIDE:
                if (e.IdValue.ival == 0) {
                    ExprPrime::arith_error_flag = true;
                }
                result = Expr(e.op, e.type, expr.IdValue.ival / e.IdValue.ival);
                break;
            default:
                throw std::runtime_error("Invalid opcode");
        }
    } else if (expr.type == Type::Real) {
        switch (e.opcode) {
            case Tag::OP_PLUS:
                result = Expr(e.op, e.type, expr.IdValue.fval + e.IdValue.fval);
                break;
            case Tag::OP_MINUS:
                result = Expr(e.op, e.type, expr.IdValue.fval - e.IdValue.fval);
                break;
            case Tag::OP_TIMES:
                result = Expr(e.op, e.type, expr.IdValue.fval * e.IdValue.fval);
                break;
            case Tag::OP_DIVIDE:
                if (e.IdValue.fval == 0) {
                    ExprPrime::arith_error_flag = true;
                }
                result = Expr(e.op, e.type, expr.IdValue.fval / e.IdValue.fval);
                break;
            default:
                throw std::runtime_error("Invalid opcode");
        }
    }

    // if (result.type == Type::Int) {
    //     std::cout << result.op.toString() << " = " << result.IdValue.ival << std::endl;
    // } else if (result.type == Type::Real) {
    //     std::cout << result.op.toString() << " = " << result.IdValue.fval << std::endl;
    // }

    expr = result;
}


bool ExprPrime::arith_error_flag = false;


class Id : public Expr {
public:
    int offset;  
    
    Id() : Expr(), offset(0) {}
    
    Id(Token id, Type p, int b = 0, int i = 0) : Expr(id, p, i), offset(b) {}
    
    Id(Token id, Type p, int b = 0, float f = 0.0f) : Expr(id, p, f), offset(b) {}
    
    std::string toString() {
        
        if (type == Type::Int) {
            return this->op.toString() + "[" + std::to_string(this->offset) + "] = " + std::to_string(IdValue.ival);
        } else if (type == Type::Real) {
            return this->op.toString() + "[" + std::to_string(this->offset) + "] = " + std::to_string(IdValue.fval);
        }

        return "unknown id";
    }
};


class Boolop {
public:
    Boolop() = default;
    Boolop(int tag) : tag(tag) {}
    int tag;

    bool yield(Expr expr1, Expr expr2) {
        if (expr1.type != expr2.type) {
            if (expr1.type == Type::Int) {
                expr1.IdValue.fval = static_cast<float>(expr1.IdValue.ival);
                expr1.type = Type::Real;
            } else {
                expr2.IdValue.fval = static_cast<float>(expr2.IdValue.ival);
                expr2.type = Type::Real;
            }
        } 
        if (expr1.type == Type::Int) {
            switch (tag) {
                case Tag::OP_EQ:
                    return expr1.IdValue.ival == expr2.IdValue.ival;
                case Tag::OP_LT:
                    return expr1.IdValue.ival < expr2.IdValue.ival;
                case Tag::OP_GT:
                    return expr1.IdValue.ival > expr2.IdValue.ival;
                case Tag::OP_LE:
                    return expr1.IdValue.ival <= expr2.IdValue.ival;
                case Tag::OP_GE:
                    return expr1.IdValue.ival >= expr2.IdValue.ival;
                default:
                    throw std::runtime_error("unknown Boolop tag");
            }
        } else if (expr1.type == Type::Real) {
            switch (tag) {
                case Tag::OP_EQ:
                    return expr1.IdValue.fval == expr2.IdValue.fval;
                case Tag::OP_LT:
                    return expr1.IdValue.fval < expr2.IdValue.fval;
                case Tag::OP_GT:
                    return expr1.IdValue.fval > expr2.IdValue.fval;
                case Tag::OP_LE:
                    return expr1.IdValue.fval <= expr2.IdValue.fval;
                case Tag::OP_GE:
                    return expr1.IdValue.fval >= expr2.IdValue.fval;
                default:
                    throw std::runtime_error("unknown Boolop tag");
            }
        } else {
            throw std::runtime_error("unknown Boolop tag");
        }
    }

    std::string toString() {
        switch (tag) {
            case Tag::OP_EQ:
                return "==";
            case Tag::OP_LT:
                return "<";
            case Tag::OP_GT:
                return ">";
            case Tag::OP_LE:
                return "<=";
            case Tag::OP_GE:
                return ">=";
            default:
                throw std::runtime_error("unknown Boolop tag");
                return "unknown";
        }
    }
};


class Env {
private:
    std::unordered_map<Word, Id> table;
    Env* prev;

public:
    Env(Env* n) : prev(n) {}

    void put(const Word& w, const Id& i) {
        table[w] = i;
    }

    Id* get(const Word& w) {
        for (Env* e = this; e != nullptr; e = e->prev) {
            auto it = e->table.find(w);
            if (it != e->table.end()) {
                return &(it->second);
            }
        }
        return nullptr;
    }

    void print() {
        for (auto& it : table) {
            std::cout << it.first.toString() << " " << it.second.toString() << std::endl;
        }
    }

    std::map<Word, Id> getEnv() {
        std::map<Word, Id> res;
        for (auto& it : table) {
            res[it.first] = it.second;
        }
        return res;
    }
};

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
    if (lexeme == "int") {
        return Tag::KW_INT;
    } else if (lexeme == "real") {
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
    } else if (lexeme == "<=") {
        return Tag::OP_LE;
    } else if (lexeme == ">=") {
        return Tag::OP_GE;
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

    assert(false);
    return -1;

}

void Lexer::printWords() {
    for (auto it = words.begin(); it != words.end(); it++) {
        std::cout << "[" << it->first << ", " << (it->second.tag & ((1 << 8) - 1)) << "]" << std::endl;
    }
}


class Parser {
private:
    Lexer lexer;
    Word look;
    Env* top = nullptr;
    int used = 0;
    bool isExecuting = true, error_flag = false;

    void move() {
        look = lexer.consumeToken();
    }

    void match(int t) {
        // std::cout << "matching " << tagToString[(t)] << " with " << tagToString[(look.tag)] << std::endl;
        if (look.tag == t) {
            move();
        } else {
            lexer.reportError("syntax error on line " + std::to_string(lexer.GetLineAndColumn().first) + " column " + std::to_string(lexer.GetLineAndColumn().second));
        }
    }

    void type_error(Type t1, Type t2, std::string s1, std::string s2) {
        error_flag = true;
        int line = lexer.GetLineAndColumn().first;
        std::cout << "error message:line " << line << "," + s1 + "num can not be translated into " + s2 + " type" << std::endl;
    }

    void arith_error() {
        error_flag = true;
        ExprPrime::arith_error_flag = false;
        int line = lexer.GetLineAndColumn().first;
        std::cout << "error message:line " << line << ",division by zero";
    }

    void decls();
    void decl();
    void stmt();
    void compoundStmt();
    void stmts();
    void ifstmt();
    void assgnstmt();
    bool boolexpr();
    Boolop boolop();     //  op = < | > | <= | >= | ==
    Expr arithexpr();  //  
    ExprPrime arithexprprime();
    Expr multexpr();
    ExprPrime multexprprime();
    Expr simpleexpr();



public:
    Parser(Lexer& l) : lexer(l),  look(lexer.consumeToken()) {
    }

    std::map<Word, Id> program(int &status) {
        // top level env
        top = new Env(top);

        decls();
        compoundStmt();

        status = error_flag ? 1 : 0;
        return top->getEnv();
    }
};

void Parser::decls() {
    if (look.tag == Tag::KW_INT || look.tag == Tag::KW_REAL) {
        decl();
        match(Tag::SEMICOLON);
        decls();
    } else {
        // epsilon
    }
}

void Parser::decl() {
    if (look.tag == Tag::KW_INT) {
        Word  id, num;
        Type type = Type::Int;

        match(Tag::KW_INT); id = look;
        match(Tag::ID);
        match(Tag::OP_ASSIGN); num = look;
        move();

        int ival = std::stoi(num.lexeme);

        Id* idPtr = new Id(id, type, used, ival);
        top->put(id, *idPtr);
        used += type.width;

        if (num.tag == Tag::REAL) {
            type_error(Type::Real, Type::Int, "real", "int");
        }
    
    } else if (look.tag == Tag::KW_REAL) {
        Word id, num;
        Type type = Type::Real;

        match(Tag::KW_REAL); id = look;
        match(Tag::ID);
        match(Tag::OP_ASSIGN); num = look;
        move();

        float fval = std::stof(num.lexeme);

        Id* idPtr = new Id(id, type, used, fval);
        top->put(id, *idPtr);
        used += type.width;
        
        if (num.tag == Tag::NUM) {
            type_error(Type::Int, Type::Real, "int", "real");
        } 

    } else {
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
    }
}

void Parser::stmt() {
    if (look.tag == Tag::ID) {
        assgnstmt();
    } else if (look.tag == Tag::KW_IF) {
        ifstmt();
    } else if (look.tag == Tag::LBRACE) {
        compoundStmt();
    } else {
        
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
    }
}

void Parser::compoundStmt() {
    // new nested env
    Env *oldTop = top;
    top = new Env(oldTop);

    match(Tag::LBRACE);
    stmts();
    match(Tag::RBRACE);

    top = oldTop;
}

void Parser::stmts() {
    if (look.tag == Tag::ID || look.tag == Tag::KW_IF || look.tag == Tag::LBRACE) {
        stmt();
        stmts();
    } else {
        // epsilon
    }
}

void Parser::ifstmt() {
    match(Tag::KW_IF);
    match(Tag::LPAREN);
    bool if_res = boolexpr();
    match(Tag::RPAREN);
    match(Tag::KW_THEN);

    bool prev_isExecuting = isExecuting;
    if (!if_res) isExecuting = false;

    stmt();
    
    isExecuting = prev_isExecuting;
    
    match(Tag::KW_ELSE);
    
    if (if_res) isExecuting = false;
    
    stmt();

    isExecuting = prev_isExecuting;
}

void Parser::assgnstmt() {
    Word id = look;
    match(Tag::ID);
    match(Tag::OP_ASSIGN);
    Expr expr = arithexpr();
    match(Tag::SEMICOLON);

    if (isExecuting) {
        Id* idPtr = top->get(id);
        if (idPtr == nullptr) {
            int line = lexer.GetLineAndColumn().first;
            int column = lexer.GetLineAndColumn().second;
            lexer.reportError("Identifier undefined on line " + std::to_string(line) + " column " + std::to_string(column));
        }

        if (idPtr->type != expr.type) {
            if (expr.type == Type::Int) {
                expr.IdValue.fval = static_cast<float>(expr.IdValue.ival);
                expr.type = Type::Real;
            } else {
                expr.IdValue.ival = static_cast<int> (expr.IdValue.fval);
                expr.type = Type::Int;
            }
        }

        // if (idPtr->type == Type::Int) {
        //     std::cout << idPtr->op.toString() << "[" << idPtr->offset << "] = " << idPtr->IdValue.ival << " -> " << expr.IdValue.ival << std::endl;
        // } else {
        //     std::cout << idPtr->op.toString() << "[" << idPtr->offset << "] = " << idPtr->IdValue.fval << " -> " << expr.IdValue.fval << std::endl;
        // }

        idPtr->IdValue = expr.IdValue;
    }
}

bool Parser::boolexpr() {

    Expr leftExpr = arithexpr();
    Boolop op = boolop();
    Expr rightExpr = arithexpr();

    return op.yield(leftExpr, rightExpr);
}

Boolop Parser::boolop() {
    Boolop op = Boolop(look.tag);
    if (look.tag == Tag::OP_EQ) {
        match(Tag::OP_EQ);
    } else if (look.tag == Tag::OP_LT) {
        match(Tag::OP_LT);
    } else if (look.tag == Tag::OP_GT) {
        match(Tag::OP_GT);
    } else if (look.tag == Tag::OP_LE) {
        match(Tag::OP_LE);
    } else if (look.tag == Tag::OP_GE) {
        match(Tag::OP_GE);
    } else {
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        lexer.reportError("Unknown bool operator on line " + std::to_string(line) + " column " + std::to_string(column));
    }
    return op;
}
Expr Parser::arithexpr() {
    Expr expr = multexpr();
    ExprPrime exprPrime = arithexprprime();
    yield(expr, exprPrime);

    if (ExprPrime::arith_error_flag) arith_error();

    return expr;
}
ExprPrime Parser::arithexprprime() {

    ExprPrime res;

    if (look.tag == Tag::OP_PLUS) {
        match(Tag::OP_PLUS);
        Expr expr = multexpr();
        ExprPrime exprPrime = arithexprprime();  
        yield(expr, exprPrime);
        res = ExprPrime(expr, Tag::OP_PLUS);
        assert(res.type != Type::Undef);
    } else if (look.tag == Tag::OP_MINUS) {
        match(Tag::OP_MINUS);
        Expr expr = multexpr();
        ExprPrime exprPrime = arithexprprime();
        yield(expr, exprPrime);
        if (ExprPrime::arith_error_flag) arith_error();
        res = ExprPrime(expr, Tag::OP_MINUS);
    } else {
        // epsilon
        res = ExprPrime();
    }
    return res;
}

Expr Parser::multexpr() {
    Expr expr = simpleexpr();
    ExprPrime exprPrime = multexprprime();
    // output the type of expr and exprPrime
    yield(expr, exprPrime);
    if (ExprPrime::arith_error_flag) arith_error();
    return expr;
}

ExprPrime Parser::multexprprime() {

    ExprPrime res;

    if (look.tag == Tag::OP_TIMES) {
        match(Tag::OP_TIMES);
        Expr expr =  simpleexpr();
        ExprPrime exprPrime = multexprprime();
        yield(expr, exprPrime);
        if (ExprPrime::arith_error_flag) arith_error();
        res = ExprPrime(expr, Tag::OP_TIMES);
    } else if (look.tag == Tag::OP_DIVIDE) {
        match(Tag::OP_DIVIDE);
        Expr expr = simpleexpr();
        ExprPrime exprPrime = multexprprime();
        yield(expr, exprPrime);
        if (ExprPrime::arith_error_flag) arith_error();
    if (ExprPrime::arith_error_flag) arith_error();
        res = ExprPrime(expr, Tag::OP_DIVIDE);

        assert(res.type != Type::Undef);
    } else {
        // epsilon
        res = ExprPrime();
    }
    
    return res;
}

Expr Parser::simpleexpr() {
    Word old = look;
    if (look.tag == Tag::ID) {
        match(Tag::ID);
        Id* idPtr = top->get(old);
        assert(idPtr != nullptr);
        if (idPtr->type == Type::Int) 
            return Expr(old, idPtr->type, idPtr->IdValue.ival);
        else 
            return Expr(old, idPtr->type, idPtr->IdValue.fval);
    } else if (look.tag == Tag::NUM) {
        match(Tag::NUM);
        return Expr(old, Type::Int, std::stoi(old.lexeme));
    } else if (look.tag == Tag::REAL) {
        match(Tag::REAL);
        return Expr(old, Type::Real, std::stof(old.lexeme));
    } else if (look.tag == Tag::LPAREN) {
        match(Tag::LPAREN);
        Expr eval = arithexpr();
        match(Tag::RPAREN);
        return eval;
    } else {
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
        return Expr();
    }
}

/**********************************/
void read_prog(std::string& prog)
{
    char c;
    while(scanf("%c",&c)!=EOF){
        prog += c;
    }
}
/**********************************/

void Analysis()
{
	std::string _prog, prog;
	read_prog(_prog);
    _prog += " $";
    for (int i = 0; i < (int)_prog.length(); i++) {
        if (i == static_cast<int>(_prog.length()) || !(_prog[i] == '\n' && _prog[i + 1] == '\n')) {
            prog.push_back(_prog[i]);
        }
    }

    int exec_status = 0;

    Lexer lexer;
    lexer.SetSourceCode(prog);
    Parser parser(lexer);
    std::map<Word, Id> symbolTable = parser.program(exec_status);
    
    if (exec_status == 0) {
        for (std::map<Word, Id>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++) {
            if (it->second.type == Type::Int) {
                std::cout << it->first.toString() << ": " << it->second.IdValue.ival;
            } else {
                std::cout << it->first.toString() << ": " << it->second.IdValue.fval;
            }
            if (std::next(it) != symbolTable.end()) {
                std::cout << std::endl;
            }
        }
    }
}

int main () {
    
    Analysis();
    
    return 0;
}