#include "RegexParser.hpp"
#include <unordered_set>

using NFANodeSet = std::unordered_set<const NFANode*>;
const int INVALID = std::numeric_limits<int>::min();

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
    
    // 最小化
    // void minimize();
    // void splitGroups();
    // void initGroup();
    // void split(std::set<int>& curGroup);
    // void createGroup(std::set<int>& states);
    // void outputData();

public:
    int runDFA(const std::string& input, std::string& token);

private:
    NFA* nfa;
    std::unordered_map<NFANodeSet, int, NFANodeSetHash, NFANodeSetEqual> nfa2dfaMap;
    std::vector<NFANodeSet> dfa2nfaMap;
    std::vector<std::string> nodeToToken;
    int statesCount = 0;
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
    // minimize();
    // outputData();
}
// implement epilson closure
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
        auto [next1, next2] = node->getNext();
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
        for (int b = 0; b < 128; b++) {
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

    for (auto ele : g) {
        std::cout << ele->getID() << " ";
    }
    std::cout << std::endl;

    int state = statesCount++;
    nfa2dfaMap[g] = state;
    dfa2nfaMap.push_back(g);
    unchecked.push_back(state);
    goTo.push_back(std::vector<int>(128, -1));
    bool isAccepted = false;
    int TokenID = -1;
    for (auto node : g) {
        if (node->isAccepted()) {
            isAccepted = true;
            TokenID = node->getID();
            break;
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
        auto [next1, next2] = node->getNext();
        if (node->getEdgeType() == NFANode::CHAR && node->getPredicate()(b)) {
            result.insert(next1);
        }
    }
    return result;
}



int DFAConverter::runDFA(const std::string& input, std::string &token) {
    int state = 0;
    for (char c : input) {
        state = goTo[state][c];
        if (state == -1) {
            token = "invalid";
            return -1;
        }
    }
    token = tokens[state];
    return state;
}


