//
// Created by flipb on 06/03/2025.
//

#ifndef PENTAPIANTA_H
#define PENTAPIANTA_H
#include <stdint.h>


class pentaPianta {
public:
    pentaPianta();
    pentaPianta(int* soglieI,int f0I);

    int begin(int* soglieI, int f0I);

    uint16_t discFreq(uint16_t value);

    uint16_t analogFreq(uint16_t value);
    void incCount() { _count++; } //increment the count of the number of times the sensor has been read
    void zeroCount() { _count = 0; } //reset the count of the number of times the sensor has been read
    void setRawValue(uint16_t value) { _raw_value = value; } //set the raw value of the sensor
    int getFreq();
    int getCount() { return _count; } //get the count of the number of times the sensor has been read
    int getRawValue() { return _raw_value; } //get the raw value of the sensor
    int getGate() { return _count > 0; } //get the gate of the sensor
    private:
    int* _soglie;
    int _f0;
    int _count;
    uint16_t _raw_value;
};



#endif //PENTAPIANTA_H
