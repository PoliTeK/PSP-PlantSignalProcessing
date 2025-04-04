// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth


// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;

// Initialization of objects and global variables
static DaisySeed hw;                                             // Main object for managing Daisy hardware
static Ramo Pluck;                                                // Object for the Ramo synth
static Ramo Key;
static Ramo Pad;                                                // Object for the Ramo synth

uint8_t wf_Pluck[n] = {1,1};                                          // Array for waveforms
uint8_t wf_Key[n] = {1,1}; 
uint8_t wf_Pad[n] = {1,1}; 

float ef_Pluck[n] = {1, 1};                                            // Array for detune
float ef_Key[n] = {1, 1};
float ef_Pad[n] = {1, 1};

float g_Pluck[n] = {0.5f, 0.5f};                                       // Array for amplitude
float g_Key[n] = {0.5f, 0.5f};
float g_Pad[n] = {0.5f, 0.5f};                                       

float f_Pluck;                                                         // Frequency
float f_Key;                                                         // Frequency   
float f_Pad;                                                         // Frequency

float shape_Pluck;                                                     // Shape modulation
float shape_Key;                                                     // Shape modulation
float shape_Pad;                                                     // Shape modulation




// Audio callback executed for each audio block
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    

    for (size_t i = 0; i < size; i++) {
                                           
        out[0][i] = in[0][i];                                         // Output processed signal (left channel)
        out[1][i] = in[1][i];                                         // Output processed signal (right channel)
    }
}

int main() {
    hw.Configure();                                                             // Hardware configuration
    hw.Init();                                                                  // Hardware initialization

    hw.SetAudioBlockSize(4);                                                    // Set audio block size
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Set sample rate to 48kHz

    Pluck.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Key.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pad.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth

    Pluck.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Key.SetWaveforms(wf_Key);                                                      // Set waveforms
    Pad.SetWaveforms(wf_Pad);                                                      // Set waveforms

    Pluck.SetFreq(440.0f);                                                       // Set frequency
    Key.SetFreq(440.0f);                                                       // Set frequency
    Pad.SetFreq(440.0f);                                                       // Set frequency

    Pluck.SetDetune(ef_Pluck);                                                         // Set detune
    Key.SetDetune(ef_Key);                                                         // Set detune
    Pad.SetDetune(ef_Pad);                                                         // Set detune

    Pluck.SetAmp(g_Pluck);                                                             // Set amplitude
    Key.SetAmp(g_Key);                                                             // Set amplitude
    Pad.SetAmp(g_Pad);                                                             // Set amplitude

    Pluck.SetShape(0f);                                                        // Set shape modulation
    Key.SetShape(0f);                                                        // Set shape modulation
    Pad.SetShape(0f);                                                        // Set shape modulation


    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {
       
        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
