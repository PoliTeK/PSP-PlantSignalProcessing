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

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // 1. Aggiorna il synth con i parametri calcolati nel main
    synth.Update(audio_controls);

    for (size_t i = 0; i < size; i++) {
        float sig = synth.Process();
        
        // Passa il campione per la visualizzazione dell'onda
        disp_handle.pushAudioSample(sig);

        out[0][i] = out[1][i] = sig;
    }
}

int main() {
    hw.Init();

    // Inizializzazione fisica encoder (Pin 13, 14, 10)
    enc.Init(hw.GetPin(13), hw.GetPin(14), hw.GetPin(10));
    menu.Init();

    // Inizializzazione Display
    MyOledDisplay::Config disp_cfg;
    disp.Init(disp_cfg);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);

    // Inizializzazione Moduli DSP
    pc.Init(IIR::FilterType::Butterworth, &hw);
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    hw.StartAudio(AudioCallback);

    uint32_t last_enc_poll = System::GetNow();
    uint32_t disp_cnt = 0;

    while(1) {
        uint32_t now = System::GetNow();

        // --- 1. CAMPIONAMENTO ENCODER ---
        if(now >= last_enc_poll + 1) {
            last_enc_poll = now; 
            
            enc.Debounce();
            int32_t inc = enc.Increment();
            bool clicked = enc.RisingEdge();

            if(inc != 0 || clicked) {
                menu.StateTransition(clicked, inc, false);
            }
            
            disp_cnt++;
        }

        // --- 2. AGGIORNAMENTO LOGICA E PARAMETRI ---
        MenuManager::MenuData ui_data = menu.GetData();

        // Sincronizza i parametri dal Menu alle classi DSP
        pc.setDelta(ui_data.delta);
        pc.setCurve(ui_data.curve);
        pc.setOctave(ui_data.octave);
        synth.SetPreset((SynthPreset) ui_data.preset);
        pc.setScale((PlantConditioner::Notes)ui_data.root, (PlantConditioner::ScaleType)ui_data.scale);

        // Calcolo della frequenza dalla pianta
        PlantConditioner::PlantState plant_data = pc.Process();
        audio_controls.freq = plant_data._freq;
        audio_controls.gate = plant_data._gate;

        // --- 3. AGGIORNAMENTO DISPLAY (Ogni 50 cicli = 50ms) ---
        if (disp_cnt >= 60) {
            disp_cnt = 0;

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
                        else if (ui_data.cursor_state == MenuManager::BACK) cursor_idx = 2;
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
            
            // Operazione bloccante I2C
            disp_handle.Update(); 
            
            // Reset fondamentale del tempo per evitare "salti" dell'encoder dopo l'update
            last_enc_poll = System::GetNow();
        }
    }
}