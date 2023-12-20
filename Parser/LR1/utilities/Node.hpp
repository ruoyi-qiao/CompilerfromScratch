#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "Token.hpp"


class Node {
private:
    int childCount;
    Token t;
    std::vector<std::shared_ptr<Node>> children;

public:
    Node();
    Node(const Token&);
    Node(const std::string&);
    Node(const Token&, const std::vector<std::shared_ptr<Node>>&);

    const Token& GetToken();
    std::vector<std::shared_ptr<Node>> GetChildren();
    void addChild(const std::shared_ptr<Node>&);
    void setChild(int, const std::shared_ptr<Node>&);
    void setChildren(const std::vector<std::shared_ptr<Node>>&);

    std::string ToString();
    void PrintTree(int, std::vector<std::shared_ptr<Node>>&);
};

#include "Node.hpp"

Node::Node(){
    t = {};
    childCount = 0;
    children = {};
}

Node::Node(const Token& token) {
    t = token;
    childCount = 0;
    children = {};
}

Node::Node(const std::string& lexeme) {
    t.lexeme = lexeme;
    childCount = 0;
    children = {};
}

Node::Node(const Token& token, const std::vector<std::shared_ptr<Node>>& children) {
    t = token;
    childCount = children.size();
    this->children = children;
}

const Token& Node::GetToken() {
    return t;
}

void Node::addChild(const std::shared_ptr<Node>& child) {
    children.push_back(child);
    childCount++;
}

void Node::setChild(int index, const std::shared_ptr<Node>& child) {
    children[index] = child;
}

void Node::setChildren(const std::vector<std::shared_ptr<Node>>& children) {
    this->children = children;
    childCount = children.size();
}

std::string Node::ToString(){
    return t.lexeme;
}

std::vector<std::shared_ptr<Node>> Node::GetChildren(){
    return children;
}

void Node::PrintTree(int depth, std::vector<std::shared_ptr<Node>>& tree){
    for (int i = 0; i < depth; i++) {
        std::cout << "\t";
    }
    std::cout << ToString() << std::endl;
    for (int i = 0; i < childCount; i++) {
        children[i]->PrintTree(depth + 1, tree);
    }
}
#endif