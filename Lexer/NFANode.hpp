#include <iostream>
#include "Predicate.hpp"

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