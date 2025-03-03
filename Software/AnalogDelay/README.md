# Analog Delay for Daisy

## Author
Angelo_PoliTeK

## Description
An analog-style delay effect for the Daisy platform.

### Features
- Variable delay time 
- Smooth parameter transitions 
- Cubic interpolation 
- Non-linear delay time control for precise adjustments
- Feedback control 
- Dry/Wet mix control

## Hardware Setup
- **Daisy Seed** board
- 3 potentiometers connected to:
  - Pin A0: Feedback control (0-90%)
  - Pin A1: Mix control (0-100%)
  - Pin A2: Delay time (50ms-1000ms)

## Controls
- **Delay Time**: Non-linear response curve
  - First half: Exponential (50ms-525ms)
  - Second half: Linear (525ms-1000ms)
- **Feedback**: Controls the amount of delay signal fed back into the input
- **Mix**: Balances between dry (original) and wet (delayed) signal

## Future Implementations
- LED indicators for delay timing
- Band pass filter
- Memory usage optimization
- Dub delay (Infinite feedback)

## Usage

The `AnalogDelay` class can be incorporated in your Daisy projects:

```cpp
#include "daisysp.h"
#include "daisy_seed.h"
#include "AnalogDelay.h"

using namespace daisy;
using namespace daisysp;

DaisySeed  hw;
AnalogDelay delay;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        // Process each channel through the delay
        float wetL = delay.Process(in[0][i]);
        float wetR = delay.Process(in[1][i]);
        
        // Output processed signal
        out[0][i] = wetL;
        out[1][i] = wetR;
    }
}

int main(void)
{
    // Initialize hardware
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    
    // Initialize delay
    delay.Init(hw.AudioSampleRate());
    delay.SetDelayTime(0.5f);  // 500ms delay
    delay.SetFeedback(0.7f);   // 70% feedback
    delay.SetMix(0.5f);        // 50/50 dry/wet mix
    
    // Start audio
    hw.StartAudio(AudioCallback);
    
    while(1) {}
}
```
