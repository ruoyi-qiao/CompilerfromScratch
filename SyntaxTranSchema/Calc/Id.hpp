#include "Expr.hpp"
#include "../Lexer/Word.hpp"

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

        return this->op.toString() + "[" + std::to_string(this->offset) + "] = " + std::to_string(IdValue.ival);

    }
};