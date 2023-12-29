#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include "Parser/Parser.hpp"

/**********************************/
void read_prog(std::string& prog) {
    char c;
    while (std::cin.get(c)) {
        prog += c;
    }
}
/**********************************/

void Analysis()
{
	std::string _prog, prog;
	read_prog(_prog);
    _prog += " $";
    for (int i = 0; i < (int)_prog.length(); i++) {
        if (i == static_cast<int>(_prog.length()) || !(_prog[i] == '\n' && _prog[i + 1] == '\n')) {
            prog.push_back(_prog[i]);
        }
    }

    int exec_status = 0;

    Lexer lexer;
    lexer.SetSourceCode(prog);
    Parser parser(lexer);
    std::map<Word, Id> symbolTable = parser.program(exec_status);
    
    if (exec_status == 0) {
        for (std::map<Word, Id>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++) {
            if (it->second.type == Type::Int) {
                std::cout << it->first.toString() << ": " << it->second.IdValue.ival;
            } else {
                std::cout << it->first.toString() << ": " << it->second.IdValue.fval;
            }
            if (std::next(it) != symbolTable.end()) {
                std::cout << std::endl;
            }
        }
    }
}


void interpret(std::istream& input) {
    // Redirect cin to read from the provided input stream
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::cin.rdbuf(input.rdbuf());

    Analysis(); // Perform processing that reads from std::cin

    // Restore cin to its original stream
    std::cin.rdbuf(cinbuf);
}

TEST(MyFunctionTest, OutputsMatchExpected) {
    const std::vector<std::string> inputFiles = {"/Test/input_1.txt", "/Test/input_2.txt", "/Test/input_3.txt"};
    const std::vector<std::string> outputFiles = {"/Test/expected_output_1.txt", "/Test/expected_output_2.txt", "/Test/expected_output_3.txt"};

    for (size_t i = 0; i < inputFiles.size(); ++i) {
        // Open input file for reading
        std::ifstream inputFile(inputFiles[i]);
        if (!inputFile.is_open()) {
            std::cerr << "Unable to open input file: " << inputFiles[i] << std::endl;
            continue;
        }

        // Get the content of input file as an input stream
        std::stringstream inputStream;
        inputStream << inputFile.rdbuf();

        // Open expected output file for reading
        std::ifstream expectedFile(outputFiles[i]);
        if (!expectedFile.is_open()) {
            std::cerr << "Unable to open output file: " << outputFiles[i] << std::endl;
            continue;
        }

        // Get the content of expected output file as a string
        std::string expectedOutput((std::istreambuf_iterator<char>(expectedFile)),
                                   std::istreambuf_iterator<char>());

        // Redirect stdout to capture the output
        testing::internal::CaptureStdout();

        // Call interpret with the input stream
        interpret(inputStream);

        // Get the captured output
        std::string programOutput = testing::internal::GetCapturedStdout();

        // Compare program output with the expected output for this test iteration
        ASSERT_EQ(programOutput, expectedOutput);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}