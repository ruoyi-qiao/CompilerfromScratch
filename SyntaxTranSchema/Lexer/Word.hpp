#pragma once

#include "Token.hpp"
#include "Tag.hpp"

class Word : public Token {
public:
    std::string lexeme = "";

    Word() : Token(0), lexeme("") {} 
    Word(const std::string& s, int tag) : Token(tag), lexeme(s) {}
    Word(const Word& w);

    Word operator=(const Word& w) {
        this->lexeme = w.lexeme;
        this->tag = w.tag;
        return *this;
    }

    std::string toString() const {
        return lexeme;
    }
};

// hash function for Word
namespace std {
    template<>
    struct hash<Word> {
        size_t operator()(const Word& w) const {
            int hash = 0;
            hash += std::hash<std::string>()(w.lexeme);
            hash += std::hash<int>()(w.tag);
            return hash;
        }
    };
}

bool operator==(const Word& w1, const Word& w2) {
    return w1.lexeme == w2.lexeme && w1.tag == w2.tag;
}

bool operator!=(const Word& w1, const Word& w2) {
    return !(w1 == w2);
}

Word::Word(const Word& w) : Token(w.tag), lexeme(w.lexeme) {}
