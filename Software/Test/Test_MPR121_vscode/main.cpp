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
bool touched;

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

    for (uint8_t i = 0; i < 12; i++)
    {
      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)))
      {
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP %d touched", i);
        hw.PrintLine("--------------------------------------------------------------------------------");
      }

      // if it *was* touched and now *isnt*, alert!
      if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)))
      {
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP %d released", i);
        hw.PrintLine("--------------------------------------------------------------------------------");
      }
    }
    lasttouched = currtouched;

    #ifdef FiltAndBase
      

      hw.PrintLine("Filt--00--01--02--03--04--05--06--07--08--09--10--11");
      hw.Print("    ");
      for (uint8_t i = 0; i < 12; i++)
      {

        hw.Print("  %d", (int)cap.FilteredData(i));
      }
      hw.PrintLine("\n");

      hw.PrintLine("Base--00--01--02--03--04--05--06--07--08--09--10--11: ");
      hw.Print("    ");
      for (uint8_t i = 0; i < 12; i++)
      {
        hw.Print("  %d", (int)cap.BaselineData(i));
      }
      hw.PrintLine("\n");

      hw.DelayMs(100);
      hw.PrintLine("--------------------------------------------------------------------------------\n");
    # endif // FiltAndBase
  }
}
