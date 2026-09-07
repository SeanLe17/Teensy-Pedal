#pragma once
#include <Audio.h>
#include "AudioRuntime.h"

class RuntimeBridge : public AudioStream {
public:
    RuntimeBridge(AudioRuntime& runtime,
                  BlockPool& pool,
                  ProcessingNode& inputNode);

    void update() override;

private:
    audio_block_t* inputQueueArray[1];

    AudioRuntime& runtime;
    BlockPool& pool;
    ProcessingNode& inputNode;
};