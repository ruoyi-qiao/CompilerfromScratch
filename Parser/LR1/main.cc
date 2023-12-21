#include <iostream>
#include <cstdio>
#include "Parser.hpp"
#include "utilities/Token.hpp"
#include <string>

int main () {
    std::string sourceCode = "{ \n ID = NUM ;  \n } $";
    sourceCode = "{ \n while ( ID == NUM )\n{ \nID = NUM \n} \n } $";
    // sourceCode = "{ \n if ( ID == NUM )\nthen { \nID = NUM ;\n} \n else \n{ \nID = NUM ;\n} \n } $";
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
/*

语法错误，第4行，缺少";"
program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ whilestmt } => 
{ while ( boolexpr ) stmt } => 
{ while ( boolexpr ) compoundstmt } => 
{ while ( boolexpr ) { stmts } } => 
{ while ( boolexpr ) { stmt stmts } } => 
{ while ( boolexpr ) { stmt } } => 
{ while ( boolexpr ) { assgstmt } } => 
{ while ( boolexpr ) { ID = arithexpr ; } } => 
{ while ( boolexpr ) { ID = multexpr arithexprprime ; } } => 
{ while ( boolexpr ) { ID = multexpr ; } } => 
{ while ( boolexpr ) { ID = simpleexpr multexprprime ; } } => 
{ while ( boolexpr ) { ID = simpleexpr ; } } => 
{ while ( boolexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop arithexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop multexpr arithexprprime ) { ID = NUM ; } } => 
{ while ( arithexpr boolop multexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop simpleexpr multexprprime ) { ID = NUM ; } } => 
{ while ( arithexpr boolop simpleexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop NUM ) { ID = NUM ; } } => 
{ while ( arithexpr == NUM ) { ID = NUM ; } } => 
{ while ( multexpr arithexprprime == NUM ) { ID = NUM ; } } => 
{ while ( multexpr == NUM ) { ID = NUM ; } } => 
{ while ( simpleexpr multexprprime == NUM ) { ID = NUM ; } } => 
{ while ( simpleexpr == NUM ) { ID = NUM ; } } => 
{ while ( ID == NUM ) { ID = NUM ; } } 
*/