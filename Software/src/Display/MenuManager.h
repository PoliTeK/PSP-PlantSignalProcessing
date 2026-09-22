#pragma once
#include <stdint.h>
#include <daisy.h>

class MenuManager {
public:
    enum MenuState {
        PLAYMODE, MAIN_MENU,
        
        // Main Hubs
        CALIBRATION_HUB, SCALES_HUB, PRESETS_HUB, THRESHOLDS_HUB, FLASH_HUB,

        // Leafs
        CURVE, DELTA, HYSTERESIS, FILTER_TYPE, //Calibration
        ROOT, SCALE, OCTAVE,                   //Scales
        TOUCHTHS_VALUE, RELTHS_VALUE,          //Thresholds
        LOAD_CONFIG, SAVE_CONFIG,              //Flash

        BACK
    };

    struct MenuData {
        MenuState current_state;
        MenuState cursor_state;
        float delta;
        float curve;
        int root;           // Es: 0 = C, 1 = C#...
        int hysteresis;     // Isteresi, in percentuale del range totale
        int filter_type;    // 0=Butter2, 1=Butter4, 2=Bessel2, 3=Bessel4
        int scale;          // Es: 0 = Major, 1 = Minor...
        int octave;         // Es: 1, 2, 3, 4
        int touchths_value; // Value of touch hresholds
        int relths_value;   // Value of release hresholds
        int preset;         // Indice per il synth
        int configs_idx;    // indexes of flash configs
    };

    void Init();

    MenuState CursorUpdate(int rotation);
    void StateUpdate();
    void MenuTimeout(uint32_t currentTime);
    void ValueUpdate(int rotation);
    MenuData GetData();
    bool IsLeafState();

private:
    MenuState _current_state;
    MenuState _cursor_state; 
    MenuData _outData;

    uint32_t _lastInteractionTime;
};