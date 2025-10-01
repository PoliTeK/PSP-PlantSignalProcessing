#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "CapFir.h"

CapFir::CapFir() {}
CapFir::~CapFir() {}

void CapFir::Init(enum ResType res_type) {
    _resType = res_type;
    switch (_resType) {
        case LOW:
            for (int i = 0; i < 16; i++) {
                _bufferL[i] = 0.0f;
                }
            break;

        case MID:
            for (int i = 0; i < 32; i++) {
                _bufferM[i] = 0.0f;
            }
            break;

        case HIGH:
            for (int i = 0; i < 64; i++) {
                _bufferH[i] = 0.0f;
            }
            break;
    }      
    
}

float CapFir::Process(float in) {
    float out = 0.0f;

    switch (_resType) {
        case LOW:
            for (int i = 15; i > 0; i--) {
                _bufferL[i] = _bufferL[i - 1];
            }
            _bufferL[0] = in;
            for (int i = 0; i < 16; i++) {
                out += _bufferL[i] * _coeffL[i];
            }
            break;

        case MID:
            for (int i = 31; i > 0; i--) {
                _bufferM[i] = _bufferM[i - 1];
            }
            _bufferM[0] = in;
            for (int i = 0; i < 32; i++) {
                out += _bufferM[i] * _coeffM[i];
            }
            break;

        case HIGH:
            for (int i = 63; i > 0; i--) {
                _bufferH[i] = _bufferH[i - 1];
            }
            _bufferH[0] = in;
            for (int i = 0; i < 64; i++) {
                out += _bufferH[i] * _coeffH[i];
            }
            break;
    }

    return out;
}

void CapFir::setBuffer(float value) {
    switch (_resType) {
        case LOW:
            for (int i = 0; i < 16; i++) {
                _bufferL[i] = value;
                }
            break;

        case MID:
            for (int i = 0; i < 32; i++) {
                _bufferM[i] = value;
            }
            break;

        case HIGH:
            for (int i = 0; i < 64; i++) {
                _bufferH[i] = value;
            }
            break;
    }      
    
}


