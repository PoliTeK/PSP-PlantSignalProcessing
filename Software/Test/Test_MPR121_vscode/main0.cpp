#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

#ifndef _BV // used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit))
#endif

using namespace daisy;
using namespace daisysp;
//--OLD ARDUINO: Adafruit_MPR121 cap = Adafruit_MPR121();

daisy::Mpr121I2C::Config mpr121ObjConf;                       // creates config for mpr121 (constructor sets all to deafult)

daisy::Mpr121I2C cap;                                         // creates object for mpr121ù

DaisySeed hw;

uint16_t lasttouched = 0;                                     // used to store the last touched value
uint16_t currtouched = 0;                                     // used to store the current touched value

bool touched = false;                                          // used to check if the cap was touched or not

int main()
{

  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  sample_rate = hw.AudioSampleRate();

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
  //cap.SetThresholds(12, 6); // sets the touch and release thresholds for all 12 channels         // non funziona PD

  while (1)
  {

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
      hw.PrintLine("| Baseline Touched Value : %d |", cap.BaselineData(0));
      hw.PrintLine("| Filtered Touched Value : %d |", cap.FilteredData(0));
      hw.PrintLine("| Difference Touched Value : %d |", cap.BaselineData(0) - cap.FilteredData(0));
      hw.PrintLine("| ");
    } else {
      hw.PrintLine(" ");
      hw.PrintLine(" ");
      hw.PrintLine("Baseline Untouched Value : %d", cap.BaselineData(0));
      hw.PrintLine("Filtered Untouched Value : %d", cap.FilteredData(0));
      hw.PrintLine("Difference Untouched Value : %d", cap.BaselineData(0)-cap.FilteredData(0));
      hw.PrintLine(" ");
      hw.PrintLine(" ");
    }
    
    
    lasttouched = currtouched;

   
    hw.DelayMs(200);
  }
}
