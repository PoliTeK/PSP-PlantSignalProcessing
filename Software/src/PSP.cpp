#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/Effects/AnalogDelay/src/AnalogDelay.h"
#include "classes/ControlPanel.h"
#define DEBUG_MODE

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

static ControlPanel control;
static Oscillator osc;
static Adsr env;
static OnePole SmoothFreq;




// --- AUDIO CALLBACK (High Priority) ---
static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    float envOut, oscOut;
    for (size_t i = 0; i < size; i += 2)
    {
        // 1. FREQUENCY CONTROL
        float smoothedFreq = SmoothFreq.Process(control.GetTargetFreq());
        osc.SetFreq(smoothedFreq);

        // 2. ENVELOPE GENERATION
        envOut = env.Process(control.IsTouched());
        
        // 3. GAIN STAGING
        osc.SetAmp(envOut * 0.85f); 

        // 4. OSCILLATOR PROCESSING
        oscOut = osc.Process();
        
        out[i] = out[i + 1] = oscOut;
    }
}

int main(){
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(48);
    hw.StartLog(false);
    control.Init(&hw);

    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(Oscillator::WAVE_SIN); 
    osc.SetFreq(440.0f);
    osc.SetAmp(0.85f); 

    env.Init(hw.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, 0.05f);
    env.SetTime(ADSR_SEG_DECAY, 0.1f);
    env.SetSustainLevel(0.7f);
    env.SetTime(ADSR_SEG_RELEASE, 0.5f);

    SmoothFreq.Init();
    SmoothFreq.SetFrequency(10/hw.AudioSampleRate()); // 10 Hz smoothing

    hw.StartAudio(AudioCallback);

    while(1)
    {

        control.Update();

        System::Delay(5); // Loop a 200Hz
    }
}