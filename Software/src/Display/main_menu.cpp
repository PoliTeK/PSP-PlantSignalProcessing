#include "daisy_seed.h"
#include "hid/encoder.h"
#include "MenuManager.h"
#include "displayHandler.h"

using namespace daisy;

DaisySeed      hw;
Encoder        enc;
MenuManager    menu;
MyOledDisplay  disp;
DisplayHandler disp_handle(&disp);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // Audio callback pulita, solo per DSP
    for (size_t i = 0; i < size; i++) {
        out[0][i] = out[1][i] = 0.0f;
    }
}

int main() {
    hw.Init();
    
    // Inizializzazione fisica (Pin 13, 14, 10)
    enc.Init(hw.GetPin(13), hw.GetPin(14), hw.GetPin(10));
    menu.Init();

    // Inizializzazione Display
    MyOledDisplay::Config disp_cfg;
    disp.Init(disp_cfg);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER);

    hw.StartAudio(AudioCallback);

    uint32_t last_enc_poll = System::GetNow();
    uint32_t disp_cnt = 0;

    while(1) {
        uint32_t now = System::GetNow();

        // 1. CAMPIONAMENTO ENCODER
        if(now >= last_enc_poll + 1) {
            last_enc_poll = now; // Allineamento preciso
            
            enc.Debounce();
            int32_t inc = enc.Increment();
            bool clicked = enc.RisingEdge();

            if(inc != 0 || clicked) {
                menu.StateTransition(clicked, inc, false);
            }
            
            disp_cnt++;
        }

        // 2. AGGIORNAMENTO DISPLAY (Ogni 50ms per dare più respiro all'encoder)
        if (disp_cnt >= 50) {
            disp_cnt = 0;

            MenuManager::MenuData ui_data = menu.GetData();

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

                    case MenuManager::DELTA:
                        disp_handle.DrawFloatParameter("DELTA", ui_data.delta);
                        break;

                    default:
                        break;
                }
            } 
            // Invia i dati (operazione bloccante)
            disp_handle.Update(); 
            
            // Dopo l'update, resetta il tempo per non far scattare immediatamente il poll successivo
            last_enc_poll = System::GetNow();
        }
    }
}