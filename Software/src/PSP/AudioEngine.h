#pragma once
#include "daisysp.h"


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
    daisysp::Oscillator _osc1;
    daisysp::Oscillator _osc2;
    daisysp::Oscillator _lfo;
    daisysp::Adsr       _Aenv;
    daisysp::Adsr       _Fenv;
    daisysp::Svf        _filter; // State Variable Filter

    // Variabili di stato interne
    SynthPreset _currentPreset;
    bool        _lastGate;
    float       _currentFreq;
};