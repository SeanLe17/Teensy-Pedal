#include <Arduino.h>
#include <math.h>
#include "DistortionNode.h"

DistortionNode :: DistortionNode()
: gain(25.0f),
  level(0.25f)
{
}

void DistortionNode :: setGain(float drive){
    drive = constrain(drive, 1.0f, 10.0f);

    // 1-10 knob → 1x-25x internal gain
    gain = 1.0f + (drive - 1.0f) * (24.0f / 9.0f);
}

void DistortionNode :: setLevel(float vol){
    vol = constrain(vol, 1.0f, 10.0f);

    // 1-10 knob → 0.1-1.0 output multiplier
    level = vol / 10.0f;
}

AudioBlock* DistortionNode:: process(AudioBlock* block){
    if(block == nullptr){
        return nullptr;
    }
    for (int i = 0; i < 128; i++) { // Accesses each individual sample in a block
         // Normalize
        float x = block->data[i] / 32768.0f;

        // Apply drive
        x *= gain;

        // Nonlinear waveshaping
        x = tanhf(x);

        // Set volume
        x *= level; 

        // Convert back to 16-bit PCM
        block->data[i] = (int16_t)(x * 32767.0f);
        }
    return block;
    


}