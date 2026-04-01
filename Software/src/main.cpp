#include "../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"
#include "../libs/PoliTeKDSP/libs/DaisySP/Source/daisysp.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/hid/encoder.h"
#include "Display/displayHandler.h"
#include "PSP/PlantConditioner.h"
#include "PSP/AudioEngine.h"
#include "Display/MenuManager.h"

using namespace daisy;

DaisySeed      hw;
Encoder        enc;
MenuManager    menu;
MyOledDisplay  disp;
DisplayHandler disp_handle(&disp);
PlantConditioner pc;
AudioEngine      synth;

ControlsStruct audio_controls = {440.0f, false};

// --- Variabili globali per il Profiling ---
volatile uint32_t audio_ticks_acc = 0;
volatile uint32_t audio_calls = 0;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    uint32_t start = System::GetTick();
    
    synth.Update(audio_controls);
    for (size_t i = 0; i < size; i++) {
        float sig = synth.Process();
        disp_handle.pushAudioSample(sig);
        out[0][i] = out[1][i] = sig;
    }
    
    audio_ticks_acc += (System::GetTick() - start);
    audio_calls++;
}

int main() {
    hw.Init();
    hw.StartLog(false); // Seriale non bloccante

    enc.Init(hw.GetPin(13), hw.GetPin(14), hw.GetPin(10));
    menu.Init();

    MyOledDisplay::Config disp_cfg;
    disp.Init(disp_cfg);
    disp_handle.SetYscale(100);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);

    pc.Init(IIR::FilterType::Butterworth, &hw);
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    hw.StartAudio(AudioCallback);

    uint32_t last_enc_poll = System::GetNow();
    uint32_t disp_cnt = 0;

    // --- Accumulatori per le task del loop principale ---
    uint32_t enc_ticks_acc = 0, enc_calls = 0;
    uint32_t logic_ticks_acc = 0, logic_calls = 0;
    uint32_t disp_ticks_acc = 0, disp_calls = 0;
    uint32_t last_disp_avg = 0; // Mantiene l'ultimo valore valido del display

    while(1) {
        uint32_t now = System::GetNow();

        // --- 1. CAMPIONAMENTO ENCODER ---
        if(now >= last_enc_poll + 1) {
            uint32_t start_enc = System::GetTick();
            last_enc_poll = now; 
            
            enc.Debounce();
            int32_t inc = enc.Increment();
            bool clicked = enc.RisingEdge();

            if(inc != 0 || clicked) {
                menu.StateTransition(clicked, inc, false);
            }
            disp_cnt++;
            
            enc_ticks_acc += (System::GetTick() - start_enc);
            enc_calls++;
        }

        // --- 2. AGGIORNAMENTO LOGICA E PARAMETRI ---
        uint32_t start_logic = System::GetTick();
        
        MenuManager::MenuData ui_data = menu.GetData();
        pc.setDelta(ui_data.delta);
        pc.setCurve(ui_data.curve);
        pc.setOctave(ui_data.octave);
        synth.SetPreset((SynthPreset) ui_data.preset);
        pc.setScale((PlantConditioner::Notes)ui_data.root, (PlantConditioner::ScaleType)ui_data.scale);

        PlantConditioner::PlantState plant_data = pc.Process();
        audio_controls.freq = plant_data._freq;
        audio_controls.gate = plant_data._gate;
        
        logic_ticks_acc += (System::GetTick() - start_logic);
        logic_calls++;

        // --- 3. AGGIORNAMENTO DISPLAY (Ogni 60ms) ---
        if (disp_cnt >= 60) {
            uint32_t start_disp = System::GetTick();
            disp_cnt = 0;

            if (ui_data.state == MenuManager::PLAYMODE) {
                disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);
            } else {
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
                    // ... altri case omessi per brevità ...
                    default: break;
                }
            } 
            
            disp_handle.Update(); 
            last_enc_poll = System::GetNow();
            
            disp_ticks_acc += (System::GetTick() - start_disp);
            disp_calls++;
        }

        // --- 4. TRASMISSIONE DATI SERIALI (Ogni 50 AudioCallback = ~16.6ms) ---
        if (audio_calls >= 50) {
            // Calcolo medie. Evita divisioni per zero se una task non è girata.
            uint32_t a_avg = audio_ticks_acc / audio_calls;
            uint32_t e_avg = enc_calls > 0 ? enc_ticks_acc / enc_calls : 0;
            uint32_t l_avg = logic_calls > 0 ? logic_ticks_acc / logic_calls : 0;
            
            if (disp_calls > 0) {
                last_disp_avg = disp_ticks_acc / disp_calls;
            }

            // Stampa CSV: Audio, Encoder, Logica, Display
            hw.PrintLine("%lu,%lu,%lu,%lu", a_avg, e_avg, l_avg, last_disp_avg);

            // Reset accumulatori
            audio_ticks_acc = 0; audio_calls = 0;
            enc_ticks_acc = 0;   enc_calls = 0;
            logic_ticks_acc = 0; logic_calls = 0;
            disp_ticks_acc = 0;  disp_calls = 0;
        }
    }
}