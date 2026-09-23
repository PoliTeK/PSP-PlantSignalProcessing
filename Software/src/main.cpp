// ============================================================================
// INCLUDES & NAMESPACES
// ============================================================================
#include "../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"
#include "../libs/PoliTeKDSP/libs/DaisySP/Source/daisysp.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/hid/encoder.h"
#include "Display/displayHandler.h"
#include "PSP/PlantConditioner.h"
#include "PSP/AudioEngine.h"
#include "Display/MenuManager.h"
#include <atomic>
#define DEUG

using namespace daisy;
#ifdef DEBUG
    GPIO Sensing_test_pin;
    GPIO Display_test_pin;
#endif
// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
DaisySeed      hw;
Encoder        enc;
MenuManager    menu;
MyOledDisplay  disp;
DisplayHandler disp_handle(&disp);

PlantConditioner pc;
AudioEngine      synth;

// ============================================================================
// GLOBAL VARIABLES & FLAGS
// ============================================================================
ControlsStruct audio_controls = {440.0f, false};

TimerHandle enc_timer;
TimerHandle plant_timer;

// Flag to trigger sensor reading outside the ISR
volatile bool plant_update_param = false; 

// Global accumulation variables to safely handle encoder data across interrupts
std::atomic<int32_t> global_inc(0);
std::atomic<bool> global_clicked(false);

volatile uint32_t display_period = 100; //(ms) => 10 fps

// ============================================================================
// INTERRUPT SERVICE ROUTINES AND AUDIO CALLBACK
// ============================================================================

void EncoderTimerCallback(void* data) {
    enc.Debounce(); 
    // Safely accumulate encoder increments inside the hardware interrupt
    global_inc += enc.Increment(); 
    if (enc.RisingEdge()) {
        global_clicked = true;
    }
}

void PlantTimerCallback(void* data) {
    // Set flag only. Avoid I2C operations in ISR to prevent Hard Faults.
    plant_update_param = true;
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    synth.Update(audio_controls);

    for (size_t i = 0; i < size; i++) {
        float sig = synth.Process();
        disp_handle.pushAudioSample(sig);
        out[0][i] = out[1][i] = sig;
    }
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    
    // --- 0. HARDWARE & PERIPHERAL INITIALIZATION ---
    hw.Init();
    #ifdef DEBUG
        Sensing_test_pin.Init(hw.GetPin(15), GPIO::Mode::OUTPUT);
        Display_test_pin.Init(hw.GetPin(12), GPIO::Mode::OUTPUT);
        Sensing_test_pin.Write(false);
        Display_test_pin.Write(false);
    #endif
    

    enc.Init(hw.GetPin(14), hw.GetPin(13), hw.GetPin(10));
    menu.Init();

    // 1. Crea la configurazione per la nuova classe
    MyOledDisplay::Config display_config;
    disp.Init(display_config);
    disp_handle.SetYscale(100);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);

    // --- 1. PLANT ACQUISITION SYSTEM INITIALIZATION ---
    pc.Init(IIR::BUTTERWORTH2, &hw);

    // --- 2. DSP INITIALIZATION ---
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    // --- 3. TIMERS CONFIGURATION ---
    // Timer Prescaler Calculation: scale core clock down to 1 MHz (1 tick = 1 us)
    auto pclk_freq = daisy::System::GetPClk2Freq();
    uint32_t prescaler_val = (pclk_freq / 1000000) - 1;
    auto timer_base_freq = 1000000;

    // --- ENCODER TIMER INTERRUPT CONFIGURATION (1000 Hz) ---
    TimerHandle::Config tim5_cfg;
    tim5_cfg.periph        = TimerHandle::Config::Peripheral::TIM_5;
    auto tim5_target_freq  = 1000;
    auto tim5_period       = timer_base_freq / tim5_target_freq;
    tim5_cfg.period        = tim5_period - 1; // 0-indexed hardware register
    tim5_cfg.enable_irq    = true;
    
    enc_timer.Init(tim5_cfg);
    enc_timer.SetPrescaler(prescaler_val);
    enc_timer.SetCallback(EncoderTimerCallback);
    HAL_NVIC_SetPriority(TIM5_IRQn, 4, 0); // Lower priority to avoid interrupting audio
    enc_timer.Start();

    // --- PLANT SENSOR TIMER INTERRUPT CONFIGURATION (200 Hz) ---
    TimerHandle::Config tim3_cfg;
    tim3_cfg.periph        = TimerHandle::Config::Peripheral::TIM_3;
    auto tim3_target_freq  = 200; 
    auto tim3_period       = timer_base_freq / tim3_target_freq;
    tim3_cfg.period        = tim3_period - 1; 
    tim3_cfg.enable_irq    = true;
    
    plant_timer.Init(tim3_cfg);
    plant_timer.SetPrescaler(prescaler_val);
    plant_timer.SetCallback(PlantTimerCallback);
    HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0); 
    plant_timer.Start();
    
    // Initialize DSP parameters from UI default data
    MenuManager::MenuData init_data = menu.GetData(); 
    pc.setDelta(init_data.delta);
    pc.setCurve(init_data.curve);
    pc.setHisteresis(init_data.hysteresis);
    pc.setOctave(init_data.octave);
    pc.setScale((PlantConditioner::Notes)init_data.root, (PlantConditioner::ScaleType)init_data.scale);
    pc.SetFilter((IIR::FilterType)init_data.filter_type);
    synth.SetPreset((SynthPreset) init_data.preset);

    // --- 4. START AUDIO ENGINE ---
    hw.StartAudio(AudioCallback);
    uint32_t last = System::GetNow();
    
    // ========================================================================
    // MAIN LOOP
    // ========================================================================
    static uint32_t max_held_time = 0;
    
    while(1) {
        uint32_t now = System::GetNow();
        
        // --- LONG PRESS DETECTION (REBOOT / BOOTLOADER) ---
        if (enc.Pressed()) {
            max_held_time = enc.TimeHeldMs();
            
            if (max_held_time >= 6000) {
                disp_handle.SetStandbyText("ENTERING DFU...");
                disp_handle.SetState(DisplayState::STANDBY);
                disp_handle.Update();
                System::Delay(200);
                daisy::System::ResetToBootloader();
            } 
            else if (max_held_time >= 2000) {
                disp_handle.SetStandbyText("RELEASE=REBOOT");
                disp_handle.SetState(DisplayState::STANDBY);
                disp_handle.Update();
                global_clicked = false; 
            }
        } 
        else {
            if (max_held_time >= 2000 && max_held_time < 6000) {
                disp_handle.SetStandbyText("REBOOTING...");
                disp_handle.SetState(DisplayState::STANDBY);
                disp_handle.Update();
                System::Delay(200);
                
                NVIC_SystemReset(); 
            }
            max_held_time = 0; 
        }

        // ====================================================================
        // --- TASK 1:ENCODER READ ---
        // ====================================================================
        int32_t local_inc = 0;
        bool local_clicked = false;
        local_inc = global_inc;
        global_inc = 0;
        local_clicked = global_clicked;
        global_clicked = false;
       

        // 1. Read state before evaluating interactions
        MenuManager::MenuData ui_data = menu.GetData(); 

        // Update menu state based on encoder rotation
        if (local_inc != 0) {
            if (menu.IsLeafState()) {
                menu.ValueUpdate(local_inc);
            } else {
                while (local_inc > 0) {
                    menu.CursorUpdate(1);
                    local_inc--;
                }
                while (local_inc < 0) {
                    menu.CursorUpdate(-1);
                    local_inc++;
                }
            }
        }

        // update menu state based on encoder click
        if (local_clicked) {
            menu.StateUpdate(); 
        }

        // 2. Update DSP parameters if there are changes
        if (local_inc != 0 || local_clicked) {
            ui_data = menu.GetData();
            PlantConditioner::PlantParams new_params;
            new_params.delta      = ui_data.delta;
            new_params.curve      = ui_data.curve;
            new_params.hysteresis = ui_data.hysteresis;
            new_params.octave     = ui_data.octave;
            new_params.root       = (PlantConditioner::Notes)ui_data.root;
            new_params.scale      = (PlantConditioner::ScaleType)ui_data.scale;
            new_params.filter     = (IIR::FilterType)ui_data.filter_type;
            __disable_irq(); 
            pc.SetAllParameters(new_params);
            synth.SetPreset((SynthPreset) ui_data.preset);
            __enable_irq();   
            if (local_clicked && ui_data.current_state == MenuManager::THRESHOLDS_HUB) {
                pc.setThresholds(ui_data.touchths_value, ui_data.relths_value);
            }
        }

        menu.MenuTimeout(now);
        // ====================================================================
        // --- 3: PLANT SENSING ---
        // ====================================================================
        
        
        if (plant_update_param) {
            plant_update_param = false;
            PlantConditioner::PlantState plant_data = pc.Process();
            audio_controls.freq = plant_data._freq;
            audio_controls.gate = plant_data._gate;
        }

        // ====================================================================
        // --- TASK 4: DISPLAY UPDATE ---
        // ====================================================================
        if (now - last >= display_period) {
            last = now; 
            ui_data = menu.GetData(); 
            
            if (ui_data.current_state == MenuManager::PLAYMODE) {
                disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);
            } 
            else {
                disp_handle.SetState(DisplayState::MENU_MODE);
                disp_handle.DrawStateW(ui_data);
            }
            
            // Blocking I2C operation
            disp_handle.Update(); 
        }
    }
}