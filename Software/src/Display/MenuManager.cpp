#include "MenuManager.h"

void MenuManager::Init() {
    // Inizializziamo direttamente i campi della struct _outData
    _outData.state = PLAYMODE;
    _outData.cursor_state = PLAYMODE;
    _outData.delta = 50.0f;
    _outData.curve = 1.0f;
    _outData.root = 0;
    _outData.scale = 0;
    _outData.hysteresis = 2;
    _outData.filter_type = 0;  // 0 = BUTTERWORTH_2 di default
    _outData.octave = 4;
    _outData.preset = 0;

    _next_state = MAIN_MENU; // Destinazione di default al primo click
}

void MenuManager::StateTransition(bool click, int rotation, bool timeout) {
    
    // Creiamo una variabile locale per non alterare il parametro in ingresso
    int current_rotation = rotation;

    // --- 1. AGGIORNAMENTO DELLO STATO (State Register) ---
    if (timeout) {
        _outData.state = PLAYMODE;
        _next_state = MAIN_MENU; 
    } 
    else if (click) {
        // Logica per risalire l'albero quando il cursore è su BACK
        if (_next_state == BACK) {
            if (_outData.state == MAIN_MENU) _outData.state = PLAYMODE;
            else if (_outData.state == CALIBRATION_HUB || _outData.state == SCALES_HUB || _outData.state == PRESETS_HUB) _outData.state = MAIN_MENU;
        } 
        else {
            // Normale ingresso nello stato puntato
            _outData.state = _next_state; 
        }

        // --- INIZIALIZZAZIONE DEL CURSORE POST-CLICK ---
        switch (_outData.state) {
            case PLAYMODE:        _next_state = MAIN_MENU; break;
            case MAIN_MENU:       _next_state = CALIBRATION_HUB; break;
            case CALIBRATION_HUB: _next_state = DELTA; break;
            case SCALES_HUB:      _next_state = ROOT; break;
            default: break; 
        }

        // Dopo un click, ignoriamo eventuali rotazioni accidentali nello stesso istante
        current_rotation = 0; 
    }

    // --- 2. RETE DELLE USCITE (Output Logic) ---
    if (current_rotation != 0) {
        switch (_outData.state) {
            case DELTA:
                _outData.delta += (current_rotation * 5.0f);
                if (_outData.delta < 1.0f) _outData.delta = 1.0f;
                if (_outData.delta > 100.0f) _outData.delta = 100.0f;
                break;

            case CURVE:
                _outData.curve += (current_rotation * 0.1f);
                if (_outData.curve < 0.1f) _outData.curve = 0.1f;
                if (_outData.curve > 2.0f) _outData.curve = 2.0f;
                break;
            case HYSTERESIS:
                _outData.hysteresis += (current_rotation * 1);
                if (_outData.hysteresis < 0) _outData.hysteresis = 0;
                if (_outData.hysteresis > 20) _outData.hysteresis = 20;
                break;
            case FILTER_TYPE: 
                // Scorrimento lineare tra le 4 modalità del filtro (0, 1, 2, 3)
                _outData.filter_type += current_rotation;
                if (_outData.filter_type < 0) _outData.filter_type = 0;
                if (_outData.filter_type > 3) _outData.filter_type = 3;
                break;

            case ROOT:
                _outData.root += current_rotation;
                if (_outData.root < 0) _outData.root = 0;
                if (_outData.root > 11) _outData.root = 11;
                break;

            case SCALE:
                _outData.scale += current_rotation;
                if (_outData.scale < 0) _outData.scale = 0;
                if (_outData.scale > 6) _outData.scale = 6;
                break;

            case OCTAVE:
                _outData.octave += current_rotation;
                if (_outData.octave < 1) _outData.octave = 1;
                if (_outData.octave > 8) _outData.octave = 8;
                break;

            case PRESETS_HUB:
                _outData.preset += current_rotation;
                if (_outData.preset < 0) _outData.preset = 0;
                if (_outData.preset > 2) _outData.preset = 2;
                break;

            default:
                break; 
        }
    }

    // --- 3. RETE DI STATO FUTURO (Next State Logic) ---
    if (current_rotation != 0) {
        switch (_outData.state) {
            case MAIN_MENU:
                if (current_rotation > 0) {
                    if (_next_state == CALIBRATION_HUB) _next_state = SCALES_HUB;
                    else if (_next_state == SCALES_HUB) _next_state = PRESETS_HUB;
                    else if (_next_state == PRESETS_HUB) _next_state = BACK;
                    else if (_next_state == BACK) _next_state = CALIBRATION_HUB;
                } else {
                    if (_next_state == CALIBRATION_HUB) _next_state = BACK;
                    else if (_next_state == SCALES_HUB) _next_state = CALIBRATION_HUB;
                    else if (_next_state == PRESETS_HUB) _next_state = SCALES_HUB;
                    else if (_next_state == BACK) _next_state = PRESETS_HUB;
                }
                break;

            case CALIBRATION_HUB:
                if (current_rotation > 0) {
                    if (_next_state == DELTA)                _next_state = CURVE;
                    else if (_next_state == CURVE)           _next_state = HYSTERESIS;
                    else if (_next_state == HYSTERESIS)      _next_state = FILTER_TYPE; 
                    else if (_next_state == FILTER_TYPE)     _next_state = BACK;        
                    else if (_next_state == BACK)            _next_state = DELTA;
                } else {
                    if (_next_state == DELTA)                _next_state = BACK;
                    else if (_next_state == CURVE)           _next_state = DELTA;
                    else if (_next_state == HYSTERESIS)      _next_state = CURVE;
                    else if (_next_state == FILTER_TYPE)     _next_state = HYSTERESIS; 
                    else if (_next_state == BACK)            _next_state = FILTER_TYPE; 
                }
                break;

            case SCALES_HUB:
                if (current_rotation > 0) {
                    if (_next_state == ROOT) _next_state = SCALE;
                    else if (_next_state == SCALE) _next_state = OCTAVE;
                    else if (_next_state == OCTAVE) _next_state = BACK;
                    else if (_next_state == BACK) _next_state = ROOT;
                } else {
                    if (_next_state == ROOT) _next_state = BACK;
                    else if (_next_state == SCALE) _next_state = ROOT;
                    else if (_next_state == OCTAVE) _next_state = SCALE;
                    else if (_next_state == BACK) _next_state = OCTAVE;
                }
                break;
                
            default:
                break;
        }
    }

    // Preparazione statica delle vie d'uscita per le Foglie
    if (_outData.state == DELTA || _outData.state == CURVE || _outData.state == HYSTERESIS || _outData.state == FILTER_TYPE) _next_state = CALIBRATION_HUB;
    if (_outData.state == ROOT || _outData.state == SCALE || _outData.state == OCTAVE) _next_state = SCALES_HUB;
    if (_outData.state == PRESETS_HUB) _next_state = MAIN_MENU; 

    // --- 4. PREPARAZIONE DATI IN USCITA ---
    _outData.cursor_state = _next_state;
}