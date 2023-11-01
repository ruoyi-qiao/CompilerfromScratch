#pragma once
#include <functional>

using Predicate = std::function<bool(int)>;

Predicate single(int ch) { 
    std::cout << "single: " << (char)ch << std::endl;
    return [ch](int c) { return c == ch; };
}

Predicate andPredicate(const Predicate& c1, const Predicate& c2) {
    return [c1, c2](int c) { return c1(c) && c2(c); };
}

Predicate orPredicate(const Predicate& c1, const Predicate& c2) {
    return [c1, c2](int c) { return c1(c) || c2(c); };
}

Predicate ranged(int start, int end) {
    return [start, end](int c) { return c >= start && c <= end; };
}
