#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "BlockPool.h"
#include "AudioRuntime.h"
#include "DistortionNode.h"
#include "ReverbNode.h"
#include "RuntimeBridge.h"

// ---------- CUSTOM RUNTIME ----------

BlockPool customPool;
AudioRuntime runtime(customPool);
DistortionNode distortion;
ReverbNode reverb;
RuntimeBridge bridge(runtime, customPool, distortion);

// ---------- TEENSY AUDIO HARDWARE ----------

AudioInputI2S audioInput;
AudioOutputI2S audioOutput;
AudioControlSGTL5000 audioShield;

// ---------- TEENSY AUDIO GRAPH ----------
//
// Native Teensy audio:
// AudioInputI2S → RuntimeBridge → AudioOutputI2S
//
// Inside RuntimeBridge:
// native audio_block_t → custom runtime → native audio_block_t

AudioConnection inputToBridge(audioInput, 0, bridge, 0);
AudioConnection bridgeToLeft(bridge, 0, audioOutput, 0);
AudioConnection bridgeToRight(bridge, 0, audioOutput, 1);

void setup() {
    Serial.begin(115200);

    // Native Teensy Audio Library block pool.
    AudioMemory(40);

    // Build custom graph:
    // DistortionNode is both the entry and final node for now.
    if (!runtime.addNode(&distortion) ||
        !runtime.addNode(&reverb) ||
        !runtime.addConnection(&distortion, &reverb) ||
        !runtime.setOutputNode(&reverb)) {

        Serial.println("Custom runtime graph setup failed.");

        while (true) {
            delay(1000);
        }
    }

    distortion.setGain(7.0f);
    distortion.setLevel(5.0f);
    reverb.setDecay(7.0f);
    reverb.setMix(7.0f);

    // Audio Shield / SGTL5000 configuration.
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN);
    audioShield.lineInLevel(5);
    audioShield.volume(0.7f);

    Serial.println("Custom runtime distortion ready.");
}

void loop() {
    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 1000) {
        lastPrint = millis();

        Serial.print("Teensy Audio CPU: ");
        Serial.print(AudioProcessorUsage());
        Serial.print("%  Max: ");
        Serial.print(AudioProcessorUsageMax());

        Serial.print("%  Native Audio Mem: ");
        Serial.print(AudioMemoryUsage());
        Serial.print("  Max: ");
        Serial.println(AudioMemoryUsageMax());
    }
}