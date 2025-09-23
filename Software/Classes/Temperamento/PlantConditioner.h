#pragma once
#include "daisysp.h"
#include "daisy_seed.h"
#include "../FIIR/CapFir.h"
#include <cstdint>



class PlantConditioner {
public:
    PlantConditioner();
    ~PlantConditioner();

    enum ScaleType {
        Major = 0,
        MinorNat,
        MinorArm,
        MinorMel,
        Pentatonic,
        Chromatic,
        Esatonic,
    };

    enum Notes {
        C = 0,
        Cs,
        D,
        Ds,
        E,
        F,
        Fs,
        G,
        Gs,
        A,
        As,
        B,
    };
        


    void Init(CapFir::ResType res_type);

    void setCurve(uint8_t delta_max, float curve_type);

    void setScale( enum Notes root_note, enum ScaleType scale_type);

    void setOctave(uint8_t octave);

    float* getBin();

    float Process(uint16_t baseline, uint16_t filtered);

    uint8_t getDeltaMin(){ return _deltaMin; }

    
    uint8_t getDeltaMax(){ return _deltaMax; } 
    
    uint8_t getRange(){ return _range; } 

private:

    const uint8_t touchTreshold = MPR121_TOUCH_THRESHOLD_DEFAULT;
    CapFir _capFir;

    float _curveType = 0; //0 linear, 1 exponential, 2 logarithmic
    uint8_t _octave, _scaleLength = 7;
    uint8_t _delta,  _deltaMin = 1, _deltaMax = 101, _range = 100;
    float  _deltaFilt;
    float _bin[12];
    float _outFreq;
    float _scale[12]; //Al massimo scala dodecafonica equalizzata. Se vogliamo farla microtonale, fottetevi ecco......   

    const float _majorScale[7] = {16.35 , 18.35 , 20.6, 21.83, 24.5, 27.5, 30.87   }; //C D E F G A B
    const float _minorNScale[7] = {16.35 , 18.35 , 19.45, 21.83, 24.5, 25.96, 29.14   }; //C D Eb F G Ab Bb
    const float _minorAScale[7] = {16.35 , 18.35 , 19.45, 21.83, 24.5, 25.96, 30.87   }; //C D Eb F G Ab B
    const float _minorMScale[7] = {16.35 , 18.35 , 19.45, 21.83, 23.12, 27.5, 30.87   }; //C D D# F G A B
    const float _pentatonicScale[5] = {16.35 , 18.35 , 21.83, 24.5, 27.5   }; //C D F G A
    const float _chromaticScale[12] = {16.35 , 17.32 , 18.35, 19.45, 20.6, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87   }; //C C# D D# E F F# G G# A A# B
    const float _esatonic[6] = {16.35 , 18.35 , 20.6, 21.83, 24.5, 27.5   }; //C D E F G A
};