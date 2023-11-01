#include <string>
#include <iostream>
class LexerError {
public:
    LexerError(const std::string& sourceCode) : sourceCode(sourceCode), line(1), column(1) {}

    LexerError(const LexerError& other) : sourceCode(other.sourceCode), line(other.line), column(other.column) {
        prevPositions = other.prevPositions;
    }

    void handle(char c) {
        prevPositions.push_back(std::make_pair(line, column));
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }

    int retrace() {
        if (prevPositions.empty()) {
            std::cerr << "No previous position to retrace" << std::endl;
            return -1;
        }
        auto [prevLine, prevColumn] = prevPositions.back();
        prevPositions.pop_back();
        return 0;
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
    std::vector<std::pair<int, int>> prevPositions;
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