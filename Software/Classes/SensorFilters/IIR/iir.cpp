#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "iir.h"


IIR::IIR() {}
IIR::~IIR() {}

void IIR::Init(FilterType filter_type) {
    _filterType = filter_type;

    
    for (int i = 0; i < _BesselORDER; i++) {
        _Bessel_buff[i] = 0.0f;
    }

    for (int i = 0; i < _ButterORDER; i++) {
        _Butter_buff[i] = 0.0f;
    }
}

float IIR::Process(float in) {
    float out = 0.0f;
    float wn;                         // Nodo intermedio (Direct Form II)


    switch (_filterType) {
        case Butterworth:
            wn = in - (_butter_a[1] * _Butter_buff[0] + _butter_a[2] * _Butter_buff[1]);
            out = _butter_b[0] * wn + _butter_b[1] * _Butter_buff[0] + _butter_b[2] * _Butter_buff[1];
            _Butter_buff[1] = _Butter_buff[0];
            _Butter_buff[0] = wn;
            break;

        case Bessel:
            wn = in - (_bessel_a[1] * _Bessel_buff[0] + _bessel_a[2] * _Bessel_buff[1]);
            out = _bessel_b[0] * wn + _bessel_b[1] * _Bessel_buff[0] + _bessel_b[2] * _Bessel_buff[1];
            _Bessel_buff[1] = _Bessel_buff[0];
            _Bessel_buff[0] = wn;
            break;
    }
    return out;
}