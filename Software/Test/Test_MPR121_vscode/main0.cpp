#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/FIIR/CapFir.h"
#include "../../Classes/Temperamento/PlantConditioner.h"
//#define DebugFrequencies
#define DebugBins
#ifndef _BV // used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit))
#endif
#define ADC_CH 2 // Numero di canali ADC da utilizzare


using namespace daisy;
using namespace daisysp;
//--OLD ARDUINO: Adafruit_MPR121 cap = Adafruit_MPR121();

daisy::Mpr121I2C::Config mpr121ObjConf;                       // creates config for mpr121 (constructor sets all to deafult)

daisy::Mpr121I2C cap;                                         // creates object for mpr121ù

DaisySeed hw;
AdcChannelConfig adcConfig[ADC_CH]; // Array di configurazione per 3 canali ADC

int t;
CapFir filter;                                                   // creates object for the CapFir filter
PlantConditioner pc;

uint16_t lasttouched = 0;                                     // used to store the last touched value
uint16_t currtouched = 0;                                     // used to store the current touched value

bool touched = false;                                          // used to check if the cap was touched or not

int main()
{

  
  hw.Configure();
  hw.Init();
  

  hw.StartLog(true);                                                            // starts the log to the serial port

  if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)                  // initializes the mpr121 with the config and see if the comunication is ok
  {
    hw.PrintLine("MPR121 not found, check wiring?");
    while (1)
    {
      hw.SetLed(true);                                                          // blink if not ok
      hw.DelayMs(500);
      hw.SetLed(false);
      hw.DelayMs(500);
    }
  }
  hw.PrintLine("MPR121 found!");

  pc.Init(CapFir::ResType::LOW);                     // initializes the filter with the low pass type and 16 coefficients
  pc.setScale(PlantConditioner::C, PlantConditioner::Major);
  pc.setOctave(3);
  pc.setCurve(100, 1.1);                               // sets the curve of the scale

  adcConfig[0].InitSingle(hw.GetPin(15)); 
  adcConfig[1].InitSingle(hw.GetPin(16)); 
  hw.adc.Init(adcConfig, ADC_CH);              
  hw.adc.Start();                         

  //cap.SetThresholds(12, 6); // sets the touch and release thresholds for all 12 channels         // non funziona PD

  while (1)
  {
    #ifdef DebugFrequencies
    currtouched = cap.Touched();                                            // reads the touched channels from the mpr121
    if ((currtouched & _BV(0)) && !(lasttouched & _BV(0)))                 // if the channel 0 is touched and it was not touched before
    {
      hw.PrintLine("--------------------------------------------------------------------------------");
      hw.PrintLine("CAP 0 touched"); 
      touched = true;  
    }

    //if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(0)) && (lasttouched & _BV(0)))                // if the channel 0 was touched and it is not touched now
    {
      hw.PrintLine("CAP 0 released");
      hw.PrintLine("--------------------------------------------------------------------------------");
      hw.PrintLine(" ");
      touched = false;
       
    }

    if (touched)                                                     
    {
      hw.PrintLine("| ");
      //hw.PrintLine("| Baseline Touched Value : %d |", cap.BaselineData(0));
      //hw.PrintLine("| Filtered Touched Value : %d |", cap.FilteredData(0));
      hw.PrintLine("| Difference Touched Value : %d |", cap.BaselineData(0) - cap.FilteredData(0));
      hw.PrintLine("| Filtered Difference Touched Value : %f |", pc.Process(cap.BaselineData(0), cap.FilteredData(0)));
      hw.PrintLine("| ");
    } else {
      hw.PrintLine(" ");
      hw.PrintLine(" ");
      //hw.PrintLine("Baseline Untouched Value : %d", cap.BaselineData(0));
      //hw.PrintLine("Filtered Untouched Value : %d", cap.FilteredData(0));
      hw.PrintLine("Difference Untouched Value : %d", cap.BaselineData(0)-cap.FilteredData(0));
      hw.PrintLine("Filtered Difference Untouched Value : %f", pc.Process(cap.BaselineData(0), cap.FilteredData(0)));
      hw.PrintLine(" ");
      hw.PrintLine(" ");
    }
    lasttouched = currtouched;
    #endif

    
    #ifdef DebugBins
    //uint8_t delta_max =pc.getDeltaMin() + (uint8_t) hw.adc.Get(0)*120/4095; //0-120
    float curve_type = hw.adc.GetFloat(1)*4.0f;
    pc.setCurve(100, filter.Process(curve_type));
    float* bins = pc.getBin();
    hw.PrintLine("RawValue : %f", curve_type);
    
    hw.PrintLine("Bins: ");
    for (int i = 0; i < 7; i++) {
        hw.Print("| %f | ", bins[i]);
    }
    hw.PrintLine(" ");
    #endif
   
    hw.DelayMs(100);
    
  }
}
