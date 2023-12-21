#include <iostream>
#include <cstdio>
#include "Parser.hpp"
#include "utilities/Token.hpp"
#include <string>

int main () {
    std::string sourceCode = "{ \n ID = NUM ;  \n } $";
    sourceCode = "{ \n while ( ID == NUM )\n{ \nID = NUM ;\n} \n } $";
    sourceCode = "{ \n if ( ID == NUM )\nthen { \nID = NUM ;\n} \n else \n{ \nID = NUM ;\n} \n } $";
    Parser parser;
    parser.SetSourceCode(sourceCode);
    std::vector<std::vector<std::string>> snapshots = parser.Parse();
    
    std::vector<std::vector<std::string>>::reverse_iterator rit;
    for (rit = snapshots.rbegin(); rit != snapshots.rend(); rit++) {
    
        std::vector<std::string>::iterator it;
        for (it = rit->begin(); it != rit->end(); it++) {
            std::cout << *it << " ";
        }
        
        if (next(rit) != snapshots.rend())
            std::cout << " => " << std::endl; 
    }
    
    return 0;
}