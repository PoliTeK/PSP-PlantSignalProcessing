#pragma once
#include <cstdint>
#include <cmath>
#include "daisysp.h"
#include "daisy_seed.h"

// Assicurati che questi percorsi siano corretti nel tuo progetto!
// Se i file sono nella stessa cartella, usa semplicemente #include "CapFir.h" ecc.
#include "../SensorFilters/FIIR/CapFir.h"
#include "../SensorFilters/IIR/iir.h"
#include "../SensorFilters/MF/MF.h"

// Fallback se la macro non è definita altrove
#ifndef MPR121_TOUCH_THRESHOLD_DEFAULT
#define MPR121_TOUCH_THRESHOLD_DEFAULT 12
#endif

class PlantConditioner {
public:
    PlantConditioner();
    ~PlantConditioner();

    enum ScaleType {
        Major = 0,
        MinorNat,
        MinorArm,
        MinorMel,
        Pentatonic,
        Chromatic,
        Esatonic,
    };

    enum Notes {
        C = 0,
        Cs,
        D,
        Ds,
        E,
        F,
        Fs,
        G,
        Gs,
        A,
        As,
        B,
    };

    void Init(IIR::FilterType filter_type);

    void setCurve(uint8_t delta_max, float curve_type);

    void setScale(enum Notes root_note, enum ScaleType scale_type);

    void setOctave(uint8_t octave);

    float Process(uint16_t baseline, uint16_t filtered);

    // DEBUG FUNCTIONS
    float* getBin();
    float getDelta() { return _delta; }
    float getDeltaFilt() { return _deltaFilt; }
    void setBuffer();

    // Getter aggiornati a float per coerenza
    float getDeltaMin() { return _deltaMin; }
    float getDeltaMax() { return _deltaMax; }
    float getRange() { return _range; }

private:
    // Parametri Hardware
    const uint8_t _touchThreshold = MPR121_TOUCH_THRESHOLD_DEFAULT;

    // Filtri
    IIR _deltaFilter;
    MF  _deltaFilterMF;
    CapFir _maxFir;
    CapFir _curveFir;

    // Parametri Logici
    const float _histeresis = 2.0f; // Isteresi (zona morta tra le note)
    int _lastNoteIndex = -1;        // IMPORTANTE: Serve per ricordare l'ultima nota e applicare l'isteresi
    
    float _curveType = 0; // 0 linear, >1 exponential, <1 logarithmic
    uint8_t _octave = 4;
    uint8_t _scaleLength = 7;

    // Cambiati in float per evitare problemi di calcolo/overflow
    float _deltaMin = 1.0f;
    float _deltaMax = 100.0f;
    float _range = 99.0f;

    float _delta = 0.0f;
    float _deltaFilt = 0.0f;
    
    // Array dati
    float _bin[12];
    float _scale[12]; 

    // Scale di Riferimento (Frequenze base in Hz - Ottava 0)
    const float _majorScale[7]      = {16.35f, 18.35f, 20.60f, 21.83f, 24.50f, 27.50f, 30.87f}; // C D E F G A B
    const float _minorNScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 24.50f, 25.96f, 29.14f}; // C D Eb F G Ab Bb
    const float _minorAScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 24.50f, 25.96f, 30.87f}; // C D Eb F G Ab B
    const float _minorMScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 23.12f, 27.50f, 30.87f}; // C D Eb F G A B (Melodica Asc)
    const float _pentatonicScale[5] = {16.35f, 18.35f, 21.83f, 24.50f, 27.50f};                 // C D F G A
    const float _chromaticScale[12] = {16.35f, 17.32f, 18.35f, 19.45f, 20.60f, 21.83f, 23.12f, 24.50f, 25.96f, 27.50f, 29.14f, 30.87f}; 
    const float _esatonic[6]        = {16.35f, 18.35f, 20.60f, 21.83f, 24.50f, 27.50f};         // C D E F G A (Esatonale custom)
};