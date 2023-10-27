#include <string>
#include <iostream>
class LexerError {
public:
    LexerError(const std::string& sourceCode) : sourceCode(sourceCode), line(1), column(0) {}

    void handle(char c) {
        if (c == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
    }

    void reportError(const std::string& message) {
        std::cerr << "Error at line " << line << ", column " << column << ": " << message << std::endl;
        if (line > 0) {
            std::cerr << sourceCode.substr(0, getLineStartIndex()) << std::endl;
            std::cerr << sourceCode.substr(getLineStartIndex(), getColumn()) << "^" << std::endl;
        }
    }

    int getLine() const {
        return line;
    }

    int getColumn() const {
        return column;
    }

private:
    const std::string& sourceCode;
    int line;
    int column;

    int getLineStartIndex() const {
        int startIndex = 0;
        for (int i = 1; i < line; i++) {
            startIndex = sourceCode.find('\n', startIndex);
            if (startIndex == std::string::npos) {
                return sourceCode.length();
            }
            startIndex++;
        }
        return startIndex;
    }
};