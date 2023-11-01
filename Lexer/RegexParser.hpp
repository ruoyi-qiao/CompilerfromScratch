#include <string>
#include <set>
#include "ASCII.hpp"
#include "NFA.hpp"
#include "StringInput.hpp"

class RegexParser {
private:
    NFA* nfa;
    std::vector<std::string> type;
    StringInput* input;

public:
    void registerRegex(std::string regex, std::string type) {
        
        input = new StringInput(regex);
        NFA* regexnfa = expr();
        markAcceptedState(regexnfa, type);

        if (nfa == nullptr) {
            nfa = regexnfa;
        } else {
            NFANode* newStart = new NFANode();
            newStart->addEpsilonEdge(nfa->getStart(), regexnfa->getStart());
            nfa->setStart(newStart);
        }
    }

    void markAcceptedState(NFA* nfa, std::string type) {
        NFANode* end = nfa->getEnd();
        end->setAccepted(true);
        this->type.resize(end->getID() + 1);
        this->type[end->getID()] = type;
        std::cout << "markAcceptedState: " << end->getID() << " " << type << std::endl;
    }

    NFA* getNfa() {
        return nfa;
    }
    
    std::vector<std::string> getType() {
        return type;
    }

    NFA* expr() {
        NFA* result = new NFA();
        while (input->available()) {
            int b = input->read();
            std::cout << "expr: " << (char)b << std::endl;
            switch (b) {
                case ')': {
                    return result;
                }
                case '|': {
                    input->read();
                    result->orNFA(seq());
                    break;
                }
                default: result->andNFA(seq());
            }
        }
        return result;
    }

    NFA* seq() {
        input->retract();
        NFA* result = new NFA();
        while (input->available()) {
            int b = input->read();
            std::cout << "seq: " << (char)b << std::endl;
            switch (b) {
                case '(': result->andNFA(checkClosure(expr())); break;
                case '|':
                case ')': {
                    std::cout << "?" << std::endl;
                    input->retract();
                    return result;
                }
                default: result->andNFA(atom());
            }
        }
        return result;
    }

    NFA* atom() {
        input->retract();
        int b = input->read();
        Predicate predicate;
        std::cout << "atom: " << (char)b << std::endl;
        switch (b) {
            case '[': predicate = clazz(); break;
            case '\\': predicate = single(input->read()); break;
            case '@': predicate = escape(input->read()); break;
            default: predicate = single(b); break;
        }
        return checkClosure((new NFA())->andAtom(predicate));
    }

    NFA* checkClosure(NFA* target) { // 处理类似于 a* a+ a? 的情况
        if (input->available()) {
            int b = input->read();
            std::cout << "checkClosure: " << (char)b << std::endl;
            switch (b) {
                case '*': target->star(); break;
                case '+': target->plus(); break;
                case '?': target->quest(); break;
                default: input->retract(); break;
            }
        }
        return target;
    }

    Predicate clazz() { // 处理类似于 [a-z] 的情况 
        Predicate result = nullptr;
        while (input->available()) {
            int b = input->read();
            switch (b) {
                case '[': result = orPredicate(result, clazz()); break;
                case ']': return result;
                default: result = orPredicate(result, minClazzPredicate()); break;
            }
        }
        return result;
    }

    Predicate minClazzPredicate() { // 处理类似于 [^a-z] 的情况 minClazzPredicate 
        input->retract();
        int b = input->read();
        if (b == '^') {
            std::set<int> excluded;
            while (input->available()) {
                int b1 = input->read();
                if (b1 == '[' || b1 == ']') {
                    input->retract();
                    break;
                }
                excluded.insert(b1);
            }
            return Predicate([excluded](int c) { return excluded.find(c) == excluded.end(); });
        }
        int b1 = input->read();
        if (b1 == '-') {
            int b2 = input->read();
            std::cout << "minClazzPredicate: " << (char)b << " " << (char)b1 << " " << (char)b2 << std::endl;
            if (b2 == '[' || b2 == ']') {
                input->retract();
                return Predicate([b](int c) { return c == b || c == '-'; });
            }
            return ranged(b, b2);
        }
        input->retract();
        return single(b);
    }

    Predicate escape(int b) {
        std::cout << "escape: " << (char) b << std::endl;
        auto func = Predicate([b](int c) -> bool {
            switch (b) {
                case 'D':
                case 'd': return ASCII::isDigit(c);
                case 'W':
                case 'w': return ASCII::isWord(c);
                case 'U':
                case 'u': return ASCII::isAlnum(c);
                case 'H':
                case 'h': return ASCII::isHex(c);
                case 'A':
                case 'a': return ASCII::isAlpha(c);
                case 'S':
                case 's': return ASCII::isCntrlorSpace(c);
                case 'L':
                case 'l': return !ASCII::isNewline(c);
                default: return true;
            }
        });
        return func;
    }

private:

};