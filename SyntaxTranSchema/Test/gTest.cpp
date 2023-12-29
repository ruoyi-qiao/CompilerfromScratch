#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include "interpreter.hpp"

void interpret(std::istream& input) {
    // Redirect cin to read from the provided input stream
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::cin.rdbuf(input.rdbuf());

    Analysis(); // Perform processing that reads from std::cin

    // Restore cin to its original stream
    std::cin.rdbuf(cinbuf);
}

TEST(MyFunctionTest, OutputsMatchExpected) {
    const std::vector<std::string> inputFiles = {"input_1.txt", "input_2.txt", "input_3.txt"};
    const std::vector<std::string> outputFiles = {"expected_output_1.txt", "expected_output_2.txt", "expected_output_3.txt"};

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
