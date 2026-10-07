
//TODO: aggiungere effetti + LFO
//TODO: migliorare shape SAW
//TODO: migliorare range noise


#pragma once
#include "../../libs/PoliTeKDSP/Oscillators/oscillator.h"
#include "daisysp.h"


#define PRESET_NUM 16

enum Direction_e {
    NONE = 0,
    VCA,
    VCF,
    SHAPE,
    DETUNE,
    FX
};

struct Control_s {
    float freq;      // Frequenza in Hz (es. dalla "pianta" o tastiera)
    bool gate;       // Stato del gate (on/off)
};

struct Oscillator_s {
    uint8_t waveform;
    float amp;
    float shape;
    float detune;
    uint8_t octave; 

};

struct Lfo_s {
    uint8_t waveform;
    float amp;
    float freq;
    Direction_e direction;
};

struct Noise_s {
    float amp;
    float color;
};

struct Adsr_s {
    float attack;
    float decay; 
    float sustain;
    float release;
    float amp; // Quantità di inviluppo applicata
};

struct Filter_s {
    float cutoff;
    float resonance;
};
struct Reverb_s {
    float dryWet;
    float lpFreq;
    float feedback;
    //others
};
// Il Preset racchiude lo stato di tutti i moduli
    struct Preset_s {
        uint8_t index;
        char name[16];
        Oscillator_s osc1;
        Oscillator_s osc2;
        Lfo_s lfo1;
        Lfo_s lfo2;
        Noise_s noise;
        Adsr_s amp_env;
        Adsr_s filt_env;
        Filter_s filter;
        Reverb_s reverb;
        bool sync;
        bool ring;

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

    void SetMasterVolume(float volume); 
    // Aggiorna i parametri interni ricevendo il preset attivo
    void SetActivePreset(const Preset_s& preset);

    // Aggiorna controlli real-time (frequenza e gate)
    void UpdateControls(const Control_s& controls);

    // Modifica un parametro del preset tramite MIDI CC e lo applica
    void ProcessMidiCC(uint8_t cc_number, uint8_t cc_value, Preset_s& preset);

    // Genera un singolo sample
    void Process(float& out_l, float& out_r);

    // Formula iper-leggera per lo smoothing
    inline void Smooth(float& current, float target, float coeff = 0.001f) {
        current += coeff * (target - current);
    }

private:
    // Moduli DaisySP
    politekdsp::Oscillator   _osc1;
    politekdsp::Oscillator   _osc2;
    daisysp::Oscillator   _lfo1;
    daisysp::Oscillator   _lfo2;
    daisysp::Dust          _dust;
    daisysp::Adsr         _amp_env;
    daisysp::Adsr         _filt_env;
    daisysp::LadderFilter _filt; 
    daisysp::ReverbSc     _reverb;
    

    // Copia locale dei parametri attuali (per smoothing e lettura nel Process)
    Preset_s _currentPreset;
    Preset_s _smoothedPreset;
    Preset_s _defaultPreset = {
        0, "Default",
        {politekdsp::Oscillator::WAVE_TRI, 1.0f, 0.0f, 0.0f, 2}, 
        {politekdsp::Oscillator::WAVE_TRI, 0.0f, 0.0f, 2},
        {daisysp::Oscillator::WAVE_SIN, 0.0f, 0.0f, Direction_e::NONE}, 
        {daisysp::Oscillator::WAVE_SIN, 0.0f, 0.0f, Direction_e::NONE}, 
        {0.0f, 0.0f},
        {0.01f, 0.1f, 0.8f, 0.1f, 1.0f},       // amp_env
        {0.01f, 0.1f, 0.8f, 0.1f, 0.0f},       // filt_env
        {20000.0f, 0.0f},                       // filter
        {0.0f, 18000.0f, 0.5f},                // reverb
        false, 
        false                            // sync, ring
    };

    // Stato controlli
    bool  _lastGate;
    float _currentFreq;
    float _masterVolume = 1.0f; // Volume master globale (0.0 a 1.0)

    


};