#pragma once

#include "Token.hpp" 
#include "Tag.hpp"  

class Real : public Token {
public:
    const float value;

    Real(float v) : Token(Tag::REAL), value(v) {}

    std::string toString() {
        return std::to_string(value);
    }
};
