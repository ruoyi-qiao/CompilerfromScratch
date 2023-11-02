#include <iostream>
#include <functional>
#include <fstream>
#include <iomanip>

#define dbg(x...) do { std::cerr << #x << "  -> "; err(x); } while (0);
void err() { std::cerr << std::endl; }
template<class T, class... Ts> void err(const T& arg, const Ts&... args) {std::cerr << arg << " "; err(args...); }

using Predicate = std::function<bool(int)>;

Predicate single(int ch) { 
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

class NFANode {
private:
    static int nodeCount;
    int edgeType;
    NFANode* next1;
    NFANode* next2;
    Predicate predicate;
    bool accepted;
    const int id;

public:
    static const int NO_EDGE = 0;
    static const int SINGLE_EPSILON = 1;
    static const int DOUBLE_EPSILON = 2;
    static const int CHAR = 3;
    
public:
    NFANode() : edgeType(NO_EDGE), next1(nullptr), next2(nullptr), predicate(nullptr), accepted(false), id(nodeCount++) {}

    void addEpsilonEdge(NFANode* next) {
        if (edgeType == NO_EDGE) {
            next1 = next;
            edgeType = SINGLE_EPSILON;
        } else if (edgeType == SINGLE_EPSILON) {
            next2 = next;
            edgeType = DOUBLE_EPSILON;
        }
    }

    void addEpsilonEdge(NFANode* next1, NFANode* next2) {
        
        this->next1 = next1;
        this->next2 = next2;
        edgeType = DOUBLE_EPSILON;
    }

    void addCharEdge(Predicate& predicate, NFANode* next) {
        this->predicate = predicate;
        next1 = next;
        edgeType = CHAR;
    }

    friend std::ostream& operator<<(std::ostream& os, const NFANode& node) {
        switch (node.edgeType) {
            case NO_EDGE:
                break;
            case SINGLE_EPSILON:
                os << node.id << " " << node.next1->id << " ε" << std::endl;
                break;
            case DOUBLE_EPSILON:
                os << node.id << " " << node.next1->id << " ε" << std::endl;
                os << node.id << " " << node.next2->id << " ε" << std::endl;
                break;
            case CHAR:
                os << node.id << " " << node.next1->id << " c" << std::endl;
                break;
        }
        return os;
    }

    int getID() const {
        return id;
    }

    int getEdgeType() const {
        return edgeType;
    }

    Predicate getPredicate() const {
        return predicate;
    }    

    std::pair<NFANode*, NFANode*> getNext() const {
        return std::make_pair(next1, next2);
    }

    static int getNodeCount() {
        return nodeCount;
    }

    bool isAccepted() const {
        return accepted;
    }

    void setAccepted(bool accepted) {
        this->accepted = accepted;
    }
};

int NFANode::nodeCount = 0;


#include <vector>
#include <queue>

class NFA {
private:
    NFANode* start;
    NFANode* end;

    bool isEmpty() {
        return start == nullptr || end == nullptr;
    }

public:
    
    NFA(NFANode* start, NFANode* end) : start(start), end(end) {}
    NFA() : start(nullptr), end(nullptr) {}

    void andNFA(NFA* other) {
        if (isEmpty()) {
            start = other->start;
            end = other->end;
        } else {
            end->addEpsilonEdge(other->start);
            end = other->end;
        }
    }

    NFA* andAtom(Predicate& predicate) {
        // Implement the 'andAtom' logic.
        NFANode* newStart = new NFANode();
        NFANode* newEnd = new NFANode();
        newStart->addCharEdge(predicate, newEnd);
        if (isEmpty()) {
            start = newStart;
            end = newEnd;
        } else {
            end->addEpsilonEdge(newStart);
            end = newEnd;
        }
        return this;
    }

    void star() {
        // Implement the 'star' logic.
        NFANode* newStart = new NFANode();
        NFANode* newEnd = new NFANode();
        newStart->addEpsilonEdge(start);
        end->addEpsilonEdge(newEnd);
        end->addEpsilonEdge(start);
        newStart->addEpsilonEdge(newEnd);
        start = newStart;
        end = newEnd;
    }

    void quest() {
        // Implement the 'quest' logic.
        NFANode* newStart = new NFANode();
        NFANode* newEnd = new NFANode();
        newStart->addEpsilonEdge(start, newEnd);
        end->addEpsilonEdge(newEnd);
        start = newStart;
        end = newEnd;
    }

    void plus() {
        // Implement the 'plus' logic.
        NFANode* newStart = new NFANode();
        NFANode* newEnd = new NFANode();
        newStart->addEpsilonEdge(start);
        end->addEpsilonEdge(newEnd, start);
        start = newStart;
        end = newEnd;
    }

    void orNFA(NFA* other) {
        // Implement the 'or' logic.
        NFANode* newStart = new NFANode();
        NFANode* newEnd = new NFANode();
        if (isEmpty()) {
            start = newStart;
            end = newEnd;
        } else {
            newStart->addEpsilonEdge(start, other->start);
            end->addEpsilonEdge(newEnd);
            other->end->addEpsilonEdge(newEnd);
            start = newStart;
            end = newEnd;
        } 
    }
    std::vector<NFANode*> getAllNode () {
        std::vector<NFANode*> nodes;
        std::vector<bool> visited(NFANode::getNodeCount(), false);
        std::queue<NFANode*> queue;
        queue.push(start);
        while (!queue.empty()) {
            NFANode* node = queue.front();
            queue.pop();
            if (visited[node->getID()]) {
                continue;
            }
            visited[node->getID()] = true;
            nodes.push_back(node);
            // auto [next1, next2] = node->getNext();
            NFANode* next1, *next2;
            std::tie(next1, next2) = node->getNext();
            int id1 = next1 == nullptr ? -1 : next1->getID();
            int id2 = next2 == nullptr ? -1 : next2->getID();

            switch (node->getEdgeType())
            {
            case NFANode::SINGLE_EPSILON:
                if(!visited[id1]) queue.push(next1);
                break;
            case NFANode::DOUBLE_EPSILON:
                if(!visited[id1]) queue.push(next1);
                if(!visited[id2]) queue.push(next2);
                break;
            case NFANode::CHAR:
                if(!visited[id1]) queue.push(next1);
                break;
            case NFANode::NO_EDGE:
                break;
            default:
                std::cerr << "Error: unknown edge type." << std::endl;
                exit(1);
                break;
            }
        }
        return nodes;
    }
    void print() {
        if (isEmpty()) {
            return;
        }
        std::vector<bool> printed(NFANode::getNodeCount(), false);
        std::queue<NFANode*> queue;
        queue.push(start);
        while (!queue.empty()) {
            NFANode* node = queue.front();
            queue.pop();
            if (printed[node->getID()]) {
                continue;
            }
            printed[node->getID()] = true;

            // auto [next1, next2] = node->getNext();
            NFANode* next1, *next2;
            std::tie(next1, next2) = node->getNext();
            int id1 = next1 == nullptr ? -1 : next1->getID();
            int id2 = next2 == nullptr ? -1 : next2->getID();

            switch (node->getEdgeType())
            {
            case NFANode::SINGLE_EPSILON:
                if(!printed[id1]) queue.push(next1);
                break;
            case NFANode::DOUBLE_EPSILON:
                if(!printed[id1]) queue.push(next1);
                if(!printed[id2]) queue.push(next2);
                break;
            case NFANode::CHAR:
                if(!printed[id1]) queue.push(next1);
                break;
            case NFANode::NO_EDGE:
                break;
            default:
                std::cerr << "Error: unknown edge type." << std::endl;
                exit(1);
                break;
            }
        }
    }

    NFANode* getStart() const {
        return start;
    }

    NFANode* getEnd() const {
        return end;
    }

    void setStart(NFANode* node) {
        start = node;
    } 

    void setEnd(NFANode* node) {
        end = node;
    } 
};

class ASCII {
public:
    static const int UPPER = 0x00000100;
    static const int LOWER = 0x00000200;
    static const int DIGIT = 0x00000400;
    static const int SPACE = 0x00000800;
    static const int PUNCT = 0x00001000;
    static const int CNTRL = 0x00002000;
    static const int BLANK = 0x00004000;
    static const int HEX   = 0x00008000;
    static const int UNDER = 0x00010000;
    
    // Letters(字母)
    static const int ALPHA = (UPPER | LOWER);
    
    // Alphanumeric characters (字母 + 数字)
    static const int ALNUM = (UPPER | LOWER | DIGIT);
    
    // Graphical characters (图形字符) 如：!@#$%^&*()_+|~-=`{}[]:";'<>?,./    
    static const int GRAPH = (PUNCT | UPPER | LOWER | DIGIT);
    
    // Word characters (单词字符) 如：A-Za-z0-9_
    static const int WORD = (UPPER | LOWER | UNDER | DIGIT);

    // 判断字符是否为某一类型
    static bool isType(int ch, int type) {
        return (ch & 0xFFFFFF80) == 0 && (ctype[ch] & type) != 0;
    }

    static bool isAlnum(int b) {
        return isType(b, ALNUM);
    }

    static bool isAlpha(int b) {
        return isType(b, ALPHA);
    }

    static bool isDigit(int b) {
        return isType(b, DIGIT);
    }

    static bool isHex(int b) {
        return isType(b, HEX);
    }

    static bool isWord(int b) {
        return isType(b, WORD);
    }

    static bool isCntrlorSpace(int b) {
        return isType(b, CNTRL|SPACE);
    }

    static bool isNewline(int b) {
        return b == '\n' || b == '\r';
    }

private:
    static const int ctype[];
};

const int ASCII::ctype[] = {
    CNTRL,                  /* 00 (NUL) */
    CNTRL,                  /* 01 (SOH) */
    CNTRL,                  /* 02 (STX) */
    CNTRL,                  /* 03 (ETX) */
    CNTRL,                  /* 04 (EOT) */
    CNTRL,                  /* 05 (ENQ) */
    CNTRL,                  /* 06 (ACK) */
    CNTRL,                  /* 07 (BEL) */
    CNTRL,                  /* 08 (BS)  */
    SPACE + CNTRL + BLANK,  /* 09 (HT)  */
    SPACE + CNTRL,          /* 0A (LF)  */
    SPACE + CNTRL,          /* 0B (VT)  */
    SPACE + CNTRL,          /* 0C (FF)  */
    SPACE + CNTRL,          /* 0D (CR)  */
    CNTRL,                  /* 0E (SI)  */
    CNTRL,                  /* 0F (SO)  */
    CNTRL,                  /* 10 (DLE) */
    CNTRL,                  /* 11 (DC1) */
    CNTRL,                  /* 12 (DC2) */
    CNTRL,                  /* 13 (DC3) */
    CNTRL,                  /* 14 (DC4) */
    CNTRL,                  /* 15 (NAK) */
    CNTRL,                  /* 16 (SYN) */
    CNTRL,                  /* 17 (ETB) */
    CNTRL,                  /* 18 (CAN) */
    CNTRL,                  /* 19 (EM)  */
    CNTRL,                  /* 1A (SUB) */
    CNTRL,                  /* 1B (ESC) */
    CNTRL,                  /* 1C (FS)  */
    CNTRL,                  /* 1D (GS)  */
    CNTRL,                  /* 1E (RS)  */
    CNTRL,                  /* 1F (US)  */
    SPACE + BLANK,          /* 20 SPACE */
    PUNCT,                  /* 21 !     */
    PUNCT,                  /* 22 "     */
    PUNCT,                  /* 23 #     */
    PUNCT,                  /* 24 $     */
    PUNCT,                  /* 25 %     */
    PUNCT,                  /* 26 &     */
    PUNCT,                  /* 27 '     */
    PUNCT,                  /* 28 (     */
    PUNCT,                  /* 29 )     */
    PUNCT,                  /* 2A *     */
    PUNCT,                  /* 2B +     */
    PUNCT,                  /* 2C ,     */
    PUNCT,                  /* 2D -     */
    PUNCT,                  /* 2E .     */
    PUNCT,                  /* 2F /     */
    DIGIT + HEX + 0,        /* 30 0     */
    DIGIT + HEX + 1,        /* 31 1     */
    DIGIT + HEX + 2,        /* 32 2     */
    DIGIT + HEX + 3,        /* 33 3     */
    DIGIT + HEX + 4,        /* 34 4     */
    DIGIT + HEX + 5,        /* 35 5     */
    DIGIT + HEX + 6,        /* 36 6     */
    DIGIT + HEX + 7,        /* 37 7     */
    DIGIT + HEX + 8,        /* 38 8     */
    DIGIT + HEX + 9,        /* 39 9     */
    PUNCT,                  /* 3A :     */
    PUNCT,                  /* 3B ;     */
    PUNCT,                  /* 3C <     */
    PUNCT,                  /* 3D =     */
    PUNCT,                  /* 3E >     */
    PUNCT,                  /* 3F ?     */
    PUNCT,                  /* 40 @     */
    UPPER + HEX + 10,       /* 41 A     */
    UPPER + HEX + 11,       /* 42 B     */
    UPPER + HEX + 12,       /* 43 C     */
    UPPER + HEX + 13,       /* 44 D     */
    UPPER + HEX + 14,       /* 45 E     */
    UPPER + HEX + 15,       /* 46 F     */
    UPPER + 16,             /* 47 G     */
    UPPER + 17,             /* 48 H     */
    UPPER + 18,             /* 49 I     */
    UPPER + 19,             /* 4A J     */
    UPPER + 20,             /* 4B K     */
    UPPER + 21,             /* 4C L     */
    UPPER + 22,             /* 4D M     */
    UPPER + 23,             /* 4E N     */
    UPPER + 24,             /* 4F O     */
    UPPER + 25,             /* 50 P     */
    UPPER + 26,             /* 51 Q     */
    UPPER + 27,             /* 52 R     */
    UPPER + 28,             /* 53 S     */
    UPPER + 29,             /* 54 T     */
    UPPER + 30,             /* 55 U     */
    UPPER + 31,             /* 56 V     */
    UPPER + 32,             /* 57 W     */
    UPPER + 33,             /* 58 X     */
    UPPER + 34,             /* 59 Y     */
    UPPER + 35,             /* 5A Z     */
    PUNCT,                  /* 5B [     */
    PUNCT,                  /* 5C \     */
    PUNCT,                  /* 5D ]     */
    PUNCT,                  /* 5E ^     */
    PUNCT | UNDER,          /* 5F _     */
    PUNCT,                  /* 60 `     */
    LOWER + HEX + 10,       /* 61 a     */
    LOWER + HEX + 11,       /* 62 b     */
    LOWER + HEX + 12,       /* 63 c     */
    LOWER + HEX + 13,       /* 64 d     */
    LOWER + HEX + 14,       /* 65 e     */
    LOWER + HEX + 15,       /* 66 f     */
    LOWER + 16,             /* 67 g     */
    LOWER + 17,             /* 68 h     */
    LOWER + 18,             /* 69 i     */
    LOWER + 19,             /* 6A j     */
    LOWER + 20,             /* 6B k     */
    LOWER + 21,             /* 6C l     */
    LOWER + 22,             /* 6D m     */
    LOWER + 23,             /* 6E n     */
    LOWER + 24,             /* 6F o     */
    LOWER + 25,             /* 70 p     */
    LOWER + 26,             /* 71 q     */
    LOWER + 27,             /* 72 r     */
    LOWER + 28,             /* 73 s     */
    LOWER + 29,             /* 74 t     */
    LOWER + 30,             /* 75 u     */
    LOWER + 31,             /* 76 v     */
    LOWER + 32,             /* 77 w     */
    LOWER + 33,             /* 78 x     */
    LOWER + 34,             /* 79 y     */
    LOWER + 35,             /* 7A z     */
    PUNCT,                  /* 7B {     */
    PUNCT,                  /* 7C |     */
    PUNCT,                  /* 7D }     */
    PUNCT,                  /* 7E ~     */
    CNTRL                   /* 7F (DEL) */
};

#include <string>
#include <stdexcept>
#include <set>

class StringInput {
// private:
public:
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
        int result = data[forward++];
        if (result < 0) result = 128;
        return result;
    }

    int readOrigin() {
        checkAvailable();
        return data[forward++];
    }

    int read(int count) {
        if (count <= 0) {
            throw std::invalid_argument("Read count should be positive: " + std::to_string(count));
        }
        forward += count - 1;
        checkAvailable();
        int result = data[forward++];
        if (result < 0) result = 128;
        return result;
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

    void retrace() {
        forward = forward > 0 ? forward - 1 : 0;
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
            switch (b) {
                case '(': result->andNFA(checkClosure(expr())); break;
                case '|':
                case ')': {
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
        switch (b) {
            case '[': predicate = clazz(); break;
            case '\\': predicate = single(input->read()); break;
            case '@': predicate = escape(input->read()); break;
            default: predicate = single(b); break;
        }
        return checkClosure((new NFA())->andAtom(predicate));
    }

    NFA* checkClosure(NFA* target) { // 处理类似于 a* a+ a? 的闭包
        if (input->available()) {
            int b = input->read();
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

    Predicate minClazzPredicate() { // 处理类似于 [^a-z] 的情况 
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

#include <unordered_set>
#include <unordered_map>

using NFANodeSet = std::unordered_set<const NFANode*>;
// const int INVALID = std::numeric_limits<int>::min();
const int INVALID = INT32_MIN;

struct NFANodeSetHash {
    std::size_t operator()(const NFANodeSet& nodeSet) const {
        std::size_t hash = 0;
        for (const NFANode* node : nodeSet) {
            // Combine the hashes of individual NFANode* pointers
            hash ^= std::hash<const NFANode*>{}(node);
        }
        return hash;
    }
};

struct NFANodeSetEqual {
    bool operator()(const NFANodeSet& lhs, const NFANodeSet& rhs) const {
        // NFANodeSet equality means that they contain the same set of pointers
        return lhs == rhs;
    }
};

class DFAConverter {
public:
    DFAConverter(RegexParser* parser);
    void convert(NFANode* startNode);

private:
    // nfa 转 dfa
    void transfer(NFANode* startNode);
    int addDFAState(NFANodeSet& g, std::vector<int>& unchecked);
    static NFANodeSet epsilonClosure(const NFANodeSet& nodes);
    static NFANodeSet next(const NFANodeSet& begin, int b);
    
    using transitionTable = std::vector<std::vector<int>>;
    // 最小化 dfa 
    transitionTable minimizeTransitionTable; 
    std::vector<std::string> minimizeTokens;

    void dfaMinimize();
    std::set<std::set<int>> split(std::set<int>& s);
    void outputData();

public:
    int runDFA(const std::string& input, std::string& token);

private:
    NFA* nfa;
    std::unordered_map<NFANodeSet, int, NFANodeSetHash, NFANodeSetEqual> nfa2dfaMap;
    std::vector<NFANodeSet> dfa2nfaMap;
    std::vector<std::string> nodeToToken;
    int statesCount = 0;
public:
    std::vector<bool> accepted;
    std::vector<std::vector<int>> goTo;
    std::vector<std::string> tokens;
};

DFAConverter::DFAConverter(RegexParser* parser) {
    this->nfa = parser->getNfa();
    this->nodeToToken = parser->getType();
    NFANode* startNode = nfa->getStart();
    convert(startNode);
}

void DFAConverter::convert(NFANode* startNode) {
    transfer(startNode);
    // dfaMinimize();
}

NFANodeSet DFAConverter::epsilonClosure(const NFANodeSet& nodes) {
    NFANodeSet result;
    std::queue<const NFANode*> q;
    for (const NFANode* node : nodes) {
        q.push(node);
        result.insert(node);
    }
    while (!q.empty()) {
        const NFANode* node = q.front();
        q.pop();
        result.insert(node);
        // auto [next1, next2] = node->getNext();
        NFANode* next1, *next2;
        std::tie(next1, next2) = node->getNext();
        if (node->getEdgeType() == NFANode::SINGLE_EPSILON) {
            if (result.find(next1) == result.end()) {
                q.push(next1);
                result.insert(next1);
            }
            q.push(next1);
            result.insert(next1);
        } 
        if (node->getEdgeType() == NFANode::DOUBLE_EPSILON) {
            if (result.find(next1) == result.end()) {
                q.push(next1);
                result.insert(next1);
            }
            if (result.find(next2) == result.end()) {
                q.push(next2);
                result.insert(next2);
            }
        }
    }

    return result;
}

void DFAConverter::transfer(NFANode* startNode) {
    NFANodeSet startSet = epsilonClosure({startNode});
    std::vector<int> unchecked;
    addDFAState(startSet, unchecked); // 
    while (!unchecked.empty()) {
        int state = unchecked.back();
        unchecked.pop_back();
        for (int b = 0; b <= 128; b++) {
            NFANodeSet nextSet = epsilonClosure(next(dfa2nfaMap[state], b));
            if (nextSet.empty()) {
                continue;
            }
            int nextState = addDFAState(nextSet, unchecked);
            goTo[state][b] = nextState;
        }
    }
}

int DFAConverter::addDFAState(NFANodeSet& g, std::vector<int>& unchecked) {
    
    if (nfa2dfaMap.find(g) != nfa2dfaMap.end()) {
        return nfa2dfaMap[g];
    }

    int state = statesCount++;
    nfa2dfaMap[g] = state;
    dfa2nfaMap.push_back(g);
    unchecked.push_back(state);
    goTo.push_back(std::vector<int>(128 + 1, -1));
    bool isAccepted = false;
    int TokenID = -1;
    std::string type;

    for (const NFANode* node : g) {
        if (node->isAccepted()) {
            if (TokenID == -1) {
                isAccepted = true;
                TokenID = node->getID();
                type = nodeToToken[TokenID];
            } else {
                std::string currentType = nodeToToken[node->getID()];
                if (std::stoi(currentType) < std::stoi(type)) {
                    TokenID = node->getID();
                    type = currentType;
                }
            }
        }
    }

    accepted.push_back(isAccepted);
    if (TokenID != -1) tokens.push_back(nodeToToken[TokenID]);
    else tokens.push_back("");
    return state;
}

NFANodeSet DFAConverter::next(const NFANodeSet& begin, int b) {
    NFANodeSet result;
    for (const NFANode* node : begin) {
        // auto [next1, next2] = node->getNext();
        NFANode* next1, *next2;
        std::tie(next1, next2) = node->getNext();
        if (node->getEdgeType() == NFANode::CHAR && node->getPredicate()(b)) {
            result.insert(next1);
        }
    }
    return result;
}

int DFAConverter::runDFA(const std::string& input, std::string &token) {
    int state = 0;
    for (int c : input) {
        if (c < 0) c = 128;
        state = goTo[state][c];
        if (state == -1) {
            token = "invalid";
            return -1;
        }
    }
    token = tokens[state];
    return state;
}

void DFAConverter::dfaMinimize() {
    std::set<std::set<int>> newDfaStates, oldDfaStates;
    std::set<int> acceptedStates, unacceptedStates;
    for (int i = 0; i < statesCount; i++) {
        if (accepted[i]) {
            acceptedStates.insert(i);
        } else {
            unacceptedStates.insert(i);
        }
    }
    newDfaStates.insert(acceptedStates);
    newDfaStates.insert(unacceptedStates);
    while (newDfaStates != oldDfaStates) {
        oldDfaStates = newDfaStates;
        newDfaStates.clear();
        for (auto s : oldDfaStates) {
            std::set<std::set<int>> s1 = split(s);
            for (auto s2 : s1) {
                newDfaStates.insert(s2);
            }
        }
    }
    // 映射最小化 dfa 的状态
    std::unordered_map<int, int> minDfaMap;
    std::vector<int> minDfa2nfaMap;
    int minStatesCount = 0;
    for (auto s : newDfaStates) {
        // 输出检查
        // for (int state : s) {
        //     std::cout << "(" << state << ", " << tokens[state] << ") ";
        // } std::cout << std::endl;

        for (int state : s) {
            minDfaMap[state] = minStatesCount;
        }
        minDfa2nfaMap.push_back(*s.begin());
        minStatesCount++;
    }
    // 构建最小化 dfa 的转移表
    minimizeTransitionTable.resize(minStatesCount);
    for (int i = 0; i < minStatesCount; i++) {
        minimizeTransitionTable[i].resize(128 + 1, -1);
    }
    for (auto s : newDfaStates) {
        int state = minDfaMap[*s.begin()];
        for (int b = 0; b <= 128; b++) {
            int nextState = goTo[*s.begin()][b];
            if (nextState != -1) {
                nextState = minDfaMap[nextState];
            }
            minimizeTransitionTable[state][b] = nextState;
        }
    }
    // minimizeTokens
    minimizeTokens.resize(minStatesCount);
    for (int i = 0; i < minStatesCount; i++) {
        minimizeTokens[i] = tokens[minDfa2nfaMap[i]];
    }
    // debug
    // std::cout << "minimizeTransitionTable:" << std::endl;

    // std::cout << std::setw(4) << " ";
    // for (int i = 0; i <= 128; i++) {
    //     std::cout << std::setw(4) << i;
    // } std::cout << std::endl;

    // for (int i = 0; i < minStatesCount; i++) {
    //     std::cout << std::setw(4) << i;
    //     for (int j = 0; j <= 128; j++) {
    //         std::cout << std::setw(4) << minimizeTransitionTable[i][j];
    //     } std::cout << std::endl;
    // }

    // std::cout << "minimizeTokens:" << std::endl;
    // for (int i = 0; i < minStatesCount; i++) {
    //     std::cout << std::setw(4) << i << " " << minimizeTokens[i] << std::endl;
    // }
}
#include <assert.h>
std::set<std::set<int>> DFAConverter::split(std::set<int>& s) {

    std::set<std::set<int>> result;
    for (int b = 0; b <= 128; b++) {
        // 如果 b 将 s 分割成两个集合，则返回 {s1, s2}
        std::set<int> s1, s2;
        int s1ID = INVALID;
        std::string tkn; 
        for (int state : s) {
            int nextState = goTo[state][b];
            if (s1ID == INVALID) {
                s1ID = nextState;
                tkn = tokens[state];
                s1.insert(state);
            } else {
                if (nextState == s1ID) {
                    if (tokens[state] == tkn) {
                        s1.insert(state);
                    } else {
                        s2.insert(state);
                    }
                } else {
                    s2.insert(state);
                }
            }
        }
        if (!s1.empty() && !s2.empty()) {
            result.insert(s1);
            result.insert(s2);
            return result;
        }
    }

    result.insert(s);
    return result;
}

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
        // auto [prevLine, prevColumn] = prevPositions.back();
        int prevLine, prevColumn;
        std::tie(prevLine, prevColumn) = prevPositions.back();
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


struct Token {
    std::string value;
    std::string type;
};


using DFATable = std::vector<std::vector<int>>;

class Lexer {
public:
    Lexer(
        DFAConverter& converter,
        const std::string& sourceCode
    ) : acceptedStates_(converter.accepted),
        transitionTable_(converter.goTo),
        tokenLabels_(converter.tokens),
        sourceCode_(sourceCode), 
        input(sourceCode),
        error(sourceCode){

        currentState_ = 0;
        currentTokenStart_ = 0;

        // output table for debug out.txt
        // std::ofstream out("out.txt");
        // out << std::setw(4) << " ";
        // for (int i = 0; i <= 128; i++) {
        //     out << std::setw(4) << i;
        // }
        // out << std::endl;
        // for (int i = 0; i < transitionTable_.size(); i++) {
        //     out << std::setw(4) << i;
        //     for (int j = 0; j < transitionTable_[i].size(); j++) {
        //         out << std::setw(4) << transitionTable_[i][j];
        //     }
        //     out << std::endl;
        // }
    }

    Token getNextToken();
    bool isAcceptedState(int state) const;
    int transition(int state, int c) const;


private:
    StringInput input;
    LexerError error;
    std::string sourceCode_;
    const std::vector<bool>& acceptedStates_;
    const DFATable& transitionTable_;
    const std::vector<std::string>& tokenLabels_;
    int currentState_;
    size_t currentTokenStart_;
    const int ErrorState = -1;
    const int BadState = -2;
};

bool Lexer::isAcceptedState(int state) const {
    if (state < 0 || state >= acceptedStates_.size()) 
        return false;
    return acceptedStates_[state];
}

int Lexer::transition(int state, int c) const {
    if (state < 0 || state >= transitionTable_.size()) {
        std::cout << "error: " << state << std::endl;
        throw LexerError("Invalid state: " + std::to_string(state));
    }
    if (c < 0 || c >= transitionTable_[state].size()) {
        std::cout << "error: " << state << std::endl;
        std::cout << "error: " << (int)c << std::endl;
        throw LexerError("Invalid input: " + std::to_string(c));
    }
    return transitionTable_[state][c];
}

Token Lexer::getNextToken() {
    if (!input.available()) {
        return {"", "EOF"};
    }
    std::string lexeme = "";
    std::vector<int> stateStack;
    
    currentState_ = 0;
    stateStack.push_back(BadState);

    while (currentState_ != ErrorState && input.available()) {
        int c = input.readOrigin();
        error.handle(c);
        lexeme += c;
        if (isAcceptedState(currentState_)) {
            stateStack.clear();
        }
        stateStack.push_back(currentState_);
        currentState_ = transition(currentState_, c < 0 ? 128 : c);
    }
        

    while (!isAcceptedState(currentState_) && currentState_ != BadState) {
        lexeme.pop_back();
        input.retrace();
        error.retrace();
        currentState_ = stateStack.back();
        stateStack.pop_back();
    }
    if (isAcceptedState(currentState_)) {
        return {lexeme, tokenLabels_[currentState_]};
    } else {
        input.retrace(); // neglect the char that caused the error
        int c = input.read();
        // std::cout << "c: " << c << std::endl;
        error.reportError("Unexpected character: " + std::to_string(c));
        return {"", ""};
    }
}

struct Rule {
    std::string regex;
    std::string type;
};

// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
using namespace std;
/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

void Analysis()
{
   std::vector<Rule> rules = {
        {"auto", "1"},      {"break", "2"},     {"case", "3"},      {"char", "4"},
        {"const", "5"},     {"continue", "6"},  {"default", "7"},   {"do", "8"},
        {"double", "9"},    {"else", "10"},     {"enum", "11"},     {"extern", "12"},
        {"float", "13"},    {"for", "14"},      {"goto", "15"},     {"if", "16"},
        {"int", "17"},      {"long", "18"},     {"register", "19"}, {"return", "20"},
        {"short", "21"},    {"signed", "22"},   {"sizeof", "23"},   {"static", "24"},
        {"struct", "25"},   {"switch", "26"},   {"typedef", "27"},  {"union", "28"},
        {"unsigned", "29"}, {"void", "30"},     {"volatile", "31"}, {"while", "32"},
        {"-", "33"},        {"--", "34"},       {"-=", "35"},       {"->", "36"},
        {"!", "37"},        {"!=", "38"},       {"%", "39"},        {"%=", "40"}, 
        {"&", "41"},        {"&&", "42"},       {"&=", "43"},       {"\\(", "44"}, {"\\)", "45"},
        {"\\*", "46"},      {"\\*=", "47"},     {",", "48"},        {"\\.", "49"},
        {"/", "50"},        {"/=", "51"},       {":", "52"},        {";", "53"},
        {"\\?", "54"},      {"\\[", "55"},      {"\\]", "56"},      {"^", "57"},
        {"\\^=", "58"},     {"{", "59"},        {"\\|", "60"},      {"\\|\\|", "61"},
        {"\\|=", "62"},     {"}", "63"},        {"~", "64"},        {"\\+", "65"},
        {"\\+\\+", "66"},   {"\\+=", "67"},     {"<", "68"},        {"<<", "69"},
        {"<<=", "70"},      {"<=", "71"},       {"=", "72"},        {"==", "73"},
        {">", "74"},        {">=", "75"},       {">>", "76"},       {">>=", "77"},
        {"\"", "78"},       {"\\/\\*@.*\\*\\/", "79"},              {"\\/\\/@l*", "79"}, 
        {"@d+(L|l)?|0(x|X)@h+", "80"},          {"@d+.@d+((e|E)(+|-)?@d+)?", "80"}, 
        {"(@a|_)@w+", "81"},                    {"\\%(-)?@d*(.@d+)?(d|ld|lld|f|lf|c|s|p|x|X|g)","81"},
        {"@a@d?","81"},     {"#", "82"},        {"'", "83"},        {"@s+", "space/ctrl"},
        {"\\\\(n|t|r)", "81"}
    };

    RegexParser* parser = new RegexParser();
    for (auto rule : rules) {
        std::string regex = rule.regex;
        std::string type = rule.type;
        parser->registerRegex(regex, type);
    }
    DFAConverter* converter = new DFAConverter(parser);


	std::string prog;
	read_prog(prog);    

    Lexer lexer(*converter, prog);

    bool newline = false;
    static int count = 1;
    for (Token token = lexer.getNextToken(); token.type != "" && token.type != "EOF"; token = lexer.getNextToken()) {
        // format: count:<value,type>
        if (token.type != "space/ctrl") {
            if (!newline) {
                newline = true;
            } else {
                std::cout << std::endl;
            }
            std::cout << count++ << ": <" << token.value << "," << token.type << ">";
        }
    }
}

int main() {
    Analysis();
    return 0;
}