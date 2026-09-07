#include "RuntimeBridge.h"

RuntimeBridge::RuntimeBridge(AudioRuntime& runtimeRef,
                             BlockPool& poolRef,
                             ProcessingNode& inputNodeRef)
    : AudioStream(1, inputQueueArray),
      runtime(runtimeRef),
      pool(poolRef),
      inputNode(inputNodeRef) {
}

void RuntimeBridge::update() {

    //Copy teensy inputs into my architecture
    audio_block_t* nativeInput = receiveReadOnly(0);
    if (nativeInput == nullptr) {
        return;
    }

    AudioBlock* customInput = pool.allocate();
    if (customInput == nullptr) {
        release(nativeInput);
        return;
    }
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        customInput->data[i] = nativeInput->data[i];
    }

    release(nativeInput);

    if (!runtime.submitInput(&inputNode, customInput)) {
        pool.release(customInput);
        return;
    }

    runtime.runTick();

    //Route my output back through teensy
    AudioBlock* customOutput = runtime.takeOutput();
    if (customOutput == nullptr) {
        return;
    }
    audio_block_t* nativeOutput = allocate();

    if (nativeOutput == nullptr) {
        pool.release(customOutput);
        return;
    }
    
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        nativeOutput->data[i] = customOutput->data[i];
    }

    transmit(nativeOutput);
    release(nativeOutput);
    pool.release(customOutput);




    
}