#include "Node.hpp"

Node::Node(){
    t = {};
    left = {};
    right = {};
}

Node::Node(const Token& token){
    t = token;
    left = {};
    right = {};
}

Node::Node(const Token& token, std::shared_ptr<Node> left, std::shared_ptr<Node> right){
    t = token;
    left = left;
    right = right;
}

const Token& Node::GetToken(){
    return t;
}

std::shared_ptr<Node> Node::GetLeft(){
    return left;
}

std::shared_ptr<Node> Node::GetRight(){
    return right;
}

std::string Node::ToString(){
    return t.lexeme;
}
