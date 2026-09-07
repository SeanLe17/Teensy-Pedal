#pragma once
#include "ProcessingNode.h"

class DistortionNode : public ProcessingNode {
public:
    DistortionNode();

    void setGain(float drive);
    void setLevel(float level);

    AudioBlock* process(AudioBlock* input) override;

private:
    float gain;
    float level;
};