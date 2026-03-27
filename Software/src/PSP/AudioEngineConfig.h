#include "daisysp.h"

/*
-------------------------------------------------------------------------------------PAD Preset
*/
uint8_t pad_waveform1 = daisysp::Oscillator::WAVE_SAW; 
float pad_amp1 = 1.0f;  float pad_shape1 = 0.5f; float pad_freq1 = 440.0f;

uint8_t pad_waveform2 = daisysp::Oscillator::WAVE_POLYBLEP_SAW; 
float pad_amp2 = 1.0f; float pad_shape2 = 0.5f; float pad_freq2 = 440.0f;

uint8_t lfo_pad_waveform1 =   daisysp::Oscillator::WAVE_SIN;
float lfo_pad_freq1 = 0.2f; float lfo_pad_Amp1 = 0.005f; float lfo_pad_Shape1 = 0.5f;

uint8_t lfo_pad_waveform2 =   daisysp::Oscillator::WAVE_TRI;
float lfo_pad_freq2 = 0.8f; float lfo_pad_Amp2 = 0.005f; float lfo_pad_Shape2 = 0.5f;

float pad_attack = 2.8f; float pad_decay = 1.0f; float pad_release = 1.2f ; float pad_sustain = 0.8f;
float pad_fattack = 2.8f; float pad_fdecay = 1.8f; float pad_frelease = 2.0f; float pad_fsustain = 0.4f;
float pad_cutoff_freq = 1000.0f; float pad_res = 0.4;

/*
-------------------------------------------------------------------------------------PLUCK Preset
*/
uint8_t pluck_waveform1 = daisysp::Oscillator::WAVE_TRI; 
float pluck_amp1 = 1.0f;  float pluck_shape1 = 0.5f; float pluck_freq1 = 440.0f;

uint8_t pluck_waveform2 = daisysp::Oscillator::WAVE_TRI; 
float pluck_amp2 = 1.0f; float pluck_shape2 = 0.5f; float pluck_freq2 = 440.0f;

uint8_t lfo_pluck_waveform1 =   daisysp::Oscillator::WAVE_TRI;
float lfo_pluck_freq1 = 7.0f; float lfo_pluck_Amp1 = 0.05f; float lfo_pluck_Shape1 = 0.5f;

uint8_t lfo_pluck_waveform2 =   daisysp::Oscillator::WAVE_TRI;
float lfo_pluck_freq2 = 0.7f; float lfo_pluck_Amp2 = 0.005f; float lfo_pluck_Shape2 = 0.5f;

float pluck_attack = 0.01f; float pluck_decay = 0.15f; float pluck_release = 0.8f; float pluck_sustain = 0.0f;

float pluck_fattack = 0.1f; float pluck_fdecay = 1.8f; float pluck_frelease = 2.0f; float pluck_fsustain = 0.4f;

float pluck_cutoff_freq = 8000.0f; float pluck_res = 0.4;

/*
-------------------------------------------------------------------------------------LEAD Preset
*/
uint8_t lead_waveform1 = daisysp::Oscillator::WAVE_POLYBLEP_SQUARE;
float lead_amp1 = 1.0f;  float lead_shape1 = 0.1f; float lead_freq1 = 440.0f;

uint8_t lead_waveform2 = daisysp::Oscillator::WAVE_POLYBLEP_SQUARE; 
float lead_amp2 = 1.0f; float lead_shape2 = 0.1f; float lead_freq2 = 440.0f;

uint8_t lfo_lead_waveform1 =   daisysp::Oscillator::WAVE_TRI;
float lfo_lead_freq1 = 0.7f; float lfo_lead_Amp1 = 0.003f; float lfo_lead_Shape1 = 0.5f;

uint8_t lfo_lead_waveform2 =   daisysp::Oscillator::WAVE_TRI;
float lfo_lead_freq2 = 0.7f; float lfo_lead_Amp2 = 0.005f; float lfo_lead_Shape2 = 0.5f;

float lead_attack = 0.05f; float lead_decay = 2.8f; float lead_release = 1.3f; float lead_sustain = 0.6f;

float lead_fattack = 1.0f; float lead_fdecay = 0.8f; float lead_frelease = 0.8f; float lead_fsustain = 0.6f; float lead_fAmp = 2;

float lead_cutoff_freq = 1000.0f; float lead_res = 0.4f;




float revLpFreq = 18000.0f;
float revFeedback = 0.85f;
float revDryWet = 0.3f; 