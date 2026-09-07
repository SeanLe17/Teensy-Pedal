#pragma once
#include "ProcessingNode.h"

class ReverbNode : public ProcessingNode {
public:
    ReverbNode();

    void setMix(float mix);
    void setDecay(float decay);
    AudioBlock* process(AudioBlock* input) override;

private:
    static constexpr int COMB1_LEN = 1116;
    static constexpr int COMB2_LEN = 1188;
    static constexpr int COMB3_LEN = 1277;
    static constexpr int COMB4_LEN = 1356;

    float comb1[COMB1_LEN] = {};
    float comb2[COMB2_LEN] = {};
    float comb3[COMB3_LEN] = {};
    float comb4[COMB4_LEN] = {};

    int comb1Index = 0;
    int comb2Index = 0;
    int comb3Index = 0;
    int comb4Index = 0;

    float comb1Filter = 0.0f;
    float comb2Filter = 0.0f;
    float comb3Filter = 0.0f;
    float comb4Filter = 0.0f;

    float feedback = 0.78f;
    float damping = 0.25f;
    float wet = 0.35f;
    float dry = 0.80f;
};