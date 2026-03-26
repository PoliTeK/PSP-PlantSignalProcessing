#pragma once
#include <stdint.h>

class MenuManager {
public:
    // Rinominato in MenuState per coerenza con il resto del codice
    enum MenuState {
        PLAYMODE,           // Visualizzazione Onda
        MAIN_MENU,
        // Nodi principali (Hub)
        CALIBRATION_HUB,
        SCALES_HUB,         // Rinominato per coerenza con PRESETS_HUB
        PRESETS_HUB,

        // Foglie: Parametri Calibrazione
        CURVE,
        DELTA,

        // Foglie: Parametri Scale
        ROOT,
        SCALE,
        OCTAVE,

        BACK,
    };

    // Struttura espansa con i nuovi parametri
    struct MenuData {
        MenuState state;
        MenuState cursor_state;
        float delta;
        float curve;
        int root;       // Es: 0 = C, 1 = C#...
        int scale;      // Es: 0 = Major, 1 = Minor...
        int octave;          // Es: 1, 2, 3, 4
        int preset;    // Indice per il synth
        //bool state_just_changed; 
    };

    void Init();
    
    void StateTransition(bool click, int rotation, bool timeout);
    
    MenuData GetData() const{return _outData};

   

private:
    MenuState _current_state, _next_state;
    
    // NUOVO: Serve per sapere su quale voce sei "posizionato" mentre navighi in un HUB
    MenuState _cursor_state; 

    MenuData _outData;

    float _delta, _curve;
    int _root, _scale, _octave, _preset;




};