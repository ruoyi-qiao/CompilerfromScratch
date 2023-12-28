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
    // testing::InitGoogleTest(&argc, argv);
    // return RUN_ALL_TESTS();
}