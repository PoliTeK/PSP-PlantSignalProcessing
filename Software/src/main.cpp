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

using namespace daisy;

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

// Riattivato il flag per eseguire la lettura fuori dall'interrupt
volatile bool plant_update_param = false; 

// Global accumulation variables to safely handle encoder data across interrupts
volatile int32_t global_inc = 0;
volatile bool global_clicked = false;

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
    // Alza solo la bandierina! Nessuna operazione I2C nell'interrupt per evitare l'Hard Fault
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

    enc.Init(hw.GetPin(14), hw.GetPin(13), hw.GetPin(10));
    menu.Init();

    MyOledDisplay::Config disp_cfg;
    disp.Init(disp_cfg);

    disp_handle.SetYscale(100);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);

    // --- 1. PLANT ACQUISITON SYSTEM INITIALIZATION ---
    // Inizializza con il primo filtro dell'enum come default
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
    tim5_cfg.period        = tim5_period - 1; // -1 because hardware registers are 0-indexed
    tim5_cfg.enable_irq    = true;
    
    enc_timer.Init(tim5_cfg);
    enc_timer.SetPrescaler(prescaler_val); // Apply the prescaler
    enc_timer.SetCallback(EncoderTimerCallback);
    HAL_NVIC_SetPriority(TIM5_IRQn, 4, 0); // Lower priority to avoid interrupting audio
    enc_timer.Start();

    // --- PLANT SENSOR TIMER INTERRUPT CONFIGURATION (200 Hz) ---
    TimerHandle::Config tim3_cfg;
    tim3_cfg.periph        = TimerHandle::Config::Peripheral::TIM_3;
    auto tim3_target_freq  = 200; 
    auto tim3_period       = timer_base_freq / tim3_target_freq;
    tim3_cfg.period        = tim3_period - 1; // -1 because hardware registers are 0-indexed
    tim3_cfg.enable_irq    = true;
    
    plant_timer.Init(tim3_cfg);
    plant_timer.SetPrescaler(prescaler_val); // Apply the prescaler
    plant_timer.SetCallback(PlantTimerCallback);
    HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0); // Lower priority to avoid interrupting audio
    plant_timer.Start();
    
    // Inizializzazione DSP parametri UI
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
        
        // --- CONTROLLO PRESSIONE LUNGA (REBOOT vs DFU) ---
        if (enc.Pressed()) {
            max_held_time = enc.TimeHeldMs();
            
            if (max_held_time >= 4000) {
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
                
                __disable_irq();
                global_clicked = false; 
                __enable_irq();
            }
        } 
        else {
            if (max_held_time >= 2000 && max_held_time < 4000) {
                disp_handle.SetStandbyText("REBOOTING...");
                disp_handle.SetState(DisplayState::STANDBY);
                disp_handle.Update();
                System::Delay(200);
                
                NVIC_SystemReset(); 
            }
            max_held_time = 0; 
        }

        MenuManager::MenuData ui_data;

        // --- TASK 1: ATOMIC ENCODER READ ---
        int32_t local_inc = 0;
        bool local_clicked = false;

        __disable_irq();
        local_inc = global_inc;
        global_inc = 0;
        local_clicked = global_clicked;
        global_clicked = false;
        __enable_irq();

        if (local_inc != 0 || local_clicked) {
            while (local_inc > 0) {
                menu.StateTransition(false, 1, false);
                local_inc--;
            }
            while (local_inc < 0) {
                menu.StateTransition(false, -1, false);
                local_inc++;
            }
            if (local_clicked) {
                menu.StateTransition(true, 0, false);
            }

            // EVENT-DRIVEN: Aggiorna DSP solo se cambi menu
            ui_data = menu.GetData(); 
            
            __disable_irq(); 
            pc.setDelta(ui_data.delta);
            pc.setCurve(ui_data.curve);
            pc.setHisteresis(ui_data.hysteresis);
            pc.setOctave(ui_data.octave);
            synth.SetPreset((SynthPreset) ui_data.preset);
            pc.setScale((PlantConditioner::Notes)ui_data.root, (PlantConditioner::ScaleType)ui_data.scale);
            pc.SetFilter((IIR::FilterType)ui_data.filter_type);
            __enable_irq();  
        }

        // --- TASK 2: PLANT SENSING (200Hz) ---
        // Il calcolo avviene qui, al riparo dalle collisioni I2C!
        if (plant_update_param) {
            plant_update_param = false;
            
            PlantConditioner::PlantState plant_data = pc.Process();
            audio_controls.freq = plant_data._freq;
            audio_controls.gate = plant_data._gate;
        }

        // --- TASK 3: DISPLAY UPDATE (10Hz) ---
        // Portato da 50 a 100 ms per liberare risorse all'I2C dell'MPR121
        if (now - last >= 200) {
            last = now; 
            ui_data = menu.GetData(); 

            if (ui_data.state == MenuManager::PLAYMODE) {
                disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);
            } 
            else {
                disp_handle.SetState(DisplayState::MENU_MODE);
                int cursor_idx = 0;
                
                switch (ui_data.state) {
                    case MenuManager::MAIN_MENU:
                        if (ui_data.cursor_state == MenuManager::CALIBRATION_HUB) cursor_idx = 0;
                        else if (ui_data.cursor_state == MenuManager::SCALES_HUB) cursor_idx = 1;
                        else if (ui_data.cursor_state == MenuManager::PRESETS_HUB) cursor_idx = 2;
                        else if (ui_data.cursor_state == MenuManager::BACK) cursor_idx = 3;
                        disp_handle.DrawMainMenu(cursor_idx);
                        break;

                    case MenuManager::CALIBRATION_HUB:
                        if (ui_data.cursor_state == MenuManager::DELTA) cursor_idx = 0;
                        else if (ui_data.cursor_state == MenuManager::CURVE) cursor_idx = 1;
                        else if (ui_data.cursor_state == MenuManager::HYSTERESIS) cursor_idx = 2;
                        else if (ui_data.cursor_state == MenuManager::FILTER_TYPE) cursor_idx = 3; 
                        else if (ui_data.cursor_state == MenuManager::BACK) cursor_idx = 4;
                        disp_handle.DrawCalibrationHub(cursor_idx);
                        break;

                    case MenuManager::SCALES_HUB:
                        if (ui_data.cursor_state == MenuManager::ROOT) cursor_idx = 0;
                        else if (ui_data.cursor_state == MenuManager::SCALE) cursor_idx = 1;
                        else if (ui_data.cursor_state == MenuManager::OCTAVE) cursor_idx = 2;
                        else if (ui_data.cursor_state == MenuManager::BACK) cursor_idx = 3;
                        disp_handle.DrawScalesHub(cursor_idx);
                        break;

                    case MenuManager::DELTA:
                        disp_handle.DrawFloatParameter("DELTA", ui_data.delta);
                        break;
                    
                    case MenuManager::CURVE:
                        disp_handle.DrawFloatParameter("CURVE", ui_data.curve);
                        break;
                    case MenuManager::HYSTERESIS:
                        disp_handle.DrawIntParameter("HYSTERESIS", ui_data.hysteresis);
                        break;
                    case MenuManager::FILTER_TYPE: 
                        disp_handle.DrawIntParameter("FILTER TYPE", ui_data.filter_type);
                        break;
                    case MenuManager::ROOT:
                        disp_handle.DrawIntParameter("ROOT", ui_data.root);
                        break;

                    case MenuManager::SCALE:
                        disp_handle.DrawIntParameter("SCALE", ui_data.scale);
                        break;

                    case MenuManager::OCTAVE:
                        disp_handle.DrawIntParameter("OCTAVE", ui_data.octave);
                        break;
                        
                    case MenuManager::PRESETS_HUB:
                        disp_handle.DrawIntParameter("PRESET", ui_data.preset);
                        break;

                    default:
                        break;
                }
            } 
            
            // Blocking I2C operation
            disp_handle.Update(); 
        }
    }
}