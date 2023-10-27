#include <vector>
#include <queue>
#include "NFANode.hpp"
#include "Predicate.hpp"

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
        std::cout << "newStart: " << *newStart << std::endl;
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
            auto [next1, next2] = node->getNext();
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
            std::cout << "empty NFA" << std::endl;
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
            std::cout << *node << std::endl;

            auto [next1, next2] = node->getNext();
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
