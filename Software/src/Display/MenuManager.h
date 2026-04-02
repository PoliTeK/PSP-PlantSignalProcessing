#pragma once
#include <stdint.h>

class MenuManager {
public:
    enum MenuState {
        PLAYMODE,           // Visualizzazione Onda
        MAIN_MENU,
        
        // Nodi principali (Hub)
        CALIBRATION_HUB,
        SCALES_HUB,         
        PRESETS_HUB,

        // Foglie: Parametri Calibrazione
        CURVE,
        DELTA,
        HYSTERESIS,
        FILTER_TYPE,        // Aggiornato per gestire le 4 modalità

        // Foglie: Parametri Scale
        ROOT,
        SCALE,
        OCTAVE,

        BACK,
    };

    struct MenuData {
        MenuState state;
        MenuState cursor_state;
        float delta;
        float curve;
        int root;           // Es: 0 = C, 1 = C#...
        int hysteresis;     // Isteresi, in percentuale del range totale
        int filter_type;    // 0=Butter2, 1=Butter4, 2=Bessel2, 3=Bessel4
        int scale;          // Es: 0 = Major, 1 = Minor...
        int octave;         // Es: 1, 2, 3, 4
        int preset;         // Indice per il synth
    };

    void Init();
    void StateTransition(bool click, int rotation, bool timeout);
    MenuData GetData() { return _outData; }

private:
    MenuState _current_state, _next_state;
    MenuState _cursor_state; 

    MenuData _outData;

    // Aggiunte le variabili private mancanti per il tracciamento interno
    float _delta, _curve;
    int _root, _scale, _octave, _preset, _hysteresis, _filter_type;

};