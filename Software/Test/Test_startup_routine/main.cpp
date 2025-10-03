#include "startup.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define ADC_CH 2 // Numero di canali ADC da utilizzare


using namespace daisy;
using namespace daisysp;

daisy::Mpr121I2C::Config mpr121ObjConf; // creates config for mpr121 (constructor sets all to deafult)
daisy::Mpr121I2C cap;                   // creates object for mpr121ù

DaisySeed hw;
AdcChannelConfig adcConfig[ADC_CH];

int main (){
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);

    float maxValue = 0;

    int status = startup_getMaxValue(
        &maxValue,
        &cap,
        hw
    );
    
    while (1){
        hw.PrintLine ("Read value: %f with status %d", maxValue, status);
        hw.DelayMs(1000);
    }
    return 0;
}