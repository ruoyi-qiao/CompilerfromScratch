#pragma once

#include "Token.hpp" 
#include "Tag.hpp" 

class Num : public Token {
public:
    const int value;

    Num(int v) : Token(Tag::NUM), value(v) {}

    std::string toString() {
        return std::to_string(value);
    }
};
