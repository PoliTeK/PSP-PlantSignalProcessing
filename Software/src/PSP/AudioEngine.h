#pragma once
#include "daisysp.h"

#define PRESET_NUM 16

// Strutture dei parametri
struct Control_s {
    float freq;      // Frequenza in Hz (es. dalla "pianta" o tastiera)
    bool gate;       // Stato del gate (on/off)
};

struct Oscillator_s {
    uint8_t Waveform;
    float Amp;
    //float Shape;
    //float detune;

};

struct Adsr_s {
    float Attack;
    float Decay; 
    float Sustain;
    float Release;
    float Amp; // Quantità di inviluppo applicata
};

struct Filter_s {
    float Cutoff;
    float Resonance;
};
struct Reverb_s {
    float DryWet;
    float revLpFreq;
    float revFeedback;
    //others
};
// Il Preset racchiude lo stato di tutti i moduli
    struct Preset_s {
        uint8_t index;
        char name[16];
        Oscillator_s osc1;
        Oscillator_s osc2;
        Oscillator_s lfo1;
        Oscillator_s lfo2;
        Adsr_s amp_env;
        Adsr_s filt_env;
        Filter_s filter;
        Reverb_s reverb;

        bool operator!=(const Preset_s& other) const {
            // Confronta l'intera struttura in memoria per rilevare qualsiasi modifica ai parametri
            return memcmp(this, &other, sizeof(Preset_s)) != 0;
        }
    };

class AudioEngine {
public:
    AudioEngine() {}
    ~AudioEngine() {}

    void Init(float sample_rate);

    // Aggiorna i parametri interni ricevendo il preset attivo
    void SetActivePreset(const Preset_s& preset);

    // Aggiorna controlli real-time (frequenza e gate)
    void UpdateControls(const Control_s& controls);

    // Modifica un parametro del preset tramite MIDI CC e lo applica
    void ProcessMidiCC(uint8_t cc_number, uint8_t cc_value, Preset_s& preset);

    // Genera un singolo sample
    void Process(float& out_l, float& out_r);

private:
    // Moduli DaisySP
    daisysp::Oscillator   _osc1;
    daisysp::Oscillator   _osc2;
    daisysp::Oscillator   _lfo1;
    daisysp::Oscillator   _lfo2;
    daisysp::Adsr         _amp_env;
    daisysp::Adsr         _filt_env;
    daisysp::LadderFilter _filt; 
    daisysp::ReverbSc     _reverb;

    // Copia locale dei parametri attuali (per smoothing e lettura nel Process)
    Preset_s _currentPreset;
    Preset_s _defaultPreset = {
        0, "Default",
        {daisysp::Oscillator::WAVE_SAW, 1.0f}, // osc1
        {daisysp::Oscillator::WAVE_SAW, 1.0f}, // osc2
        {daisysp::Oscillator::WAVE_SIN, 1.0f}, // lfo1
        {daisysp::Oscillator::WAVE_SIN, 1.0f}, // lfo2
        {0.01f, 0.1f, 0.8f, 0.5f, 1.0f},       // amp_env
        {0.01f, 0.1f, 0.8f, 0.5f, 1.0f},       // filt_env
        {1000.0f, 0.5f},                       // filter
        {0.3f, 18000.0f, 0.85f}                // reverb
    };

    // Stato controlli
    bool  _lastGate;
    float _currentFreq;


};