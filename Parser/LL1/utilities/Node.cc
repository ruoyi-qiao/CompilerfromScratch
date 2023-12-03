#include "Node.hpp"

Node::Node(){
    t = {};
}

Node::Node(const Token& token) {
    t = token;
}

Node::Node(const Token& token, const std::vector<std::shared_ptr<Node>>& children) {
    t = token;
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

std::string Node::ToString(){
    return t.lexeme;
}
