#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

#ifndef _BV // used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit))
#endif

using namespace daisy;
using namespace daisysp;
//--OLD ARDUINO: Adafruit_MPR121 cap = Adafruit_MPR121();

daisy::Mpr121I2C::Config mpr121ObjConf; // creates config for mpr121 (constructor sets all to deafult)

daisy::Mpr121I2C cap; // creates object for mpr121ù

DaisySeed hw;

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

int main()
{

  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  sample_rate = hw.AudioSampleRate();

  hw.StartLog(true);

  if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
  {
    hw.PrintLine("MPR121 not found, check wiring?");
    while (1)
    {
      hw.SetLed(true);
      hw.DelayMs(500);
      hw.SetLed(false);
      hw.DelayMs(500);
    }
  }
  hw.PrintLine("MPR121 found!");
  //cap.SetThresholds(12, 6); // sets the touch and release thresholds for all 12 channels

  while (1)
  {

    currtouched = cap.Touched();

    
    if ((currtouched & _BV(0)) && !(lasttouched & _BV(0)))
    {
      hw.PrintLine("--------------------------------------------------------------------------------");
      hw.PrintLine("CAP 0 touched");
      hw.PrintLine("%d", cap.FilteredData(0));
      hw.PrintLine("--------------------------------------------------------------------------------");
      
    }

    //if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(0)) && (lasttouched & _BV(0)))
    {
       hw.PrintLine("--------------------------------------------------------------------------------");
       hw.PrintLine("CAP 0 released");
       hw.PrintLine("--------------------------------------------------------------------------------");
       
    }
    
    lasttouched = currtouched;

   
    hw.DelayMs(50);
  }
}
