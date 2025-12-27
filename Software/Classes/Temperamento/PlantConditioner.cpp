#include "PlantConditioner.h"
#include "../../libs/DaisySP/Source/utility/dsp.h"

using namespace daisysp;

PlantConditioner::PlantConditioner() {}
PlantConditioner::~PlantConditioner() {}

void PlantConditioner::Init(IIR::FilterType filter_type) {
    _deltaFilter.Init(filter_type);
    _deltaFilterMF.Init();
    _maxFir.Init(CapFir::ResType::HIGH);
    _curveFir.Init(CapFir::ResType::HIGH);
    _delta = 0;
    _deltaFilt = 0;
    _octave = 4;
    _lastFreq = 440.0f;
    _lastNoteIndex = -1; // Reset indice nota
}

void PlantConditioner::setScale(enum Notes root_note, enum ScaleType scale_type) {
    // Copia della scala base
    switch (scale_type) 
    {
    case Major:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) _scale[i] = _majorScale[i];
        break;
    case MinorNat:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) _scale[i] = _minorNScale[i];
        break;
    case MinorArm:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) _scale[i] = _minorAScale[i];
        break;
    case MinorMel:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) _scale[i] = _minorMScale[i];
        break;
    case Pentatonic:
        _scaleLength = 5;
        for (int i = 0; i < 5; i++) _scale[i] = _pentatonicScale[i];
        break;
    case Chromatic:
        _scaleLength = 12;
        for (int i = 0; i < 12; i++) _scale[i] = _chromaticScale[i];
        break;
    case Esatonic:
        _scaleLength = 6;
        for (int i = 0; i < 6; i++) _scale[i] = _esatonic[i];
        break;
    default:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) _scale[i] = _majorScale[i];
        break;
    }

    // Trasposizione (Pitch Shifting) in base alla nota fondamentale
    // powf(2, n/12) calcola il rapporto semitonale
    float transposition = powf(2.0f, (float)root_note / 12.0f);
    for (int i = 0; i < _scaleLength; i++) {
        _scale[i] = _scale[i] * transposition; 
    }
}

void PlantConditioner::setOctave(uint8_t octave) {
    _octave = octave;
}

float PlantConditioner::Process(uint16_t baseline, uint16_t filtered) {
    float out = _lastFreq;
    
    // Calcolo Delta
    _delta = (float)baseline - (float)filtered - (float)_touchThreshold;
    
    // Filtraggio e Clamp
    _deltaFilt = _deltaFilter.Process(_deltaFilterMF.Process(_delta));
    _deltaFilt = fclamp(_deltaFilt, _deltaMin, _deltaMax - 0.001f);
    
    // ---------------------------------------------------------
    // FASE 1: STICKY CHECK (Priorità alla nota corrente)
    // Controlliamo subito se possiamo rimanere sulla nota attuale
    // ---------------------------------------------------------
    if (_lastNoteIndex >= 0 && _lastNoteIndex < _scaleLength) {
        int i = _lastNoteIndex;
        
        // Ricalcoliamo i limiti SOLO per la nota corrente
        float pct_lower = (float)i / _scaleLength;
        float pct_upper = (float)(i+1) / _scaleLength;
        
        float lower = _deltaMin + _range * powf(pct_lower, _curveType);
        float upper = _deltaMin + _range * powf(pct_upper, _curveType);
        float hyst  = _histeresis * powf(pct_upper, _curveType); // Isteresi corretta

        // Allarghiamo i confini
        lower -= hyst;
        upper += hyst;

        // Se siamo ancora dentro questi confini allargati, NON cambiare nulla!
        if (_deltaFilt >= lower && _deltaFilt < upper) {
            return _lastFreq; // Ritorna subito, ignora il loop sotto
        }
    }

    // ---------------------------------------------------------
    // FASE 2: RICERCA (Se siamo usciti dalla nota corrente)
    // ---------------------------------------------------------
    for (int i = 0; i < _scaleLength; i++) {
        // Se siamo qui, non siamo nella nota corrente, quindi calcoliamo
        // i bin "puliti" senza isteresi (o con isteresi minima standard)
        
        float lower = _deltaMin + _range * powf((float)i / _scaleLength, _curveType);
        float upper = _deltaMin + _range * powf((float)(i+1) / _scaleLength, _curveType);
        
        if (_deltaFilt >= lower && _deltaFilt < upper) {
            out = _scale[i] * (1 << _octave);
            _lastNoteIndex = i; // Aggiorna la nuova nota corrente
            _lastFreq = out;    // Aggiorna la frequenza
            break;
        }
    }
    
    return out;
}

void PlantConditioner::setCurve(uint8_t delta_max, float curve_type) {
    _curveType = _curveFir.Process(curve_type);
    
    // Assicuriamoci che deltaMax sia sempre maggiore di deltaMin
    float newMax = _maxFir.Process((float)delta_max);
    if (newMax <= _deltaMin) newMax = _deltaMin + 1.0f;
    
    _deltaMax = newMax;
    _range = _deltaMax - _deltaMin;
}

// DEBUG FUNCTIONS
float* PlantConditioner::getBin() {
    for (int i = 0; i < _scaleLength; i++) {
        float lower = _deltaMin + _range * powf((float)i / _scaleLength, _curveType);
        float upper = _deltaMin + _range * powf((float)(i+1) / _scaleLength, _curveType);
        _bin[i] = upper - lower; 
    }
    return _bin;
}

void PlantConditioner::setBuffer() {
    _delta = _deltaFilt;
}