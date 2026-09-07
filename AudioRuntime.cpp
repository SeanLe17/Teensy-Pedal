#include "AudioRuntime.h"

AudioRuntime :: AudioRuntime(BlockPool& pool)
    :blockPool(pool),
    nodeCount(0),
    connectionCount(0),
    outputNode(nullptr),
    pendingOutput(nullptr)
{

}

bool AudioRuntime :: addNode(ProcessingNode* node){
    if(node != nullptr && nodeCount < MAX_NODES){
        nodes[nodeCount] = node;
        nodeCount++;
        return true;
    }
    return false;
}

bool AudioRuntime :: addConnection(ProcessingNode* source, ProcessingNode* destination){
    if(source != nullptr &&  destination != nullptr && connectionCount < MAX_CONNECTIONS){
        connections[connectionCount] = {source, destination};
        connectionCount++;
        return true;
    }
    return false;
}

bool AudioRuntime :: submitInput(ProcessingNode* input, AudioBlock* block){
    if(input == nullptr && block == nullptr){
        return false;
    }
    return input->acceptInput(block);
}

bool AudioRuntime:: setOutputNode(ProcessingNode* node){
    if(node == nullptr){
        return false;
    }
    outputNode = node;
    return true;
}

AudioBlock* AudioRuntime:: takeOutput(){
    AudioBlock* output = pendingOutput;
    pendingOutput = nullptr;
    return output;
}

void AudioRuntime::routeOutput(ProcessingNode* source,
                               AudioBlock* block) {
    if (block == nullptr) {
        return;
    }
    if(source == outputNode){
        if(pendingOutput == nullptr){
            pendingOutput = block;
            return;
        }
        blockPool.release(block);
        return;
    }

    for (int i = 0; i < connectionCount; i++) {
        if (connections[i].source == source) {
            blockPool.retain(block);

            if (!connections[i].destination->acceptInput(block)) {
                blockPool.release(block);
            }
        }
    }

    blockPool.release(block);
}

void AudioRuntime:: runTick(){
    for(int i = 0; i < nodeCount; i++){
        ProcessingNode* node = nodes[i];

        if(!node->hasPendingInput()){
                continue;
        }

        AudioBlock* input = node->takeInput(); 
        AudioBlock* output = node->process(input);
        routeOutput(node, output);
        }
    }