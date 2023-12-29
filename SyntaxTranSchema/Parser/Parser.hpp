
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
    bool isExecuting = true, error_flag = false;

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

    void type_error(Type t1, Type t2, std::string s1, std::string s2) {
        error_flag = true;
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        std::cout << "error message:line " << line << "," + s1 + "num can not be translated into " + s2 + " type" << std::endl;
    }

    void arith_error() {
        error_flag = true;
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        std::cout << "error message:line " << line << ",division by zero" << std::endl;
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

    void program() {
        // top level env
        top = new Env(top);

        decls();
        compoundStmt();

        if (!error_flag)
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
            int line = lexer.GetLineAndColumn().first;
            int column = lexer.GetLineAndColumn().second;
            lexer.reportError("Identifier undefined on line " + std::to_string(line) + " column " + std::to_string(column));
        }

        std::cout << "before update: " << idPtr->toString() << std::endl;
        if (idPtr->type != expr.type) {
            if (expr.type == Type::Int) {
                expr.IdValue.fval = static_cast<float>(expr.IdValue.ival);
                expr.type = Type::Real;
            } else {
                expr.IdValue.ival = static_cast<int> (expr.IdValue.fval);
                expr.type = Type::Int;
            }
        }
        idPtr->IdValue = expr.IdValue;

        if (idPtr->type == Type::Int) {
            std::cout << "update value: " << idPtr->IdValue.ival << std::endl;
        } else {
            std::cout << "update value: " << idPtr->IdValue.fval << std::endl;
        }
    }
}

bool Parser::boolexpr() {

    Expr leftExpr = arithexpr();
    Boolop op = boolop();
    std::cout << "op: " << op.toString() << std::endl;
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
    std::cout << "expr1: " << expr.type.toString() << " exprPrime1: " << exprPrime.type.toString() << std::endl;
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
        std::cout << "?\n";
        std::cout << "exprPrime.type: " << expr.type.toString() << std::endl;
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
    std::cout << "exprPrime.ival: " << res.IdValue.ival << std::endl;
    return res;
}

Expr Parser::multexpr() {
    Expr expr = simpleexpr();
    ExprPrime exprPrime = multexprprime();
    // output the type of expr and exprPrime
    std::cout << "expr2: " << expr.type.toString() << " exprPrime2: " << exprPrime.type.toString() << std::endl;
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
        // output red color
        std::cout << "\033[1;31m" << "ID" << "\033[0m" << std::endl;
        match(Tag::ID);
        Id* idPtr = top->get(old);
        assert(idPtr != nullptr);
        if (idPtr->type == Type::Int) 
            return Expr(old, idPtr->type, idPtr->IdValue.ival);
        else 
            return Expr(old, idPtr->type, idPtr->IdValue.fval);
    } else if (look.tag == Tag::NUM) {
        std::cout << "\033[1;31m" << "NUM" << "\033[0m" << std::endl;
        match(Tag::NUM);
        return Expr(old, Type::Int, std::stoi(old.lexeme));
    } else if (look.tag == Tag::REAL) {
        std::cout << "\033[1;31m" << "REAL" << "\033[0m" << std::endl;
        match(Tag::REAL);
        return Expr(old, Type::Real, std::stof(old.lexeme));
    } else if (look.tag == Tag::LPAREN) {
        std::cout << "\033[1;31m" << "(VAL)" << "\033[0m" << std::endl;
        match(Tag::LPAREN);
        Expr eval = arithexpr();
        match(Tag::RPAREN);
        return eval;
    } else {
        int line = lexer.GetLineAndColumn().first;
        int column = lexer.GetLineAndColumn().second;
        lexer.reportError("syntax error on line " + std::to_string(line) + " column " + std::to_string(column));
    }
}