#pragma once

#include <iostream>
#include <memory>
#include "../Lexer/Lexer.hpp"

class Node {
public:
    static int labels;
    int newlabel() { return ++labels; }

    void emitlabel(int i) { std::cout << "L" << i << ":"; }

    void emit(const std::string& s) {
        std::cout << "\t" << s << std::endl;
    }

    std::vector<std::shared_ptr<Node>> children;
};

int Node::labels = 0;