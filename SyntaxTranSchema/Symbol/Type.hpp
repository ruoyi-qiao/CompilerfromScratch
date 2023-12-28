#include "../Lexer/Word.hpp" 
#include "../Lexer/Tag.hpp"   

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

// Initialization of static members
Type Type::Int("int", Tag::KW_INT, 4);
Type Type::Real("real", Tag::KW_REAL, 8);
Type Type::Undef("undef", Tag::KW_UNDEF, 0);
    