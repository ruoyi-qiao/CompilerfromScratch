#pragma once

#include "Expr.hpp"
#include "../Lexer/Word.hpp"

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
            // std::cout << "e.IdValue.ival = " << e.IdValue.ival << std::endl;
            e.IdValue.fval = static_cast<float>(e.IdValue.ival);
            e.type = Type::Real;
        } else {
            expr.IdValue.fval = static_cast<float>(expr.IdValue.ival);
            expr.type = Type::Real;
        }
    }
    std::cout << "here!" << std::endl;
    if (expr.type == Type::Int) {
        std::cout << "expr.IdValue.ival = " << expr.IdValue.ival << "  e.IdValue.ival = " << e.IdValue.ival << std::endl;

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
        std::cout << "expr.IdValue.fval = " << expr.IdValue.fval << "  e.IdValue.fval = " << e.IdValue.fval << std::endl;
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

    expr = result;
}


bool ExprPrime::arith_error_flag = false;