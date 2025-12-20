#pragma once
#include "daisysp.h"
#include "daisy_seed.h"
#include <cstdint>

class IIR {
public:
    IIR();
    ~IIR();

    enum FilterType {
        Bessel = 0,
        Butterworth,
    };

    void Init(FilterType filter_type);
    float Process(float in);

private:
    FilterType _filterType;
    float _sampleRate = 100.0f;  

    // --------------------------------------------------------
    // Butterworth Low-Pass (Order: 2, Fc: 4 Hz @ 100Hz SR)
    // b = numerator, a = denominator
    // --------------------------------------------------------
    static const int _ButterORDER = 2;
    float _butter_b[_ButterORDER + 1] = {0.01335920f, 0.02671840f, 0.01335920f};
    float _butter_a[_ButterORDER + 1] = {1.00000000f, -1.64745998f, 0.70089678f};
    float _Butter_buff[_ButterORDER] = {0.0f}; 

    // --------------------------------------------------------
    // Bessel Low-Pass (Order: 2, Fc: 5 Hz @ 100Hz SR)
    // b = numerator, a = denominator
    // --------------------------------------------------------
    static const int _BesselORDER = 2;
    float _bessel_b[_BesselORDER + 1] = {0.01902767f, 0.03805533f, 0.01902767f};
    float _bessel_a[_BesselORDER + 1] = {1.00000000f, -1.50426924f, 0.58037990f};
    float _Bessel_buff[_BesselORDER] = {0.0f}; 

};