#include "BlockPool.h"

BlockPool :: BlockPool(){
    freeMask = 0xFFFFFFFF; // All 32 blocks start free (all 1s)
    for(int i = 0; i < NUM_BLOCKS; i++){ 
        pool[i].ref_count = 0;
        pool[i].index = i;
    }
}

AudioBlock* BlockPool:: allocate(){

    if(!freeMask){ // Return null if all blocks full
        return nullptr;
    }
    for(int i = 0; i < NUM_BLOCKS; i++){
        if (freeMask & (1u << i)){ // Bitwise AND
            freeMask &= ~(1u << i);
            pool[i].ref_count = 1;
            return &pool[i];
        } 
    }
    return nullptr;
}

void BlockPool:: retain(AudioBlock* block){
    block->ref_count++;
}

void BlockPool::release(AudioBlock* block){
    block->ref_count--;
    if(!block->ref_count){
        freeMask |= (1u << block->index);
    }
}