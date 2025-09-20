# include "PlantConditioner.h"

PlantConditioner::PlantConditioner() {}
PlantConditioner::~PlantConditioner() {}

void PlantConditioner::Init(enum ResType res_type) {
    _capFir.Init(res_type);
    _delta = 0;
    _deltaFilt = 0;
    _octave = 4;
}
void PlantConditioner::setScale( enum Notes rootNote, enum ScaleType scale_type) {
    switch (scale_type) 
    {
    case Major:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) {
            _scale[i] = _majorScale[i];
        }
        break;
    case MinorNat:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) {
            _scale[i] = _minorNScale[i];
        }
        break;
    case MinorArm:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) {
            _scale[i] = _minorAScale[i];
        }
        break;
    case MinorMel:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) {
            _scale[i] = _minorMScale[i];
        }
        break;
    case Pentatonic:
        _scaleLength = 5;
        for (int i = 0; i < 5; i++) {
            _scale[i] = _pentatonicScale[i];
        }
        break;
    case Chromatic:
        _scaleLength = 12;
        for (int i = 0; i < 12; i++) {
            _scale[i] = _chromaticScale[i];
        }
        break;
    case Esatonic:
        _scaleLength = 6;
        for (int i = 0; i < 6; i++) {
            _scale[i] = _esatonic[i];
        }
        break;
    default:
        _scaleLength = 7;
        for (int i = 0; i < 7; i++) {
            _scale[i] = _majorScale[i];
        }
        break;
    }

    for (int i = 0; i < _scaleLength; i++) {
        _scale[i] = _scale[i] * powf(2.0f, (float) rootNote / 12.0f); 
    }
}

void PlantConditioner::setOctave(uint8_t octave) {
    _octave = octave;
}

float PlantConditioner::Process(uint16_t baseline, uint16_t filtered) {
    float out = 0.0f;

    _delta = baseline - filtered - touchTreshold;
    _deltaFilt = _capFir.Process((float)_delta);
    if (_deltaFilt < _deltaMin) _deltaFilt = _deltaMin;
    if (_deltaFilt > _deltaMax) _deltaFilt = _deltaMax;
    
    for (int i = 0; i < _scaleLength; i++) {
        if (_deltaFilt < (_deltaMin + (i+1) * (_range / _scaleLength)) && _deltaFilt >= (_deltaMin + i * (_range / _scaleLength))) {
            out = _scale[i] * (1 << _octave);  
        }
    }
    return out;
}