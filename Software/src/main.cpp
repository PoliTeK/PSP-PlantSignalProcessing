#include "../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"
#include "../libs/PoliTeKDSP/libs/DaisySP/Source/daisysp.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/hid/encoder.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/util/scopedirqblocker.h"
#include "Display/displayHandler.h"
#include "PSP/PlantConditioner.h"
#include "PSP/AudioEngine.h"

using namespace daisy;

#define DISPLAY_REFRESH_FREQ 25u // 25Hz

// Istanze hardware e software
DaisySeed hw;
MyOledDisplay display;
DisplayHandler display_handle(&display);

PlantConditioner pc;
AudioEngine synth;

TimerHandle display_timer;

ControlsStruct audio_controls = {440.0f, false};
TuningStruct tuning_controls = {50.0f, 1.0f};



volatile bool display_refresh_due = false;




void DisplayRefreshTimerCallback(void* data) {
    (void)data;
    display_refresh_due = true;
}



void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    synth.Update(audio_controls);

    for (size_t i = 0; i < size; i++) {
        // 1. Genera il campione una volta sola
        float sig = synth.Process();
        
        // 2. Passalo al display (moltiplicalo per 2 se l'onda è troppo piccola)
        display_handle.pushAudioSample(sig);

        // 3. Liberalo sulle uscite
        out[0][i] = sig;
        out[1][i] = sig;
    }
}

int main() {
    // Inizializzazione Hardware
    hw.Init();

    // Inizializzazione Display OLED
    MyOledDisplay::Config display_cfg;
    display.Init(display_cfg);
    display_handle.SetState(DisplayState::WAVEFORM_VIEWER); 

    // Timer hardware per refresh periodico del display
    TimerHandle::Config display_timer_cfg;
    display_timer_cfg.periph     = TimerHandle::Config::Peripheral::TIM_5;
    display_timer_cfg.enable_irq = true;
    display_timer.Init(display_timer_cfg);
    display_timer.SetCallback(DisplayRefreshTimerCallback);

    auto display_period_ticks = display_timer.GetFreq() / DISPLAY_REFRESH_FREQ;
    if(display_period_ticks == 0)
        display_period_ticks = 1;
    display_timer.SetPeriod(display_period_ticks);
    display_timer.Start();
    
    

    // Inizializzazione PlantConditioner
    pc.Init(IIR::Butterworth, &hw);
    pc.setDelta(50.0f);
    pc.setCurve(1.0f);
    pc.setScale(PlantConditioner::G, PlantConditioner::Major);
    pc.setOctave(4);

    // Inizializzazione AudioEngine
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    // Avvia la callback audio in background
    hw.SetAudioBlockSize(16);
    hw.StartAudio(AudioCallback);

    while(1) {


        PlantConditioner::PlantState plant_data = pc.Process();

        audio_controls.freq = plant_data._freq;
        audio_controls.gate = plant_data._gate;




        
        if(display_refresh_due) {
            display_refresh_due = false; 
            display_handle.Update(); 
        }
        System::Delay(1);
    }
}