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

MidiUsbHandler   midi;

//============================================================================  
// PERSISTENT STORAGE CONFIGURATION
//============================================================================
struct PresetBank {
    Preset_s presets[PRESET_NUM];
    
    bool operator!=(const PresetBank& other) const {
        for(uint8_t i = 0; i < PRESET_NUM; i++) {
            if (presets[i] != other.presets[i]) return true;
        }
        return false;
    }
};
PersistentStorage<PresetBank> storage(hw.qspi);

// ============================================================================
// GLOBAL VARIABLES & FLAGS
// ============================================================================
Control_s audio_controls = {440.0f, false};

TimerHandle enc_timer;
TimerHandle plant_timer;

// Master volume (0.0 to 1.0) for audio output
float master_volume = 1.0f;

// Flag to trigger sensor reading outside the ISR
volatile bool plant_update_param = false; 

// Global accumulation variables to safely handle encoder data across interrupts
std::atomic<int32_t> global_inc(0);
std::atomic<bool> global_clicked(false);

volatile uint32_t display_period = 50; //(ms) => 25 fps
volatile uint32_t master_volume_period = 10; //(ms) => 100 Hz

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
    synth.UpdateControls(audio_controls);

    for(size_t i = 0; i < size; i++) {

        synth.Process(out[0][i], out[1][i]);
        disp_handle.pushAudioSample(out[0][i]); 
    }
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    
    // --- 0. HARDWARE & PERIPHERAL INITIALIZATION ---
    hw.Init();

    // --- ADC INITIALIZATION (MASTER VOLUME) ---
    AdcChannelConfig adc_config;
    adc_config.InitSingle(hw.GetPin(22)); 
    hw.adc.Init(&adc_config, 1);
    hw.adc.Start();

    // --- MIDI USB INITIALIZATION ---
    MidiUsbHandler::Config midi_cfg;
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    midi.Init(midi_cfg); // Tolto hw.

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

    // --- 2.5 PERSISTENT STORAGE INIT ---
    // Crea un oggetto PresetBank che conterrà i valori di fabbrica
    PresetBank default_bank = {};

    // Popola tutti gli slot con valori validi
    for(uint8_t i = 0; i < PRESET_NUM; i++) {
        default_bank.presets[i].index = i;
        sprintf(default_bank.presets[i].name, "Preset %d", i+1);
        // Assegna esplicitamente tutti i campi della struttura
        default_bank.presets[i].osc1 = {daisysp::Oscillator::WAVE_TRI, 1.0f, 0.0f, 0.0f, 2};
        default_bank.presets[i].osc2 = {daisysp::Oscillator::WAVE_TRI, 0.0f, 0.0f, 0.0f, 2};
        default_bank.presets[i].lfo1 = {daisysp::Oscillator::WAVE_SIN, 0.0f, 0.0f, Direction_e::NONE}; 
        default_bank.presets[i].lfo2 = {daisysp::Oscillator::WAVE_SIN, 0.0f, 0.0f, Direction_e::NONE};
        default_bank.presets[i].noise = {0.0f, 0.5f}; // Rumore mutato
        
        default_bank.presets[i].amp_env = {0.01f, 0.1f, 0.8f, 0.1f, 1.0f};
        default_bank.presets[i].filt_env = {0.01f, 0.1f, 0.8f, 0.1f, 0.0f};
        default_bank.presets[i].filter = {20000.0f, 0.0f};
        default_bank.presets[i].reverb = {0.0f, 18000.0f, 0.5f};
        
        default_bank.presets[i].sync = false;
        default_bank.presets[i].ring = false;
    }
    
    // Inizializza lo storage passandogli direttamente l'oggetto di default
    storage.Init(default_bank);
    storage.RestoreDefaults(); // Uncomment this line to reset to factory defaults/ update flash struct

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

    synth.SetActivePreset(storage.GetSettings().presets[init_data.preset]);
    // --- 4. START AUDIO ENGINE ---
    hw.StartAudio(AudioCallback);
    
    uint32_t display_last = System::GetNow();
    uint32_t master_volume_last = System::GetNow();
    
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
            new_params.root       = static_cast<PlantConditioner::Notes>(ui_data.root);
            new_params.scale      = static_cast<PlantConditioner::ScaleType>(ui_data.scale);
            new_params.filter     = static_cast<IIR::FilterType>(ui_data.filter_type);

            uint8_t selected_preset = ui_data.preset;

            __disable_irq(); 

            pc.SetAllParameters(new_params);

            synth.SetActivePreset(storage.GetSettings().presets[selected_preset]);

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
        // --- 3.5: MASTER VOLUME READ ---
        // ====================================================================
        if (now - master_volume_last >= master_volume_period) {
            master_volume_last = now;
            master_volume = hw.adc.GetFloat(0); // Read ADC value (0.0 to 1.0)
            synth.SetMasterVolume(master_volume); // Update the audio engine with the new master volume
        }

        // ====================================================================
        // --- TASK 4: DISPLAY UPDATE ---
        // ====================================================================
        if (now - display_last >= display_period) {
            display_last = now; 
            ui_data = menu.GetData(); 
            
            if (ui_data.current_state == MenuManager::PLAYMODE) {
                disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);
            } 
            else {
                disp_handle.SetState(DisplayState::MENU_MODE);
                disp_handle.DrawStateW(ui_data, storage.GetSettings().presets);
            }
            disp_handle.Update(); 
        }
        // ====================================================================
        // --- TASK 5: MIDI USB READ ---
        // ====================================================================
        midi.Listen();
        
        while (midi.HasEvents()) {
            MidiEvent msg = midi.PopEvent();
            
            if (msg.type == ControlChange) {
                ControlChangeEvent cc = msg.AsControlChange();
                
                // 1. Comando di salvataggio esplicito (es. CC 119)
                // Usiamo un valore > 63 per simulare la pressione di un bottone
                if (cc.control_number == 119 && cc.value > 63) {
                    
                    // Feedback visivo iniziale
                    disp_handle.SetStandbyText("SAVING...");
                    disp_handle.SetState(DisplayState::STANDBY);
                    disp_handle.Update();
                    
                    // Ferma l'audio e disattiva gli interrupt per proteggere la QSPI e la USB
                    __disable_irq();
                    hw.StopAudio(); 
                    
                    storage.Save(); // Scrittura fisica sicura su memoria Flash
                    
                    hw.StartAudio(AudioCallback);
                    __enable_irq();
                    
                    // Feedback visivo finale
                    disp_handle.SetStandbyText("PRESET SAVED");
                    disp_handle.Update();
                    
                    // Breve pausa per rendere leggibile il messaggio
                    System::Delay(600); 
                    
                    // Ripristina lo stato precedente del display
                    MenuManager::MenuData ui_data = menu.GetData();
                    if (ui_data.current_state == MenuManager::PLAYMODE) {
                        disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);
                    } else {
                        disp_handle.SetState(DisplayState::MENU_MODE);
                    }
                    continue; // Passa al prossimo evento MIDI ignorando l'audio engine
                }
                
                // 2. Modifica dei parametri audio in RAM (per tutti gli altri CC)
                uint8_t p_idx = menu.GetData().preset;
                
                __disable_irq();
                Preset_s current_preset = storage.GetSettings().presets[p_idx];
                synth.ProcessMidiCC(cc.control_number, cc.value, current_preset);
                storage.GetSettings().presets[p_idx] = current_preset;
                __enable_irq();
            }
        }
    }
}