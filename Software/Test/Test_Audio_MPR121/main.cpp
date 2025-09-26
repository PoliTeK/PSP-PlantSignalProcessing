#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/FIIR/CapFir.h"
#include "../../Classes/Temperamento/PlantConditioner.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define ADC_CH 2 // Numero di canali ADC da utilizzare
//#define DEBUG
using namespace daisy;
using namespace daisysp;

daisy::Mpr121I2C::Config mpr121ObjConf; // creates config for mpr121 (constructor sets all to deafult)
daisy::Mpr121I2C cap;                   // creates object for mpr121ù

DaisySeed hw;
AdcChannelConfig adcConfig[ADC_CH];
CapFir CurveFilter; // creates object for the CapFir filter
CapFir DeltaFilter;
CapFir MaxFilter;
PlantConditioner pc;

static Oscillator osc;     
float f = 0.0f;     
float delta = 0.0f; 
float filtDelta = 0.0f; 
bool gate = false;  

bool flag = false; // used for debug printing

static Adsr env;


uint16_t lastTouched = 0;                               // last touched value
uint16_t currTouched = 0;                               // current touched value                                        ù
uint8_t touchTreshold = MPR121_TOUCH_THRESHOLD_DEFAULT; 

float output = 0.0f; // used to store the output value

static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
  float envOut, oscOut;
  for (size_t i = 0; i < size; i += 2)
  {

    envOut = env.Process(gate); 
    osc.SetAmp(envOut);

    oscOut = osc.Process();
    out[i] = out[i + 1] = oscOut;               // outputs the same value to left and right channels
  }
}

int main()
{
  int k = 0;

  CurveFilter.Init(CapFir::ResType::HIGH);
  DeltaFilter.Init(CapFir::ResType::HIGH);
  MaxFilter.Init(CapFir::ResType::HIGH);

  float sampleRate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  sampleRate = hw.AudioSampleRate();

  pc.Init(CapFir::ResType::HIGH); 
  pc.setScale(PlantConditioner::C, PlantConditioner::MinorArm);
  pc.setOctave(3);
  pc.setCurve(100,1.1);
  

  osc.Init(sampleRate);                  // initializes the oscillator with the sample rate
  osc.SetWaveform(Oscillator::WAVE_TRI); // sets the waveform
  osc.SetFreq(440.0f);                   // sets the frequency
  osc.SetAmp(0.5f);                      // sets the amplitude

  env.Init(sampleRate); // initializes the envelope with the sample rate
  env.SetTime(ADSR_SEG_ATTACK, 0.05f);
  env.SetTime(ADSR_SEG_DECAY, 0.1f);
  env.SetSustainLevel(0.7f);
  env.SetTime(ADSR_SEG_RELEASE, 0.2f);

  adcConfig[0].InitSingle(daisy::seed::A0); 
  adcConfig[1].InitSingle(daisy::seed::A1); 
  hw.adc.Init(adcConfig, ADC_CH);              
  hw.adc.Start();                

  #ifdef DEBUG
  hw.StartLog(false); // starts the log to the serial port
  #endif
  hw.StartLog(false);

  daisy::Mpr121I2C::Result status = cap.Init(mpr121ObjConf);
  if (status != daisy::Mpr121I2C::Result::OK) // initializes the mpr121 with the config and see if the comunication is ok
  {
    while (1)
    {
      hw.SetLed(true); // blink if not ok
      hw.DelayMs(500);
      hw.SetLed(false);
      hw.DelayMs(500);    }
  }
  else
  {
    hw.SetLed(true);
  }

  // cap.SetThresholds(12, 6);                                           // sets the touch and release thresholds for all 12 channels         // non funziona PD
  hw.StartAudio(AudioCallback);

  while (1)
  {
    float curve_type = hw.adc.GetFloat(1)*2 + 1;
    float delta_max = hw.adc.GetFloat(0)*90 + 30;
    pc.setCurve(MaxFilter.Process(delta_max), CurveFilter.Process(curve_type));
    

    currTouched = cap.Touched();                                   // reads the touched channels from the mpr121
    if ((currTouched & _BV(0)) && !(lastTouched & _BV(0)) || gate) // if the channel 0 is touched and it was not touched before
    {
      gate = true; // sets the gate to true
      f = pc.Process(cap.BaselineData(0), cap.FilteredData(0));
      osc.SetFreq(f);
    }

    if (!(currTouched & _BV(0)) && (lastTouched & _BV(0))) // if the channel 0 was touched and it is not touched now
    {
      gate = false; // sets the gate to false
    }

    // calculates the frequency based on the delta value

#ifdef DEBUG
    if (gate)
    {
      if (!flag){
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP touched");
        flag = true;
      }
      hw.PrintLine("| ");
      hw.PrintLine("| Delta : %f, | DeltaFiltered :  %f | Frequency : %f ", pc.getDelta(), pc.getDeltaFilt(), f);
      hw.PrintLine("| ");
      
    }
    else
    {
      if ( flag)
      {
        hw.PrintLine("CAP released");
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine(" ");
        flag = false;
      }
      k++;
      if (k>50){
        hw.PrintLine(" ");
        hw.PrintLine("deltaMax : %f", delta_max);
        hw.PrintLine("curveType : %f", curve_type);
        k = 0;
      }
    }
#endif

    lastTouched = currTouched;
    
      hw.PrintLine("%f, %f, %f", f, pc.getDelta(), pc.getDeltaFilt());
      //hw.PrintLine(" ");
      //hw.PrintLine("%d, %d ", cap.BaselineData(0), cap.FilteredData(0));
      //hw.PrintLine(" ");
    
   
    hw.DelayMs(10);
    

  }
}
