
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include "../Lexer/Lexer.hpp"
#include "../Symbol/Env.hpp"
#include "../Lexer/Num.hpp"
#include "../Lexer/Real.hpp"
#include "../Calc/ExprPrime.hpp"
#include "../Calc/Boolop.hpp"


class Parser {
private:
    Lexer lexer;
    Word look;
    Env* top = nullptr;
    int used = 0;
    bool isExecuting = false;

    void move() {
        look = lexer.consumeToken();
    }

    void match(int t) {
        std::cout << "matching " << tagToString[(t)] << " with " << tagToString[(look.tag)] << std::endl;
        if (look.tag == t) {
            move();
        } else {
            lexer.reportError("syntax error on line " + std::to_string(lexer.GetLineAndColumn().first) + " column " + std::to_string(lexer.GetLineAndColumn().second));
        }
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

    // Stmt* block();
    // void decls();
    // Type* type();
    // Type* dims(Type* p);
    // Stmt* stmts();
    // Stmt* stmt();
    // Stmt* assign();
    // Expr* bool_();
    // Expr* join();
    // Expr* equality();
    // Expr* rel();
    // Expr* expr();
    // Expr* term();
    // Expr* unary();
    // Expr* factor();
    // Access* offset(Id* a);

public:
    Parser(Lexer& l) : lexer(l),  look(lexer.consumeToken()) {
    }

    void program() {
        // top level env
        top = new Env(top);

        decls();
        compoundStmt();

        top->print();
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
        match(Tag::NUM);

        assert(num.tag == Tag::NUM);
        int ival = std::stoi(num.lexeme);

        Id* idPtr = new Id(id, type, used, ival);
        top->put(id, *idPtr);
        used += type.width;
    
    } else if (look.tag == Tag::KW_REAL) {
        Word id, num;
        Type type = Type::Real;

        match(Tag::KW_REAL); id = look;
        match(Tag::ID);
        match(Tag::OP_ASSIGN); num = look;
        match(Tag::REAL);

        assert(num.tag == Tag::REAL);
        float fval = std::stof(num.lexeme);

        Id* idPtr = new Id(id, type, used, fval);
        top->put(id, *idPtr);
        used += type.width;

    } else {
        auto [line, column] = lexer.GetLineAndColumn();
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
        auto [line, column] = lexer.GetLineAndColumn();
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
    
    if (!if_res) isExecuting = false;

    stmt();
    
    isExecuting = true;
    
    match(Tag::KW_ELSE);
    
    if (if_res) isExecuting = false;
    
    stmt();

    isExecuting = true;
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
            auto [line, column] = lexer.GetLineAndColumn();
            lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
        }
        idPtr->IdValue = expr.IdValue;

        std::cout << "update: " << idPtr->toString() << std::endl;
        if (idPtr->type == Type::Int) {
            std::cout << "value: " << idPtr->IdValue.ival << std::endl;
        } else {
            std::cout << "value: " << idPtr->IdValue.fval << std::endl;
        }
    }
}

bool Parser::boolexpr() {

    Expr leftExpr = arithexpr();
    Boolop op = boolop();
    Expr rightExpr = arithexpr();

    return op.yield(leftExpr, rightExpr);
}

Boolop Parser::boolop() {
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
        auto [line, column] = lexer.GetLineAndColumn();
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
    }
}
Expr Parser::arithexpr() {
    Expr expr = multexpr();
    ExprPrime exprPrime = arithexprprime();
    yield(expr, exprPrime);
    return expr;
}
ExprPrime Parser::arithexprprime() {

    ExprPrime exprPrime;

    if (look.tag == Tag::OP_PLUS) {
        match(Tag::OP_PLUS);
        Expr expr = multexpr();
        ExprPrime exprPrime = arithexprprime();
        yield(expr, exprPrime);
        exprPrime = ExprPrime(expr, Tag::OP_PLUS);
    } else if (look.tag == Tag::OP_MINUS) {
        match(Tag::OP_MINUS);
        Expr expr = multexpr();
        ExprPrime exprPrime = arithexprprime();
        yield(expr, exprPrime);
        exprPrime = ExprPrime(expr, Tag::OP_MINUS);
    } else {
        // epsilon
        exprPrime = ExprPrime();
    }

    return exprPrime;
}

Expr Parser::multexpr() {
    simpleexpr();
    multexprprime();
}

ExprPrime Parser::multexprprime() {

    ExprPrime exprPrime;

    if (look.tag == Tag::OP_TIMES) {
        match(Tag::OP_TIMES);
        Expr expr =  simpleexpr();
        ExprPrime exprPrime = multexprprime();
        yield(expr, exprPrime);
        exprPrime = ExprPrime(expr, Tag::OP_TIMES);
    } else if (look.tag == Tag::OP_DIVIDE) {
        match(Tag::OP_DIVIDE);
        Expr expr = simpleexpr();
        ExprPrime exprPrime = multexprprime();
        yield(expr, exprPrime);
        exprPrime = ExprPrime(expr, Tag::OP_DIVIDE);
    } else {
        // epsilon
        exprPrime = ExprPrime();
    }

    return exprPrime;
}

Expr Parser::simpleexpr() {
    Word old = look;
    if (look.tag == Tag::ID) {
        match(Tag::ID);
        return Id(old, Type::Int, 0, 0);
    } else if (look.tag == Tag::NUM) {
        match(Tag::NUM);
        return Expr(old, Type::Int, 0);
    } else if (look.tag == Tag::REAL) {
        match(Tag::REAL);
        return Expr(old, Type::Real, 0.0f);
    } else if (look.tag == Tag::LPAREN) {
        match(Tag::LPAREN);
        Expr eval = arithexpr();
        match(Tag::RPAREN);
        return eval;
    } else {
        auto [line, column] = lexer.GetLineAndColumn();
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
    }
}