#pragma once
#include <string>

class Token {
public:
    int tag;
    
    Token() : tag(-1) {}

    Token(int t) : tag(t) {}
    
    std::string toString() {
        return std::to_string(static_cast<int>(tag));
    }
};



