#pragma once

#include <stdint.h>

//retain increments ref_count
//release decrements ref_count and updates bitmap

struct AudioBlock {
    uint8_t ref_count;
    uint16_t index;
    int16_t data[128];  
};

class BlockPool {
public:
    BlockPool();

    AudioBlock* allocate();
    void retain(AudioBlock* block);
    void release(AudioBlock* block);

private:
    static constexpr int NUM_BLOCKS = 32;

    AudioBlock pool[NUM_BLOCKS];

    uint32_t freeMask;
};