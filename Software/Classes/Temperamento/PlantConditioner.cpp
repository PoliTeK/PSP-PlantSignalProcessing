#include "PlantConditioner.h"

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
    float out = 0.0f;
    
    // Calcolo Delta
    // Nota: cast a int per gestire numeri negativi prima del float
    _delta = (float)baseline - (float)filtered - (float)_touchThreshold;
    
    // Filtraggio
    _deltaFilt = _deltaFilter.Process(_deltaFilterMF.Process(_delta));

    // Saturazione limiti
    if (_deltaFilt < _deltaMin) {
        // Sotto il minimo: suona la nota più bassa o silenzio? 
        // Dal tuo codice sembra suonare la prima nota scale[0]
        out = _scale[0] * (1 << _octave); 
        _lastNoteIndex = 0;
    }
    else if (_deltaFilt > _deltaMax) {
        // Sopra il massimo: suona l'ultima nota
        out = _scale[_scaleLength - 1] * (1 << _octave);
        _lastNoteIndex = _scaleLength - 1;
    }
    else {
        // Ricerca del Bin corretto
        for (int i = 0; i < _scaleLength; i++) {

            // Calcolo limiti del bin corrente
            float lower = _deltaMin + _range * powf((float)i / _scaleLength, _curveType);
            float upper = _deltaMin + _range * powf((float)(i+1) / _scaleLength, _curveType);
            
            // Calcolo Isteresi dinamica (più grande per note alte)
            float currentHisteresis = _histeresis * powf((float)(i+1) / _scaleLength, _curveType); 

            // --- LOGICA ISTERESI APPLICATA ---
            // Se questa è la nota che stiamo già suonando, allarghiamo i suoi confini
            // per renderla "appiccicosa" e non farla cambiare per piccoli rumori.
            if (i == _lastNoteIndex) {
                lower -= currentHisteresis;
                upper += currentHisteresis;
            }

            // Check se siamo dentro il bin
            if (_deltaFilt >= lower && _deltaFilt < upper) {
                out = _scale[i] * (1 << _octave); // Frequenza base * 2^ottava
                _lastNoteIndex = i; // Memorizza questa nota come attiva
                break;
            }
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