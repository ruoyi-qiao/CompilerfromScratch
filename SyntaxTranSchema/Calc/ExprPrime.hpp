#pragma once

#include "Expr.hpp"
#include "../Lexer/Word.hpp"

class ExprPrime : public Expr {

public:
    int opcode;

    ExprPrime() : opcode(0) {}
    ExprPrime(Token tok, Type p, int ival, int opcode) : Expr(tok, p, ival), opcode(opcode) {}
    ExprPrime(Expr& expr, const int& opcode) : Expr(expr), opcode(opcode) {}


    std::string toString() {
        return op.toString();
    }
};

Expr yield (Expr &expr, const ExprPrime& e) {
    Expr result;
    assert(expr.type == e.type);
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
            result = Expr(e.op, e.type, expr.IdValue.ival / e.IdValue.ival);
            break;
        default:
            throw std::runtime_error("Invalid opcode");
    }
    return result;
}