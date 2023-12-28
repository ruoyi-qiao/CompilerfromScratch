#include <string>
#include <stdexcept>
#include "../Lexer/Tag.hpp"
#include "Expr.hpp"

class Boolop {
public:
    Boolop() = default;
    Boolop(int tag) : tag(tag) {}
    int tag;

    bool yield(Expr expr1, Expr expr2) {
        if (expr1.type != expr2.type) {
            throw std::runtime_error("type mismatch");
        }
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