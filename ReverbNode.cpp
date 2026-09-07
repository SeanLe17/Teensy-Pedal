#include <Arduino.h>

#include "ReverbNode.h"

ReverbNode::ReverbNode() {
}

void ReverbNode::setMix(float mix) {
    mix = constrain(mix, 1.0f, 10.0f);

    wet = 0.05f + (mix - 1.0f) * (0.55f / 9.0f);
    dry = 1.0f - wet;
}

void ReverbNode::setDecay(float decay) {
    decay = constrain(decay, 1.0f, 10.0f);

    feedback = 0.55f + (decay - 1.0f) * (0.33f / 9.0f);
}

AudioBlock* ReverbNode::process(AudioBlock* input) {
    if (input == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < 128; i++) {
        float x = input->data[i] / 32768.0f;

        float y1 = comb1[comb1Index];
        comb1Filter = y1 * (1.0f - damping) + comb1Filter * damping;
        comb1[comb1Index] = x + comb1Filter * feedback;
        if (++comb1Index == COMB1_LEN) {
            comb1Index = 0;
        }

        float y2 = comb2[comb2Index];
        comb2Filter = y2 * (1.0f - damping) + comb2Filter * damping;
        comb2[comb2Index] = x + comb2Filter * feedback;
        if (++comb2Index == COMB2_LEN) {
            comb2Index = 0;
        }

        float y3 = comb3[comb3Index];
        comb3Filter = y3 * (1.0f - damping) + comb3Filter * damping;
        comb3[comb3Index] = x + comb3Filter * feedback;
        if (++comb3Index == COMB3_LEN) {
            comb3Index = 0;
        }

        float y4 = comb4[comb4Index];
        comb4Filter = y4 * (1.0f - damping) + comb4Filter * damping;
        comb4[comb4Index] = x + comb4Filter * feedback;
        if (++comb4Index == COMB4_LEN) {
            comb4Index = 0;
        }

        float reverb = 0.25f * (y1 + y2 + y3 + y4);
        float output = dry * x + wet * reverb;

        if (output > 1.0f) {
            output = 1.0f;
        } else if (output < -1.0f) {
            output = -1.0f;
        }

        input->data[i] = (int16_t)(output * 32767.0f);
    }

    return input;
}