# include "PlantConditioner.h"

PlantConditioner::PlantConditioner() {}
PlantConditioner::~PlantConditioner() {}

void PlantConditioner::Init(CapFir::ResType res_type) {
    _deltaFir.Init(res_type);
    _maxFir.Init(CapFir::ResType::HIGH);
    _curveFir.Init(CapFir::ResType::HIGH);
    _delta = 0;
    _deltaFilt = 0;
    _octave = 4;
}
void PlantConditioner::setScale( enum Notes root_note, enum ScaleType scale_type) {
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
        _scale[i] = _scale[i] * powf(2.0f, (float) root_note / 12.0f); 
    }
}

void PlantConditioner::setOctave(uint8_t octave) {
    _octave = octave;
}

float PlantConditioner::Process(uint16_t baseline, uint16_t filtered) {
    float out = 0.0f;
    _delta = baseline - filtered - touchTreshold;
    _deltaFilt = _deltaFir.Process(_delta);
    if (_deltaFilt < _deltaMin) out = _scale[0] * (1 << _octave);
    else if (_deltaFilt > _deltaMax) out = _scale[_scaleLength - 1] * (1 << _octave);
    else {
        for (int i = 0; i < _scaleLength; i++) {

        //Dimensione Crescente Bin
        float lower = _deltaMin + _range * powf((float)i / _scaleLength, _curveType);
        float upper = _deltaMin + _range * powf((float)(i+1) / _scaleLength, _curveType);
        float histeresis = _histeresis * powf((float)(i+1) / _scaleLength, _curveType); //histeresis proporzionale alla dimensione del bin

        //Dimensione Decrescente Bin
        //float lower = _deltaMin + _range * (1.0f - powf(1.0f - (float)i / _scaleLength, _curveType));
        //float upper = _deltaMin + _range * (1.0f - powf(1.0f - (float)(i + 1) / _scaleLength, _curveType));
            
            if (_deltaFilt >= lower && _deltaFilt < upper) {
                out = _scale[i] * (1 << _octave);
                break;
            }
        }
    }
    
    
    return out;
}

float* PlantConditioner::getBin() {
    for (int i = 0; i < _scaleLength; i++) {
        
        //Dimensione Crescente Bin
        float lower = _deltaMin + _range * powf((float)i / _scaleLength, _curveType);
        float upper = _deltaMin + _range * powf((float)(i+1) / _scaleLength, _curveType);

        //Dimensione Decrescente Bin
        //float lower = _deltaMin + _range * (1.0f - powf(1.0f - (float)i / _scaleLength, _curveType));
        //float upper = _deltaMin + _range * (1.0f - powf(1.0f - (float)(i + 1) / _scaleLength, _curveType));
            
        _bin[i] = upper - lower;  // x debug
    }
    return _bin;
}

void PlantConditioner::setCurve(uint8_t delta_max, float curve_type) {

    _curveType = _curveFir.Process(curve_type);
    _deltaMax = _maxFir.Process(delta_max);
    _range = _deltaMax - _deltaMin;
}

void PlantConditioner::setBuffer() {
    _delta = _deltaFilt ;
    _deltaFir.setBuffer(_delta);
}