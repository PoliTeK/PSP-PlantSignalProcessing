//
// Created by flipb on 06/03/2025.
//

#ifndef EFFECT_H
#define EFFECT_H
//#include <iostream>
#include <stdint.h>


class Effect {
public:
    void begin(int* thresholdsI);
    void setRawValue(uint16_t value) { _raw_value = value; } //set the raw value of the sensor
    void setGate(bool gate) { _gate = gate; } //set the gate of the sensor
    float analogCoeffFine(uint16_t value);
    float analogCoeffIgnorant(uint16_t value);
    float getValue() { return analogCoeffFine(_raw_value); } //get the raw value of the sensor
    bool getGate() { return _gate; } //get the gate of the sensor
    uint16_t getRaw() {return _raw_value;}

private:
    int* _thresholds;  //
    float _coeff[6]={0.0, 0.2, 0.4, 0.6, 0.8, 1}; //coefficients 
    bool _gate;
    uint16_t _raw_value; //raw value of the sensor
};



#endif //EFFECT_H
