#include "Ramo.h"       // Library for the Ramo synth

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\PLUCK
//Parametri Oscillatori---------------------Parametri AmpEG-----------------Parametri FilterEG
uint8_t wf_Pluck[n] = {2,1};                               

float ef_Pluck[n] = {1, 1};                                                  float f_EG_pluck = 2000.0f;                                                                                                  

float g_Pluck[n]  = {0.5f, 0.5f}; 

float f_Pluck = 440.0f;



//Parametri LFO-----------------Parametri Filter------------------Gate
float f_lfo_Pluck = 10;         float ft_pluck = 3000.0f;         bool Gate_Pluck;
float amp_lfo_Pluck = 0.3;      float res_pluck = 0.1f; // Resonance for filter
                                float drive_pluck = 0.5f; // Drive for filter
//-------------------------------------------------------------------------------------ADSR PLUCK
float Pluck_Amp_ADSR[4] = {0.05f, 0.27f, 0.0f, 1.0f}; // Attack, Decay, Sustain, Release
float Pluck_Filter_ADSR[4] = {0.1f, 0.03f, 0.0f, 1.0f}; // Attack, Decay, Sustain, Release

float mix_amp_Pluck = 0.33;



//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\PADS
//Parametri Oscillatori---------------------Parametri AmpEG-----------------Parametri FilterEG
uint8_t wf_Pad[n] = {3,2};                  

float ef_Pad[n] = {1, 1};                                                   float f_EG_pad = 8000.0f;

float g_Pad[n] = {0.5f, 0.5f}; 

float f_Pad = 220.0f; 



//Parametri LFO-----------------Parametri Filter------------------Gate
float f_lfo_Pad   = 10;         float ft_pad = 500.0f;            bool Gate_Pad;
float amp_lfo_Pad = 0.3;        float res_pad = 0.1f; 
                                float drive_pad = 0.5f; 
//-------------------------------------------------------------------------------------ADSR PADS
float Pad_Amp_ADSR[4] = {12.0f, 0.5f, 0.4f, 1.0f}; // Attack, Decay, Sustain, Release
float Pad_Filter_ADSR[4] = {9.0f, 0.4f, 0.0f, 1.0f}; // Attack, Decay, Sustain, Release

float mix_amp_Pad = 0.33;




//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\BASS
//Parametri Oscillatori---------------------Parametri AmpEG-----------------Parametri FilterEG

uint8_t wf_Bass[n] = {1,3};                 

float ef_Bass[n] = {1, 1};                                                  float f_EG_bass = 5000.0f;

float g_Bass[n] = {0.5f, 0.5f};

float f_Bass = 55.0f;



// Parametri LFO-----------------Parametri Filter------------------Gate

float f_lfo_Bass   = 10;         float ft_bass = 200.0f;            bool Gate_Bass;
float amp_lfo_Bass = 0.3;        float res_bass = 0.1f; // Resonance for filter
                                 float drive_bass = 0.5f; // Drive for filter
//-------------------------------------------------------------------------------------ADSR BASS
float Bass_Amp_ADSR[4] = {0.15f, 0.5f, 0.75f, 0.33f}; // Attack, Decay, Sustain, Release
float Bass_Filter_ADSR[4] = {10.0f, 1.0f, 0.6f, 0.5f}; // Attack, Decay, Sustain, Release

float mix_amp_Bass = 0.33;

                                            


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\DELAY
float time;
float delayTime;


const float MIN_DELAY = 0.02f;  // 20ms minimo delay
const float MAX_DELAY = 1.0f;   // 1000ms massimo delay

float LogScale(float pos, float min_value, float max_value) {
    // Implementa una scala logaritmica tra min_value e max_value
    float min_log = logf(min_value);
    float max_log = logf(max_value);
    float exp_scale = min_log + (max_log - min_log) * pos;
    return expf(exp_scale);
}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\REVERB