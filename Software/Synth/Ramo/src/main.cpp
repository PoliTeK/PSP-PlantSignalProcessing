// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth

#define ADC_CHANNEL_NUM 2 // Number of ADC channels

// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;

// Initialization of objects and global variables
static DaisySeed hw;                                             // Main object for managing Daisy hardware
static Ramo ramo;                                                // Object for the Ramo synth

uint8_t wf[n] = {1, 1};                                          // Array for waveforms
float ef[n] = {1, 1};                                            // Array for detune
float g[n] = {0.5f, 0.5f};                                       // Array for amplitude

float f;                                                         // Frequency
float shape;                                                     // Shape modulation

// ADC configuration
AdcChannelConfig adcConfig[ADC_CHANNEL_NUM];                     // Configuration array for ADC channels

void InitHardware() {
    // Initialize the ADC pins for the potentiometers
    for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
        adcConfig[i].InitSingle(hw.GetPin(15 + i));
    }
    hw.adc.Init(adcConfig, ADC_CHANNEL_NUM);                     // Initialize ADC with N channels
    hw.adc.Start();                                              // Start ADC conversion
}

// Audio callback executed for each audio block
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    float sig;
    ramo.SetFreq(f);                                              // Set frequency
    ramo.SetShape(shape);                                         // Set waveform shape

    for (size_t i = 0; i < size; i++) {
        sig = ramo.Process();                                     // Process the audio signal
        out[0][i] = sig;                                         // Output processed signal (left channel)
        out[1][i] = sig;                                         // Output processed signal (right channel)
    }
}

int main() {
    hw.Configure();                                                             // Hardware configuration
    hw.Init();                                                                  // Hardware initialization

    hw.SetAudioBlockSize(4);                                                    // Set audio block size
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Set sample rate to 48kHz

    ramo.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    ramo.SetWaveforms(wf);                                                      // Set waveforms
    ramo.SetFreq(440.0f);                                                       // Set frequency
    ramo.SetDetune(ef);                                                         // Set detune
    ramo.SetAmp(g);                                                             // Set amplitude
    ramo.SetShape(0.5f);                                                        // Set shape modulation

    InitHardware();                                                             // Initialize potentiometers
    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {
        f = 220.0f + 220.0f * hw.adc.GetFloat(0);                                // Read potentiometer 1 and calculate frequency
        shape = hw.adc.GetFloat(1);                                              // Read potentiometer 2 and calculate waveform modulation

        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
