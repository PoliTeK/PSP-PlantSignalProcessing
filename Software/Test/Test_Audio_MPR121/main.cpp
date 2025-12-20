#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/Temperamento/PlantConditioner.h"
#include "../../Classes/Effects/AnalogDelay/src/AnalogDelay.h"
#include "../../Classes/SensorFilters/IIR/IIR.h"
#include "../../Classes/SensorFilters/MF/MF.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// DEBUG: Uncomment this line to enable serial printing.
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
float f = 440.0f;           // Current frequency
bool gate = false;          // Note On/Off status
uint16_t lastTouched = 0;   // Previous touch state
uint16_t currTouched = 0;   // Current touch state

// --- AUDIO CALLBACK ---
static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    float envOut, oscOut;
    for (size_t i = 0; i < size; i += 2)
    {
        // 1. FREQUENCY CONTROL
        // We pass the frequency calculated in the main directly to the oscillator.
        osc.SetFreq(f);

        // 2. ENVELOPE GENERATION
        envOut = env.Process(gate);
        
        // 3. GAIN STAGING
        osc.SetAmp(envOut * 0.85f); 

        // 4. OSCILLATOR PROCESSING
        oscOut = osc.Process();
        
        // Output to both Left and Right channels
        out[i] = out[i + 1] = oscOut;
    }
}

// --- MAIN FUNCTION ---
int main()
{
    // 1. HARDWARE INITIALIZATION
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);

    // Initialize Serial Logger
    #ifdef Debug
    hw.StartLog(false);
    #endif

    // 2. MPR121 INITIALIZATION
    mpr121ObjConf.transport_config.speed = daisy::I2CHandle::Config::Speed::I2C_400KHZ;

    if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
    {
        #ifdef Debug
        hw.PrintLine("ERROR: MPR121 not found. Check wiring!");
        #endif
        while (1) {
            hw.SetLed(true); hw.DelayMs(100);
            hw.SetLed(false); hw.DelayMs(100);
        }
    }
    
    #ifdef Debug
    hw.PrintLine("MPR121 Initialized @ 400kHz!");
    #endif
    hw.SetLed(true);

    // 3. DSP INITIALIZATION
    pc.Init(IIR::Butterworth); 
    pc.setScale(PlantConditioner::C, PlantConditioner::MinorArm);
    pc.setOctave(2);
    pc.setCurve(50, 1.1);

    // Oscillator setup 
    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(Oscillator::WAVE_SIN); 
    osc.SetFreq(440.0f);
    osc.SetAmp(0.85f); 

    // Envelope setup
    env.Init(hw.AudioSampleRate());
    env.SetTime(ADSR_SEG_ATTACK, 0.05f);
    env.SetTime(ADSR_SEG_DECAY, 0.1f);
    env.SetSustainLevel(0.7f);
    env.SetTime(ADSR_SEG_RELEASE, 0.5f);

    // 4. ADC INITIALIZATION
    adcConfig[0].InitSingle(daisy::seed::A0);
    adcConfig[1].InitSingle(daisy::seed::A1);
    hw.adc.Init(adcConfig, ADC_CH);
    hw.adc.Start();

    // Start Audio Callback
    hw.StartAudio(AudioCallback);

    // --- MAIN LOOP ---
    uint32_t last_tick = System::GetNow();
    uint32_t last_print = System::GetNow();

    while (1)
    {
        uint32_t now = System::GetNow();

        // A. FAST LOOP  for sensor (200Hz - Every 5ms)
        if (now - last_tick >= 5)
        {
            last_tick = now;

            // Read ADC Parameters
            float curve_type = hw.adc.GetFloat(1) * 2 + 1;
            float delta_max = hw.adc.GetFloat(0) * 100 + 20;
            pc.setCurve(delta_max, curve_type);

            // Read MPR121
            currTouched = cap.Touched();
            
            // Process Filters
            float target_freq = pc.Process(cap.BaselineData(0), cap.FilteredData(0));
            
            bool is_touched_now = (currTouched & _BV(0));

            // Gate Logic
            if (is_touched_now)
            {
                if (!gate) gate = true; 
                f = target_freq; // Updates instantly now
            }
            else
            {
                if (gate) gate = false; 
            }
            lastTouched = currTouched;
        }

        // B. SLOW LOOP for Debug (4Hz - Every 250ms) 
        #ifdef Debug
        if (now - last_print >= 250)
        {
            last_print = now;
            
            if (gate) {
                hw.PrintLine("TOUCH | Freq: %.2f Hz | Delta: %.2f | Filt: %.2f", f, pc.getDelta(), pc.getDeltaFilt());
            } else {
                hw.PrintLine("..... | Freq: %.2f Hz | Delta: %.2f | Filt: %.2f", f, pc.getDelta(), pc.getDeltaFilt());
            }
        }
        #endif
        
        // C. CPU REST  ciao matteo, ogni volta che lo tolgo gemini mi insulta
        System::Delay(1);
    }
}