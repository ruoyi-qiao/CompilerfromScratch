#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <memory>
#include <vector>
#include "utilities/Token.hpp"


class Node {
private:
    int childCount;
    Token t;
    std::vector<std::shared_ptr<Node>> children;

public:
    Node();
    Node(const Token&);
    Node(const Token&, const std::vector<std::shared_ptr<Node>>&);

    const Token& GetToken();
    std::vector<std::shared_ptr<Node>> GetChildren();
    void addChild(const std::shared_ptr<Node>&);
    void setChild(int, const std::shared_ptr<Node>&);

    std::string ToString();
};

#endif