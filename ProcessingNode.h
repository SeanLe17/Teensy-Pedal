#pragma once
#include "BlockPool.h"

//pendingInput is mailbox


class ProcessingNode {
public:
    virtual AudioBlock* process(AudioBlock* input) = 0;

    bool hasPendingInput() const;
    bool acceptInput(AudioBlock* block);
    AudioBlock* takeInput();

private:
    AudioBlock* pendingInput = nullptr;
};