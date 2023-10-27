#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

class StringInput {
private:
    std::string data;
    size_t forward;
    std::vector<size_t> marks;

public:
    StringInput(const std::string str) : data(str), forward(0) {
        mark();
    }

    bool isOpen() const {
        return !data.empty();
    }

    bool available() const {
        return forward < data.length();
    }

    int read() {
        checkAvailable();
        return data[forward++];
    }

    int read(int count) {
        if (count <= 0) {
            throw std::invalid_argument("Read count should be positive: " + std::to_string(count));
        }
        forward += count - 1;
        checkAvailable();
        return data[forward++];
    }

    void mark() {
        marks.push_back(forward);
    }

    void removeMark() {
        if (!marks.empty()) {
            marks.pop_back();
        }
    }

    void recover(bool consume) {
        if (!marks.empty()) {
            forward = consume ? marks.back() : marks.back();
            marks.pop_back();
        }
    }

    std::string capture() {
        switch (marks.size()) {
            case 1:
                return capture(forward, marks.back());
            case 0:
                return capture(0, marks.back());
            default:
                size_t end = marks.back();
                marks.pop_back();
                size_t start = marks.back();
                marks.pop_back();
                return capture(start, end);
        }
    }

    int retract() {
        forward = forward > 0 ? forward - 1 : 0;
        return data[forward];
    }

private:
    void checkAvailable() {
        if (!available()) {
            throw std::runtime_error("No more data available to read.");
        }
    }
    std::string capture(size_t start, size_t end) {
        return data.substr(start, end - start);
    }
};
