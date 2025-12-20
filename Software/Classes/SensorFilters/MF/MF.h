#pragma once
#include "daisysp.h"
#include "daisy_seed.h"
#include <cstdint>

class MF {
public:
    MF();
    ~MF();

    void Init();
    float Process(float in);

private:
    float _sampleRate = 100.0f;

    // IMPORTANTE: Deve essere dispari (3, 5, 7, 9...) per avere una mediana centrale
    static const int _ORDER = 5;   

    // Inizializza tutto il buffer a 0.0f automaticamente
    float _buff[_ORDER] = {0.0f}; 
};