// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth

#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico



// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;

// Initialization of objects and global variables
static DaisySeed hw;                                             // Main object for managing Daisy hardware
static Ramo Pluck;                                                // Object for the Ramo synth
static Ramo Bass;
static Ramo Pad;                                                // Object for the Ramo synth

static Oscillator lfo_Pluck;
static Oscillator lfo_Bass;
static Oscillator lfo_Pad;                                     // LFO object for modulation

static Adsr AmpEnv_Pluck;                                          // Envelope object for amplitude
static Adsr AmpEnv_Bass;
static Adsr AmpEnv_Pad;                                          // Envelope object for amplitude

static Adsr FilterEnv_Pluck;                                       // Envelope object for filter
static Adsr FilterEnv_Bass;
static Adsr FilterEnv_Pad;                                       // Envelope object for filter

static Svf filter_Pluck;                                          // Filter object for the synth
static Svf filter_Bass;
static Svf filter_Pad;                                          // Filter object for the synth

AnalogDelay delay;                                              // Oggetto per gestire l'effetto delay

uint8_t wf_Pluck[n] = {1,1};                                          // Array for waveforms
uint8_t wf_Bass[n] = {1,1}; 
uint8_t wf_Pad[n] = {1,1}; 

float ef_Pluck[n] = {1, 1};                                            // Array for detune
float ef_Bass[n] = {1, 1};
float ef_Pad[n] = {1, 1};

float g_Pluck[n] = {0.5f, 0.5f};                                       // Array for amplitude
float g_Bass[n] = {0.5f, 0.5f};
float g_Pad[n] = {0.5f, 0.5f};                                       

float f_lfo_Pluck;                                                         
float f_lfo_Bass;                                                           
float f_lfo_Pad;                                                         

float shape_Pluck;                                                    
float shape_Bass;                                                     
float shape_Pad;                                                    

float f_Pluck;
float f_Bass;
float f_Pad;                                                         // Frequency

float Gate_Pluck;                                                     // Gate signal for Pluck
float Gate_Bass;                                                     // Gate signal for Bass
float Gate_Pad;                                                     // Gate signal for Pad

float amp_env_Pluck, amp_env_Bass, amp_env_Pad;                           // Amplitude envelope variables
float filt_env_Pluck, filt_env_Bass, filt_env_Pad;                           // Filter envelope variables

float Raw_Pluck, Raw_Bass, Raw_Pad;                                   // Raw signal variables

float out_noFX;                                                     // Output signal variable
                                                     // Delay time variable
float time, delayTime;  // Tempo del delay


const float MIN_DELAY = 0.02f;  // 20ms minimo delay
const float MAX_DELAY = 1.0f;   // 1000ms massimo delay


float LogScale(float pos, float min_value, float max_value) {
    // Implementa una scala logaritmica tra min_value e max_value
    float min_log = logf(min_value);
    float max_log = logf(max_value);
    float exp_scale = min_log + (max_log - min_log) * pos;
    return expf(exp_scale);
}

// Audio callback executed for each audio block
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {

                                                    
    for (size_t i = 0; i < size; i++) {
//--------------------------------------------------------------------Setta DelayTime
    delay.setDelayTime(delayTime);      // Imposta il tempo di delay
//-------------------------------------------------------------------Setta Shape
    shape_Pluck = lfo_Pluck.Process();                                   
    shape_Bass = lfo_Bass.Process();                                   
    shape_Pad = lfo_Pad.Process();                                   

    Pluck.SetShape(shape_Pluck);                                     
    Bass.SetShape(shape_Bass);                                     
    Pad.SetShape(shape_Pad);                                     
    
//---------------------------------------------------------------Setta Freq
    Pluck.SetFreq(f_Pluck);                                         
    Bass.SetFreq(f_Bass);                                         
    Pad.SetFreq(f_Pad);                 

//-----------------------------------------------------------------------------------------------------------Setta Inviluppo Amplitude
    amp_env_Pluck = AmpEnv_Pluck.Process(Gate_Pluck);                             
    amp_env_Bass = AmpEnv_Bass.Process(Gate_Bass);                             
    amp_env_Pad = AmpEnv_Pad.Process(Gate_Pad);                             

//-----------------------------------------------------------------------------------------------------------Setta iinviluppo filtro
    filt_env_Pluck = FilterEnv_Pluck.Process(Gate_Pluck);                          
    filt_env_Bass = FilterEnv_Bass.Process(Gate_Bass);                           
    filt_env_Pad = FilterEnv_Pad.Process(Gate_Pad);                           
//-----------------------------------------------------------------------------------------------------------Setta ft Filtri
                                    
    filter_Pluck.SetFreq(filt_env_Pluck);                             
    filter_Bass.SetFreq(filt_env_Bass);
    filter_Pad.SetFreq(filt_env_Pad);                            
//-----------------------------------------------------------------------------------------------------------Setta Amp 
    Pluck.SetAmp(amp_env_Pluck);                                     
    Bass.SetAmp(amp_env_Bass);                                     
    Pad.SetAmp(amp_env_Pad);                                     
//-----------------------------------------------------------------------------------------------------------Processa rami
    Raw_Pluck = Pluck.Process();
    Raw_Bass = Bass.Process();
    Raw_Pad = Pad.Process();
//-----------------------------------------------------------------------------------------------------------Filtra Rami
    filter_Pluck.Process(Raw_Pluck);
    filter_Bass.Process(Raw_Bass);
    filter_Pad.Process(Raw_Pad);
//-----------------------------------------------------------------------------------------------------------Somma tutto con guadagni
    out_noFX = filter_Pluck.Low() * 0.33 + filter_Bass.Low() * 0.33 + filter_Pad.Low() * 0.33; // Output processed signal (left channel)
//-----------------------------------------------------------------------------------------------------------Applica effetto
    float out_FX = delay.Process(out_noFX); // Process the output signal with the delay effect

//-----------------------------------------------------------------------------------------------------------Output
        out[0][i] = out_noFX;                                         // Output processed signal (left channel)
        out[1][i] = out_noFX;                                         // Output processed signal (right channel)
    }
}

int main() {
    hw.Configure();                                                             // Hardware configuration
    hw.Init();                                                                  // Hardware initialization

    hw.SetAudioBlockSize(4);                                                    // Set audio block size
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Set sample rate to 48kHz
//------------------------------------------------------------------------------Inizializza Ogetti audio e parametri
    Pluck.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Bass.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pad.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth

    Pluck.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Bass.SetWaveforms(wf_Bass);                                                      // Set waveforms
    Pad.SetWaveforms(wf_Pad);                                                      // Set waveforms

    Pluck.SetFreq(440.0f);                                                       // Set frequency
    Bass.SetFreq(440.0f);                                                       // Set frequency
    Pad.SetFreq(440.0f);                                                       // Set frequency

    Pluck.SetDetune(ef_Pluck);                                                         // Set detune
    Bass.SetDetune(ef_Bass);                                                         // Set detune
    Pad.SetDetune(ef_Pad);                                                         // Set detune

    Pluck.SetAmp(g_Pluck);                                                             // Set amplitude
    Bass.SetAmp(g_Bass);                                                             // Set amplitude
    Pad.SetAmp(g_Pad);                                                             // Set amplitude

    Pluck.SetShape(0);                                                        // Set shape modulation
    Bass.SetShape(0);                                                        // Set shape modulation
    Pad.SetShape(0);                                                        // Set shape modulation
//----------------------------------------------------------------------------------Inizializza LFO e parametri
    lfo_Pluck.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Bass.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pad.Init(hw.AudioSampleRate());                                          // Initialize LFO

    lfo_Pluck.SetWaveform(Oscillator::WAVE_TRI);                                 // Set LFO waveform
    lfo_Bass.SetWaveform(Oscillator::WAVE_TRI);                                 // Set LFO waveform
    lfo_Pad.SetWaveform(Oscillator::WAVE_TRI);                                 // Set LFO waveform

    lfo_Pluck.SetFreq(0.5f);                                                    // Set LFO frequency
    lfo_Bass.SetFreq(0.5f);                                                    // Set LFO frequency
    lfo_Pad.SetFreq(0.5f);                                                    // Set LFO frequency

    lfo_Pluck.SetAmp(1);                                                    // Set LFO amplitude
    lfo_Bass.SetAmp(1);                                                    // Set LFO amplitude
    lfo_Pad.SetAmp(1);                                                    // Set LFO amplitude

//-----------------------------------------------------------------------------Inizializza AMP ADSR e parametri
    AmpEnv_Pluck.Init(hw.AudioSampleRate());                                       // Initialize envelope
    AmpEnv_Bass.Init(hw.AudioSampleRate());                                       // Initialize envelope
    AmpEnv_Pad.Init(hw.AudioSampleRate());                                       // Initialize envelope

    AmpEnv_Pluck.SetTime(ADSR_SEG_ATTACK, 0.1f);                                  // Set attack time
    AmpEnv_Bass.SetTime(ADSR_SEG_ATTACK, 0.1f);                                  // Set attack time
    AmpEnv_Pad.SetTime(ADSR_SEG_ATTACK, 0.1f);                                  // Set attack time

    AmpEnv_Pluck.SetTime(ADSR_SEG_DECAY, 0.1f);                                   // Set decay time
    AmpEnv_Bass.SetTime(ADSR_SEG_DECAY, 0.1f);                                   // Set decay time
    AmpEnv_Pad.SetTime(ADSR_SEG_DECAY, 0.1f);                                   // Set decay time

    AmpEnv_Pluck.SetSustainLevel(0.5f);                                          // Set sustain level
    AmpEnv_Bass.SetSustainLevel(0.5f);                                          // Set sustain level
    AmpEnv_Pad.SetSustainLevel(0.5f);                                          // Set sustain level

    AmpEnv_Pluck.SetTime(ADSR_SEG_RELEASE, 0.1f);                              // Set release time
    AmpEnv_Bass.SetTime(ADSR_SEG_RELEASE, 0.1f);                              // Set release time
    AmpEnv_Pad.SetTime(ADSR_SEG_RELEASE, 0.1f);                              // Set release time
    AmpEnv_Pad.SetTime(ADSR_SEG_RELEASE, 0.1f);                              // Set release time
//-----------------------------------------------------------------------------------Inizializza FILTER e parametri
    filter_Pluck.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Bass.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pad.Init(hw.AudioSampleRate());                                        // Initialize filter

    filter_Pluck.SetFreq(1000.0f);                                               // Set cutoff frequency
    filter_Bass.SetFreq(1000.0f);                                               // Set cutoff frequency
    filter_Pad.SetFreq(1000.0f);                                               // Set cutoff frequency

    filter_Pluck.SetRes(0.5f);                                             // Set resonance
    filter_Bass.SetRes(0.5f);                                             // Set resonance
    filter_Pad.SetRes(0.5f);                                             // Set resonance

    filter_Pluck.SetDrive(0.5f);                                               // Set filter amplitude
    filter_Bass.SetDrive(0.5f);                                               // Set filter amplitude
    filter_Pad.SetDrive(0.5f);                                               // Set filter amplitude

//-----------------------------------------------------------------------------------Inizializza FILTER ADSR e parametri
    FilterEnv_Pluck.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Bass.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Pad.Init(hw.AudioSampleRate());                                   // Initialize envelope

    FilterEnv_Pluck.SetTime(ADSR_SEG_ATTACK, 0.1f);                              // Set attack time
    FilterEnv_Bass.SetTime(ADSR_SEG_ATTACK, 0.1f);                              // Set attack time
    FilterEnv_Pad.SetTime(ADSR_SEG_ATTACK, 0.1f);                              // Set attack time

    FilterEnv_Pluck.SetTime(ADSR_SEG_DECAY, 0.1f);                               // Set decay time
    FilterEnv_Bass.SetTime(ADSR_SEG_DECAY, 0.1f);                               // Set decay time
    FilterEnv_Pad.SetTime(ADSR_SEG_DECAY, 0.1f);                               // Set decay time

    FilterEnv_Pluck.SetSustainLevel(0.5f);                                      // Set sustain level
    FilterEnv_Bass.SetSustainLevel(0.5f);                                      // Set sustain level
    FilterEnv_Pad.SetSustainLevel(0.5f);                                      // Set sustain level

    FilterEnv_Pluck.SetTime(ADSR_SEG_RELEASE, 0.1f);                          // Set release time
    FilterEnv_Bass.SetTime(ADSR_SEG_RELEASE, 0.1f);                          // Set release time
    FilterEnv_Pad.SetTime(ADSR_SEG_RELEASE, 0.1f);                          // Set release time
//--------
    delay.Init(hw.AudioSampleRate());                                       // Inizializza il delay con il sample rate corrente
    
    delay.setDelayTime(delayTime);      // Imposta il tempo di delay
    delay.setFeedback(0.5f);        // Imposta il feedback
    delay.setMix(0.5f);                  // Imposta il mix
    delay.setDepth(0.5f);              // Imposta la profondità del pitch shifter
    

    


    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {
        //f_lfo_Bass = ..
        //f_lfo_Pluck = ..
        //f_lfo_Pad = ..

        //f_Pluck = ..
        //f_Bass = ..
        //f_Pad = ..

        //Gate_Pluck = ..
        //Gate_Bass = ..
        //Gate_Pad = ..

        //time =..
        //delayTime =logscale(time, MIN_DELAY, MAX_DELAY); // Imposta il tempo di delay
        

        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
