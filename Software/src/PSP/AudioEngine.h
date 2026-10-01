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
    float Shape;
    // Freq è omesso qui se è controllato globalmente da Control_s (pitch), 
    // a meno che non serva come detune o offset.
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

    // Aggiungi l'operatore != necessario per PersistentStorage in libDaisy
    bool operator!=(const Preset_s& other) const {
        return index != other.index; // Semplificato, in produzione confronta i parametri chiave
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

    // Genera un singolo sample
    float Process();

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

    // Stato controlli
    bool  _lastGate;
    float _currentFreq;
};