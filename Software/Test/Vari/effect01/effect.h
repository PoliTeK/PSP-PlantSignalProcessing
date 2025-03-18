//
// Created by flipb on 06/03/2025.
//

#ifndef EFFECT_H
#define EFFECT_H
#include <iostream>
#include <stdint.h>


class Effect {
private:
    int* _thresholds;  //
    float _coeff[6]={0.0, 0.2, 0.4, 0.6, 0.8, 1}; //coefficients 

public:
    void EffectBegin(int* thresholdsI);


    uint16_t analogCoeffFine(uint16_t value);
    uint16_t analogCoeffIgnorant(uint16_t value);
};



#endif //EFFECT_H
