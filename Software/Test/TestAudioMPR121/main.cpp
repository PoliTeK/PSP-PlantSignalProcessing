#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define DEBUG
using namespace daisy;
using namespace daisysp;

daisy::Mpr121I2C::Config mpr121ObjConf; // creates config for mpr121 (constructor sets all to deafult)
daisy::Mpr121I2C cap;                   // creates object for mpr121ù

DaisySeed hw;

static Oscillator osc;     // creates an oscillator object
float f = 0.0f;     // used to store the frequency
float delta = 0.0f; // used to store the delta value
bool gate = false;  // used to store the gate value

uint16_t lastTouched = 0;                               // used to store the last touched value
uint16_t currTouched = 0;                               // used to store the current touched value                                         // used to check if the cap was touched or not
uint8_t touchTreshold = MPR121_TOUCH_THRESHOLD_DEFAULT; // used to set the touch treshold

float output = 0.0f; // used to store the output value

static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{

  for (size_t i = 0; i < size; i += 2)
  {

    if (gate)
    {
      out[i] = out[i + 1] = osc.Process(); // if the gate is true, set the output to 0
    }
    else
    {
      out[i] = out[i + 1] = 0.0f;
    }
  }
}

int main()
{



  float sampleRate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  sampleRate = hw.AudioSampleRate();

  osc.Init(sampleRate);                  // initializes the oscillator with the sample rate
  osc.SetWaveform(Oscillator::WAVE_TRI); // sets the waveform
  osc.SetFreq(440.0f);                   // sets the frequency
  osc.SetAmp(0.5f);                      // sets the amplitude

  #ifdef DEBUG
  hw.StartLog(false); // starts the log to the serial port
  #endif


  daisy::Mpr121I2C::Result status = cap.Init(mpr121ObjConf);
  if (status != daisy::Mpr121I2C::Result::OK) // initializes the mpr121 with the config and see if the comunication is ok
  {
    while (1)
    {
      hw.SetLed(true); // blink if not ok
      hw.DelayMs(500);
      hw.SetLed(false);
      hw.DelayMs(500);
    }
  }
  else
  {
    hw.SetLed(true);
  }

  // cap.SetThresholds(12, 6);                                           // sets the touch and release thresholds for all 12 channels         // non funziona PD
  hw.StartAudio(AudioCallback);
  while (1)
  {
    currTouched = cap.Touched();                                   // reads the touched channels from the mpr121
    if ((currTouched & _BV(0)) && !(lastTouched & _BV(0)) || gate) // if the channel 0 is touched and it was not touched before
    {
      delta = (float)(cap.FilteredData(0) - cap.BaselineData(0) - touchTreshold); // calculates the delta between the filtered and baseline data
      gate = true;
      f = 440 + (delta / 40) * 440;
      osc.SetFreq(f);
#ifdef DEBUG // sets the gate to true
      hw.PrintLine("--------------------------------------------------------------------------------");
      hw.PrintLine("CAP 0 touched");
#endif
    }

    if (!(currTouched & _BV(0)) && (lastTouched & _BV(0))) // if the channel 0 was touched and it is not touched now
    {
      gate = false; // sets the gate to false
#ifdef DEBUG
      hw.PrintLine("CAP 0 released");
      hw.PrintLine("--------------------------------------------------------------------------------");
      hw.PrintLine(" ");
#endif
    }

    // calculates the frequency based on the delta value

#ifdef DEBUG
    if (gate)
    {
      hw.PrintLine("| ");
      hw.PrintLine("| Baseline Touched Value : %d |", cap.BaselineData(0));
      hw.PrintLine("| Filtered Touched Value : %d |", cap.FilteredData(0));
      hw.PrintLine("| Difference Touched Value : %d |", cap.BaselineData(0) - cap.FilteredData(0));
      hw.PrintLine("| ");
    }
    else
    {
      hw.PrintLine(" ");
      hw.PrintLine(" ");
      hw.PrintLine("Baseline Untouched Value : %d", cap.BaselineData(0));
      hw.PrintLine("Filtered Untouched Value : %d", cap.FilteredData(0));
      hw.PrintLine("Difference Untouched Value : %d", cap.BaselineData(0) - cap.FilteredData(0));
      hw.PrintLine(" ");
      hw.PrintLine(" ");
    }
#endif

    lastTouched = currTouched;

    hw.DelayMs(100);
  }
}
