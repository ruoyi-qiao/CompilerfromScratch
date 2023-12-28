#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <memory>
#include "Lexer.hpp"

TEST(module_name, test_name) {
    EXPECT_FALSE(false);
}

TEST(Lexer__Test, check_read_word) {
    std::string code = "int a = 1 ; int b = 2 ; float c = 3.0 ;\n";
    code += "{\n";
    code += "  a = a + 1 ;\n";
    code += "  b = b * a ;\n";
    code += "  if ( a < b ) {\n";
    code += "    c = c / 2 ;\n";
    code += "  } else {\n";
    code += "    c = c / 4 ;\n";
    code += "  }\n";
    code += "} $";
    Lexer lexer;
    lexer.SetSourceCode(code);
    Word word = lexer.consumeToken();
    for (; word.lexeme != "$"; word = lexer.consumeToken()) {
        std::cout << "lexeme: " << word.lexeme << std::endl;
        std::cout << "tag: " << (word.tag & ((1 << 8) - 1)) << std::endl;
    }

    lexer.printWords();
    EXPECT_FALSE(false);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}