#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "Parser.hpp"

TEST(module_name, test_name) {
    EXPECT_FALSE(false);
}

TEST(Parser__Test, check_read_word) {
    std::string code = "int a = 1 ; int b = 2 ; float c = 3.0 ;\n";
    code += "{\n";
    code += "  a = a + 1 ;\n";
    code += "  b = b * a ;\n";
    code += "  if ( a < b ) then\n";
    code += "    c = c / 2 ;\n";
    code += "   else \n";
    code += "    c = c / 4 ;\n";
    code += "  \n";
    code += "} $";
    Lexer lexer;
    lexer.SetSourceCode(code);
    Parser parser(lexer);
    parser.program();
}


int main (int argc, char** argv) {
    Expr expr = Expr();
    ExprPrime(expr, Tag::OP_MINUS);
    std::string code = "int a = 1 ; int b = 2 ; real c = 3.0 ;\n";
    code += "{\n";
    code += "  a = a + 1 ;\n";
    code += "  b = b * a ;\n";
    code += "  if ( a < b ) then\n";
    code += "    c = c / 2 ;\n";
    code += "   else \n";
    code += "    c = c / 4 ;\n";
    code += "  \n";
    code += "} $";
    std::stringstream stream;
    stream << "int a = 3 ;\n"
         << "int b = 5 ;\n"
         << "real c = 3.0 ;\n\n"
         << "{\n"
         << "    a = a + 1 ;\n"
         << "    b = b * a ;\n\n"
         << "    if ( a < b ) then {\n"
         << "        c = c / 10 ;\n  b = b + 2.0 ; "
         << "    } else \n"
         << "        c = c / 4 ;\n"
         << "    \n"
         << "}\n $";

    code = stream.str();
    Lexer lexer;
    lexer.SetSourceCode(code);
    Parser parser(lexer);
    parser.program();
    // testing::InitGoogleTest(&argc, argv);
    // return RUN_ALL_TESTS();
}