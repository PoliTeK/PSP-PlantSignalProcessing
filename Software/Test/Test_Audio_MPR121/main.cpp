#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/Temperamento/PlantConditioner.h"
#include "../../Classes/Effects/AnalogDelay/src/AnalogDelay.h"
#include "../../Classes/SensorFilters/IIR/IIR.h"
#include "../../Classes/SensorFilters/MF/MF.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// DEBUG: Uncomment this line to enable serial printing via USB.
// Keep commented for best audio performance.
#define Debug 

#define ADC_CH 2

using namespace daisy;
using namespace daisysp;

// --- GLOBAL OBJECTS ---
daisy::Mpr121I2C::Config mpr121ObjConf;
daisy::Mpr121I2C cap;
DaisySeed hw;
AdcChannelConfig adcConfig[ADC_CH];
PlantConditioner pc;
static Oscillator osc;
static Adsr env;

// --- STATE VARIABLES ---
volatile float target_freq = 440.0f; 
bool gate = false;          
uint16_t lastTouched = 0;   
uint16_t currTouched = 0;   

// Fixed: Added initialization and missing semicolon
float curve_type = 1.0f;
float delta_max = 50.0f;

// --- AUDIO CALLBACK (High Priority) ---
static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    float envOut, oscOut;
    for (size_t i = 0; i < size; i += 2)
    {
        // 1. FREQUENCY CONTROL
        osc.SetFreq(target_freq);

        // 2. ENVELOPE GENERATION
        envOut = env.Process(gate);
        
        // 3. GAIN STAGING
        osc.SetAmp(envOut * 0.85f); 

        // 4. OSCILLATOR PROCESSING
        oscOut = osc.Process();
        
        out[i] = out[i + 1] = oscOut;
    }
}

// --- MAIN FUNCTION ---
int main()
{
    // 1. HARDWARE INIT
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);

    #ifdef Debug
    hw.StartLog(false);
    #endif

    // 2. MPR121 INIT
    mpr121ObjConf.transport_config.speed = daisy::I2CHandle::Config::Speed::I2C_400KHZ;

    if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
    {
        #ifdef Debug
        hw.PrintLine("ERROR: MPR121 not found.");
        #endif
        while (1) {
            hw.SetLed(true); hw.DelayMs(100);
            hw.SetLed(false); hw.DelayMs(100);
        }
    }
    
    #ifdef Debug
    hw.PrintLine("MPR121 OK @ 400kHz");
    #endif
    hw.SetLed(true);

    // 3. DSP INIT
    pc.Init(IIR::Butterworth); 
    pc.setScale(PlantConditioner::C, PlantConditioner::MinorArm);
    pc.setOctave(2);
    pc.setCurve(50, 1.1);

    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(Oscillator::WAVE_SIN); 
    osc.SetFreq(440.0f);
    osc.SetAmp(0.85f); 

    env.Init(hw.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, 0.05f);
    env.SetTime(ADSR_SEG_DECAY, 0.1f);
    env.SetSustainLevel(0.7f);
    env.SetTime(ADSR_SEG_RELEASE, 0.5f);

    // 4. ADC INIT
    adcConfig[0].InitSingle(daisy::seed::A0);
    adcConfig[1].InitSingle(daisy::seed::A1);
    hw.adc.Init(adcConfig, ADC_CH);
    hw.adc.Start();

    hw.StartAudio(AudioCallback);

    // --- MAIN LOOP ---
    uint32_t last_tick = System::GetNow();
    #ifdef Debug
        uint32_t last_print = System::GetNow();
    #endif

    while (1)
    {
        uint32_t now = System::GetNow();

        // A. FAST LOOP (200Hz)
        if (now - last_tick >= 5)
        {
            last_tick = now;

            // Read Analog Controls
            curve_type = hw.adc.GetFloat(1) * 2 + 1;
            delta_max = hw.adc.GetFloat(0) * 100 + 20;
            pc.setCurve(delta_max, curve_type);

            // Read Digital Sensor
            currTouched = cap.Touched();
            
            // Calculate Frequency (but don't apply it to the oscillator yet)
            float calculated_freq = pc.Process(cap.BaselineData(0), cap.FilteredData(0));

            // Update Gate and Target Frequency based on Touch State
            if (currTouched & _BV(0))
            {
                if (!gate) gate = true; 
                // We just update the variable here. The Audio Callback picks it up instantly.
                target_freq = calculated_freq; 
            }
            else
            {
                if (gate) gate = false; 
            }
            lastTouched = currTouched;
        }

        // B. DEBUG LOOP (4Hz)
        #ifdef Debug
        if (now - last_print >= 250)
        {
            last_print = now;
            if (gate) {
                hw.PrintLine("TOUCH | Freq: %.2f | Delta: %.2f | DeltaFilt: %.2f", target_freq, pc.getDelta(), pc.getDeltaFilt());
            } else {
                hw.PrintLine("..... | Freq: %.2f | Delta: %.2f | DeltaFilt: %.2f", target_freq, pc.getDelta(), pc.getDeltaFilt());
            }
        }
        #endif
        
        // C. CPU REST
        System::Delay(1);
    }
}