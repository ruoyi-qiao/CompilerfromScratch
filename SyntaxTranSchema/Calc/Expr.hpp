#pragma once

#include "Node.hpp"   
#include "../Lexer/Word.hpp" 
#include "../Symbol/Type.hpp"   

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


    std::string toString() {
        return op.toString();
    }
};




