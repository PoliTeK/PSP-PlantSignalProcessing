#include "PlantConditioner.h"


using namespace daisysp;

PlantConditioner::PlantConditioner() {}
PlantConditioner::~PlantConditioner() {}

void PlantConditioner::Init(IIR::FilterType filter_type, daisy::DaisySeed* hw) {
    
    _hw = hw;

    _mpr121ObjConf.transport_config.speed = daisy::I2CHandle::Config::Speed::I2C_400KHZ;
    if (_cap.Init(_mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
    {
        while (1) {
            _hw->SetLed(true); _hw->DelayMs(100);
            _hw->SetLed(false); _hw->DelayMs(100);
        }
    }
    _hw->SetLed(true);



    _deltaFilter.Init(filter_type);
    _deltaFilterMF.Init();
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
    updateThresholds();
}

void PlantConditioner::setOctave(uint8_t octave) {
    _octave = octave;
}

PlantConditioner::PlantState PlantConditioner::Process() {
    float out = _lastFreq;
    if (_cap.Touched() & _BV(0))
    {
        if (!_isTouched) _isTouched = true; // New touch detected
        
        // Computation of filtered and clamped delta
        _delta = _cap.BaselineData(0) - _cap.FilteredData(0) - (float)_touchThreshold;
        _deltaFilt = _deltaFilter.Process(_deltaFilterMF.Process(_delta));
        _deltaFilt = fclamp(_deltaFilt, _deltaMin, _deltaMax - 0.001f);
        
        // ---------------------------------------------------------
        // STICKY CHECK (Current Note has the priority)
        // Controlliamo subito se possiamo rimanere sulla nota attuale
        // ---------------------------------------------------------
        if (_lastNoteIndex >= 0 && _lastNoteIndex < _scaleLength) {
            int i = _lastNoteIndex;
            
            float lower = _noteThresholds[i];
            float upper = _noteThresholds[i+1];
            
            // L'isteresi può essere una percentuale fissa del range totale
            float hyst = _histeresis * (_range / 100.0f); 

            if (_deltaFilt >= (lower - hyst) && _deltaFilt < (upper + hyst)) {
                return {out, _isTouched}; 
            }
        }

        // ---------------------------------------------------------
        // NEW NOTE CHECK
        // ---------------------------------------------------------
        for (int i = 0; i < _scaleLength; i++) {
            float lower = _noteThresholds[i];
            float upper = _noteThresholds[i+1];
            
            // Controllo standard + sicurezza per l'ultimo bin
            if ((_deltaFilt >= lower && _deltaFilt < upper) || (i == _scaleLength - 1 && _deltaFilt >= upper)) {
                out = _scale[i] * (1 << _octave);
                _lastNoteIndex = i; 
                _lastFreq = out;    
                break;
            }
        }
    } else {
        if (_isTouched) _isTouched = false; 
    }

    return {out, _isTouched};
}

void PlantConditioner::setCurve(float curve_type) {
    _curveType = curve_type;
    updateThresholds();
}

void PlantConditioner::setDelta(float delta_max) {
    if (delta_max <= _deltaMin) delta_max = _deltaMin + 1.0f;
    _deltaMax = delta_max;
    _range = _deltaMax - _deltaMin;
    updateThresholds();
}

void PlantConditioner::updateThresholds() {
    for (int i = 0; i <= _scaleLength; i++) {
        float pct = (float)i / _scaleLength;
        _noteThresholds[i] = _deltaMin + _range * powf(pct, _curveType);
    }
}

void PlantConditioner::setHisteresis(float histeresis) {
    _histeresis = histeresis; updateThresholds(); 
}
