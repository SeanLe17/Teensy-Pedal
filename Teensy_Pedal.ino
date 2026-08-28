#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// ---------- AUDIO OBJECTS ----------

AudioInputI2S        audioInput;
AudioEffectFreeverb  reverb;
AudioMixer4          mixer;
AudioOutputI2S       audioOutput;

// Measures the ORIGINAL guitar input
AudioAnalyzePeak     peak;


// ---------- CONNECTIONS ----------

// Dry guitar
AudioConnection patchCord1(audioInput, 0, mixer, 0);

// Guitar -> reverb
AudioConnection patchCord2(audioInput, 0, reverb, 0);

// Reverb -> mixer
AudioConnection patchCord3(reverb, 0, mixer, 1);

// Mixer -> headphones L/R
AudioConnection patchCord4(mixer, 0, audioOutput, 0);
AudioConnection patchCord5(mixer, 0, audioOutput, 1);

// Guitar -> peak measurement
AudioConnection patchCord6(audioInput, 0, peak, 0);


// ---------- AUDIO SHIELD ----------

AudioControlSGTL5000 audioShield;


void setup() {

  Serial.begin(115200);

  AudioMemory(40);

  // Turn on Audio Shield
  audioShield.enable();

  // Use LINE IN
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);

  // Input level
  audioShield.lineInLevel(5);

  // Headphone volume
  audioShield.volume(0.7);


  // ---------- MIX ----------

  // Dry guitar
  mixer.gain(0, 0.7);

  // Reverb
  mixer.gain(1, 0.6);

  // Unused mixer channels
  mixer.gain(2, 0);
  mixer.gain(3, 0);


  // ---------- REVERB ----------

  reverb.roomsize(0.8);
  reverb.damping(0.4);

  Serial.println("Guitar + Reverb started!");
}


void loop() {

  // Print original guitar input level
  if (peak.available()) {

    float level = peak.read();

    Serial.print("Input Peak: ");
    Serial.println(level, 4);
  }

  delay(20);
}
