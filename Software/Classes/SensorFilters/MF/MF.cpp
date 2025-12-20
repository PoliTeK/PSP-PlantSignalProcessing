#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "MF.h"

MF::MF() {}
MF::~MF() {}

void MF::Init() {
    for (int i = 0; i < _ORDER ; i++) {
        _buff[i] = 0.0f;
    }
}

float MF::Process(float in) {
    for (int i = _ORDER - 1; i > 0; i--) {
        _buff[i] = _buff[i - 1];
    }
    _buff[0] = in; 
    
    float tempBuffer[_ORDER];
    
   for (int i = 0; i < _ORDER ; i++) {
        tempBuffer[i] = _buff[i];
    }
    
    for (int i = 1; i < _ORDER; i++) {
        float key = tempBuffer[i];
        int j = i - 1;
        while (j >= 0 && tempBuffer[j] > key) {
            tempBuffer[j + 1] = tempBuffer[j];
            j = j - 1;
        }
        tempBuffer[j + 1] = key;
    }

    return tempBuffer[_ORDER / 2];
}