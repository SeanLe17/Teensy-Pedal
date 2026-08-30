#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//DSP classes
class Distortion : public AudioStream { // Child class of audiostream
public:
    Distortion() // Constructor, executes when object is created
        : AudioStream(1, inputQueueArray), gain(25.0f) {} // Creates the audiostream portion of the object

    void update(void) override { // Inherited virtual class
        audio_block_t *block = receiveWritable(0); // gets copy of audio block if necessary

        if (!block) {
            return;
        }

        //DSP happens here
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) { // Accesses each individual sample in a block
            // 1. Normalize
            float x = block->data[i] / 32768.0f;

            // 2. Apply drive
            x *= gain;

            // 3. Nonlinear waveshaping
            x = tanhf(x);

            // 4. Convert back to 16-bit PCM
            block->data[i] = (int16_t)(x * 32767.0f);
        }

        transmit(block);
        release(block);
    }
  private:
    audio_block_t *inputQueueArray[1];
    float gain;
  };




class LowPassBiquad : public AudioStream {
public:
    LowPassBiquad()
        : AudioStream(1, inputQueueArray),
          b0(0.0f), b1(0.0f), b2(0.0f),
          a1(0.0f), a2(0.0f),
          x1(0.0f), x2(0.0f),
          y1(0.0f), y2(0.0f)
    {
        setLowpass(18000.0f, 0.707f);
    }

    void setLowpass(float cutoffHz, float Q)
    {
        const float fs = AUDIO_SAMPLE_RATE_EXACT;

        float omega = 2.0f * PI * cutoffHz / fs;
        float sinOmega = sinf(omega);
        float cosOmega = cosf(omega);
        float alpha = sinOmega / (2.0f * Q);

        float a0 = 1.0f + alpha;

        // RBJ low-pass coefficients
        b0 = (1.0f - cosOmega) / 2.0f;
        b1 = 1.0f - cosOmega;
        b2 = (1.0f - cosOmega) / 2.0f;

        a1 = -2.0f * cosOmega;
        a2 = 1.0f - alpha;

        // Normalize everything by a0
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
    }

    void update(void) override
    {
        audio_block_t *block = receiveWritable(0);

        if (!block) {
            return;
        }

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

            // Convert int16 PCM -> float [-1, 1]
            float x = block->data[i] / 32768.0f;

            // Biquad difference equation
            float y =
                b0 * x +
                b1 * x1 +
                b2 * x2 -
                a1 * y1 -
                a2 * y2;

            // Shift input history
            x2 = x1;
            x1 = x;

            // Shift output history
            y2 = y1;
            y1 = y;

            // Safety clamp before converting back to int16
            if (y > 1.0f) {
                y = 1.0f;
            }
            else if (y < -1.0f) {
                y = -1.0f;
            }

            // Convert back to 16-bit PCM
            block->data[i] = (int16_t)(y * 32767.0f);
        }

        transmit(block);
        release(block);
    }

private:
    audio_block_t *inputQueueArray[1];

    // Filter coefficients
    float b0, b1, b2;
    float a1, a2;

    // Persistent filter state
    float x1, x2;
    float y1, y2;
};

class customDelay : public AudioStream {
public:
 customDelay()
  : AudioStream(1, inputQueueArray),
  delaySamples(22050),
  writePointer(0),
  readPointer((writePointer-delaySamples+buffer_size)%buffer_size),
  wet(0.0f),
  dry(1.0f)
  {
    //Constructor function execution. Fill buffer using loop here
    for(int i = 0; i < buffer_size; i++){
      delayBuffer[i]=0;
    }
  }
  void update(void) override {
    audio_block_t *block = receiveWritable(0);
    if(!block){
      return;
    }
    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++){
      int16_t input = block->data[i];
      delayBuffer[writePointer] = input;
      float output = input * dry + delayBuffer[readPointer] * wet;

      if(output > 32767.0f){
        output = 32767.0f;
      }
      else if(output < -32768.0f){
        output = -32768.0f;
      }

      block->data[i] = (int16_t)output;

      writePointer = (writePointer + 1) % buffer_size;
      readPointer = (readPointer + 1) % buffer_size;
    }

    transmit(block);
    release(block);

  }
private:
  audio_block_t * inputQueueArray[1];
  static constexpr int buffer_size = 44100;
  int16_t delayBuffer[buffer_size];
  int delaySamples; // How long delay is set to 
  int writePointer;
  int readPointer;
  float wet;
  float dry;


}; 


AudioInputI2S  audioInput;
AudioOutputI2S audioOutput;
AudioAnalyzePeak peak;

//Construct DSP vertices
Distortion distortion;
LowPassBiquad lowpass;
customDelay delay1;

// Build Graph
AudioConnection patchCord1(audioInput, 0, distortion, 0);
AudioConnection patchCord2(distortion, 0, lowpass, 0);
AudioConnection patchCord3(lowpass, 0, delay1, 0);

AudioConnection patchCord4(delay1, 0, audioOutput, 0);
AudioConnection patchCord5(delay1, 0, audioOutput, 1);

// Parallel branch only for measuring ORIGINAL input
AudioConnection patchCordPeak(audioInput, 0, peak, 0);


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
