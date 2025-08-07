#pragma once 

#include "daisy_seed.h"    // Include le funzionalità base della scheda Daisy
#include "daisysp.h"

class Utilities
{

public:
    Utilities();
    ~Utilities();

    enum
    {
        LOW_RES,
        MID_RES,
        HIGH_RES,
        LAST_RES,
    };
    
    void init(float sample_rate);
    float SmoothAnalogRead(unsigned int ADC_Channel, unsigned int res_type, daisy::DaisySeed& hw);

private:
    /* data */
    float _sr, _sr_recip; // sample rate ed il suo reciproco
    unsigned int _res_type; // tipo di risoluzione per la lettura
    

};


