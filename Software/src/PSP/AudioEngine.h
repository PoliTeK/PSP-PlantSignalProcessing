#pragma once
#include "daisysp.h"
#include "AudioEngineConfig.h"


// Definizione dei preset disponibili
enum SynthPreset {
    PRESET_PAD = 0,
    PRESET_PLUCK,
    PRESET_LEAD,     //BoC Roygbiv like lead sound
    NUM_PRESETS
};

struct ControlsStruct {
    float freq; // Frequenza in Hz
    bool gate;       // Stato del gate (on/off)
};

struct Oscillator_str{
    uint8_t Waveform;
    float Amp;
    float Shape;
    float Freq;
};

struct Adsr_str{
    float Attack;
    float Decay; 
    float Sustain;
    float Release;
};
struct Filter_str{
    float Cutoff;
    float Resonance;
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    // Inizializza i moduli DSP con il sample rate della scheda
    void Init(float sample_rate);

    // Cambia i parametri interni in base al preset scelto
    void SetPreset(SynthPreset preset);

    // Riceve lo struct dalla pianta e aggiorna frequenza e gate (Control Rate)
    void Update(ControlsStruct Controls);

    // Genera un singolo sample audio (Audio Rate)
    float Process();

private:
    // Moduli DaisySP
    daisysp::Oscillator   _osc1;
    daisysp::Oscillator   _osc2;
    daisysp::Oscillator   _lfo1;
    daisysp::Oscillator   _lfo2;
    daisysp::Adsr         _Aenv;
    daisysp::Adsr         _Fenv;
    daisysp::LadderFilter _filter; // State Variable Filter

    Oscillator_str _osc1Param;
    Oscillator_str _osc2Param;
    Oscillator_str _lfo1Param;
    Oscillator_str _lfo2Param;

    Adsr_str _AmpADSR;
    Adsr_str _FiltADSR;

    Filter_str _Filt;

    // Variabili di stato interne
    SynthPreset _currentPreset;
    bool        _lastGate;
    float       _currentFreq;
};