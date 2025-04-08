#include <Arduino.h>
// adsr envelope example

#include "daisysp.h"
#include "daisy_seed.h"

// Shortening long macro for sample rate
#ifndef sample_rate

#endif

// Interleaved audio definitions
#define LEFT (i)
#define RIGHT (i + 1)

using namespace daisysp;
using namespace daisy;

static DaisySeed  hw;
static Adsr       env;
static Oscillator osc;
static Metro      tick;
bool              gate;


// put function declarations here:
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
  {
                            
  float osc_out, env_out, lpf_out;
  for(size_t i = 0; i < size; i += 2)
    {
    // When the metro ticks, trigger the envelope to start.
    if(tick.Process())
    {
      gate = !gate;
    }

    // Use envelope to control the amplitude of the oscillator.
    env_out = env.Process(gate);

    

    osc.SetAmp(env_out);
    osc_out = osc.Process();

    // filters oscillator sample
    lpf_out = lpf.Process (osc_out); 

    out[LEFT]  = lpf_out;
    out[RIGHT] = lpf_out;
    }
}

int main() {
  // put your setup code here, to run once:
  static DaisySeed  hw;
  static Adsr       env;
  static Oscillator osc;
  static LadderFilter lpf;

  float sample_rate = hw.AudioSampleRate();
  float cutoff_f = 200;
  lpf.Init(sample_rate);
  // can be given from 5Hz to sample_rate/2 Hz (to verify)
  lpf.SetFreq (cutoff_f);

  env.Init (sample_rate);
  osc.Init (sample_rate);

  // only 3 points needed to draw adsr shape
  env.SetTime(ADSR_SEG_ATTACK, .1);
  env.SetTime(ADSR_SEG_DECAY, .1);
  env.SetTime(ADSR_SEG_RELEASE, .01);

  env.SetSustainLevel(.25);

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_TRI);
  osc.SetFreq(220);
  osc.SetAmp(0.25);

  // start callback
  hw.StartAudio(AudioCallback);
    
}
// put function definitions here:
//int myFunction(int x, int y) {
//  return x + y;
//}