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
        Butterworth,cd
    };

    void Init(FilterType filter_type);
    float Process(float in);

private:
    FilterType _filterType;
    float _sampleRate = 100.0f;  

    // --------------------------------------------------------
    // Butterworth Low-Pass (Order: 2, Fc: 4 Hz @ 1000Hz SR)
    // b = numerator, a = denominator
    // --------------------------------------------------------
    static const int _ButterORDER = 2;
    float _butter_b[_ButterORDER + 1] = {0.00362168f, 0.00724336f, 0.00362168f};
    float _butter_a[_ButterORDER + 1] = {1.00000000f, -1.82269493f, 0.83718165f};
    float _Butter_buff[_ButterORDER] = {0.0f}; 

    // --------------------------------------------------------
    // Bessel Low-Pass (Order: 2, Fc: 5 Hz @ 1000Hz SR)
    // b = numerator, a = denominator
    // --------------------------------------------------------
    static const int _BesselORDER = 2;
    float _bessel_b[_BesselORDER + 1] = {0.00540053f, 0.01080106f, 0.00540053f};
    float _bessel_a[_BesselORDER + 1] = {1.00000000f, -1.74020047f, 0.76180259f};
    float _Bessel_buff[_BesselORDER] = {0.0f}; 

};