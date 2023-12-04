#include <iostream>
#include <cstdio>
#include "Parser.hpp"
#include "utilities/Token.hpp"
#include <string>

int main () {
    std::string sourceCode = "{ \n ID = NUM ;  \n } $";
    sourceCode = "{ \n while ( ID == NUM )\n{ \nID = NUM \n} \n } $";
    Parser parser;
    parser.SetSourceCode(sourceCode);
    std::vector<std::shared_ptr<Node>> tree = parser.Parse();
    for (auto node : tree) {
        std::cout << node->ToString() << std::endl;
        for (auto child : node->GetChildren()) {
            std::cout << "\t" << child->ToString() << std::endl;
        }
    }
    std::shared_ptr<Node> root = tree[0];
    std::cout << root->ToString() << std::endl;
    root->PrintTree(0, tree);
    return 0;
}