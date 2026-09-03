#include "ProcessingNode.h"

bool ProcessingNode::hasPendingInput() const {
    return pendingInput!= nullptr;
}

bool ProcessingNode::acceptInput(AudioBlock* block) {
    if (block == nullptr) {
        return false;
    }

    if (pendingInput != nullptr) {
        return false;
    }

    pendingInput = block;
    return true;
}

