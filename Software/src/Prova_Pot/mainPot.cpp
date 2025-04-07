// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth

//#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico

#define ADC_CH 6 // Numero di canali ADC da utilizzare

// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;

// Initialization of objects and global variables
static DaisySeed hw;                                             // Main object for managing Daisy hardware
AdcChannelConfig adcConfig[ADC_CH]; // Array di configurazione per 3 canali ADC
static Metro      tick1, tick2, tick3; // Metronome objects for timing


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

//AnalogDelay delay;                                              // Oggetto per gestire l'effetto delay

uint8_t wf_Pluck[n] = {2,1};                                          // Array for waveforms
uint8_t wf_Bass[n] = {3,3}; 
uint8_t wf_Pad[n] = {3,2}; 

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

bool Gate_Pluck;                                                     // Gate signal for Pluck
bool Gate_Bass;                                                     // Gate signal for Bass
bool Gate_Pad;                                                      // Gate signal for Pad


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


void InitHardware() {
    
    // Inizializzazione dei tre pin ADC per i potenziometri
    adcConfig[0].InitSingle(hw.GetPin(15)); // Primo potenziometro
    adcConfig[1].InitSingle(hw.GetPin(16)); // Secondo potenziometro
    adcConfig[2].InitSingle(hw.GetPin(17)); // Terzo potenziometro
    adcConfig[3].InitSingle(hw.GetPin(18)); // Quarto potenziometro
    adcConfig[4].InitSingle(hw.GetPin(19)); // Quinto potenziometro
    adcConfig[5].InitSingle(hw.GetPin(20)); // Sesto potenziometro
    hw.adc.Init(adcConfig, ADC_CH);              // Inizializzazione ADC con 3 canali
    hw.adc.Start();                         // Avvio della conversione ADC
}
// Audio callback executed for each audio block
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {

                                                    
    for (size_t i = 0; i < size; i++) {

        if(tick1.Process())
        {
            Gate_Pad = !Gate_Pad; // Attiva il gate per Pluck
        } 
        if(tick2.Process())
        {
            Gate_Pluck = !Gate_Pluck; // Attiva il gate per Bass
        }
        if(tick3.Process())
        {
            Gate_Bass = !Gate_Bass; // Attiva il gate per Pad
        }
//--------------------------------------------------------------------Setta DelayTime
    //delay.setDelayTime(delayTime);      // Imposta il tempo di delay
//-------------------------------------------------------------------Setta Shape
    lfo_Pluck.SetFreq(f_lfo_Pluck);
    lfo_Bass.SetFreq(f_lfo_Bass);
    lfo_Pad.SetFreq(f_lfo_Pad);
    
    lfo_Pad.SetAmp(0.3f); // Set LFO amplitude for Pad
    lfo_Bass.SetAmp(0.3f); // Set LFO amplitude for Bass
    lfo_Pluck.SetAmp(0.3f); // Set LFO amplitude for Pluck


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
    filter_Pluck.SetFreq(3000.0f - filt_env_Pluck*2000.0f);                             
    filter_Bass.SetFreq(200.0f + filt_env_Bass*5000.0f);
    filter_Pad.SetFreq(500.0f + filt_env_Pad*8000.0f);                            
//-----------------------------------------------------------------------------------------------------------Setta Amp 
    Pluck.SetGain(amp_env_Pluck);                                     
    Bass.SetGain(amp_env_Bass);                                     
    Pad.SetGain(amp_env_Pad);                                     
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
    //float out_FX = delay.Process(out_noFX); // Process the output signal with the delay effect

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

    lfo_Pluck.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform

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

    AmpEnv_Pluck.SetTime(ADSR_SEG_ATTACK, 0.05f);                                  // Set attack time
    AmpEnv_Bass.SetTime(ADSR_SEG_ATTACK, 0.15f);                                  // Set attack time
    AmpEnv_Pad.SetTime(ADSR_SEG_ATTACK, 12.0f);                                  // Set attack time

    AmpEnv_Pluck.SetTime(ADSR_SEG_DECAY, 0.27f);                                   // Set decay time
    AmpEnv_Bass.SetTime(ADSR_SEG_DECAY, 0.5f);                                   // Set decay time
    AmpEnv_Pad.SetTime(ADSR_SEG_DECAY, 0.5f);                                   // Set decay time

    AmpEnv_Pluck.SetSustainLevel(0.0f);                                          // Set sustain level
    AmpEnv_Bass.SetSustainLevel(0.75f);                                          // Set sustain level
    AmpEnv_Pad.SetSustainLevel(0.4f);                                          // Set sustain level

    AmpEnv_Pluck.SetTime(ADSR_SEG_RELEASE, 1.0f);                              // Set release time
    AmpEnv_Bass.SetTime(ADSR_SEG_RELEASE, 0.33f);                              // Set release time
    AmpEnv_Pad.SetTime(ADSR_SEG_RELEASE, 1.0f);                              // Set release time
    
//-----------------------------------------------------------------------------------Inizializza FILTER e parametri
    filter_Pluck.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Bass.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pad.Init(hw.AudioSampleRate());                                        // Initialize filter

    filter_Pluck.SetFreq(1000.0f);                                               // Set cutoff frequency
    filter_Bass.SetFreq(1000.0f);                                               // Set cutoff frequency
    filter_Pad.SetFreq(1000.0f);                                               // Set cutoff frequency

    filter_Pluck.SetRes(0.1f);                                             // Set resonance
    filter_Bass.SetRes(0.05f);                                             // Set resonance
    filter_Pad.SetRes(0.1f);                                             // Set resonance

    filter_Pluck.SetDrive(0.5f);                                               // Set filter amplitude
    filter_Bass.SetDrive(0.5f);                                               // Set filter amplitude
    filter_Pad.SetDrive(0.5f);                                               // Set filter amplitude

//-----------------------------------------------------------------------------------Inizializza FILTER ADSR e parametri
    FilterEnv_Pluck.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Bass.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Pad.Init(hw.AudioSampleRate());                                   // Initialize envelope

    FilterEnv_Pluck.SetTime(ADSR_SEG_ATTACK, 0.1f);                              // Set attack time
    FilterEnv_Bass.SetTime(ADSR_SEG_ATTACK, 10.0f);                              // Set attack time
    FilterEnv_Pad.SetTime(ADSR_SEG_ATTACK, 9.0f);                              // Set attack time

    FilterEnv_Pluck.SetTime(ADSR_SEG_DECAY, 0.03f);                               // Set decay time
    FilterEnv_Bass.SetTime(ADSR_SEG_DECAY, 1.0f);                               // Set decay time
    FilterEnv_Pad.SetTime(ADSR_SEG_DECAY, 0.4f);                               // Set decay time

    FilterEnv_Pluck.SetSustainLevel(0.0);                                      // Set sustain level
    FilterEnv_Bass.SetSustainLevel(0.60f);                                      // Set sustain level
    FilterEnv_Pad.SetSustainLevel(0.4f);                                      // Set sustain level

    FilterEnv_Pluck.SetTime(ADSR_SEG_RELEASE, 0);                          // Set release time
    FilterEnv_Bass.SetTime(ADSR_SEG_RELEASE, 0.50f);                          // Set release time
    FilterEnv_Pad.SetTime(ADSR_SEG_RELEASE, 0.6f);                          // Set release time
//--------
    //delay.Init(hw.AudioSampleRate());                                       // Inizializza il delay con il sample rate corrente
    
    //delay.setDelayTime(1000);      // Imposta il tempo di delay
    //delay.setFeedback(0.5f);        // Imposta il feedback
    //delay.setMix(0.5f);                  // Imposta il mix
    //delay.setDepth(0.5f);              // Imposta la profondità del pitch shifter
    

    
    tick1.Init(0.25f, hw.AudioSampleRate());
    tick2.Init(0.75f, hw.AudioSampleRate());
    tick3.Init(1.5f, hw.AudioSampleRate());
    InitHardware();                                                             // Initialize hardware (LED and ADC)
    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {

        float knob1 = hw.adc.GetFloat(0);
        float knob2 = hw.adc.GetFloat(1);
        float knob3 = hw.adc.GetFloat(2);
        float knob4 = hw.adc.GetFloat(3);
        float knob5 = hw.adc.GetFloat(4);
        float knob6 = hw.adc.GetFloat(5);                                                         // Update controls


        f_lfo_Bass = knob1*10.0f; // Frequency of LFO for Bass
        f_lfo_Pluck = knob5*10.0f; // Frequency of LFO for Pluck
        f_lfo_Pad = knob3*10.0f; // Frequency of LFO for Pad

        f_Pluck = 440+ knob6*440; // Frequency of Pluck
        f_Bass = 55+ knob2*55; // Frequency of Bass
        f_Pad = 220+ knob4*220; // Frequency of Pad

        //Gate_Pluck = ..
        //Gate_Bass = ..
        //Gate_Pad = ..

        //time =..
        //delayTime =logscale(time, MIN_DELAY, MAX_DELAY); // Imposta il tempo di delay
        

        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
