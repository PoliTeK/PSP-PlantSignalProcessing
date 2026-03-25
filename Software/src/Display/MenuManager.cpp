#include "MenuManager.h"

void MenuManager::Init() {
    _current_state = PLAYMODE;
    _next_state = MAIN_MENU; // Destinazione di default al primo click

    _outData.state = _current_state;
    _outData.delta = 50.0f;
    _outData.curve = 1.0f;
    _outData.root_note = 0;
    _outData.scale_type = 0;
    _outData.octave = 4;
    _outData.preset = 0;
    _outData.state_just_changed = true;
}

void MenuManager::StateTransition(bool click, int rotation) {
    _outData.state_just_changed = false;

    // --- 1. AGGIORNAMENTO DELLO STATO ---
    if (click) {
        // Gestione speciale per la "risalita" dei menu (BACK non è uno stato persistente)
        if (_next_state == BACK) {
            if (_current_state == MAIN_MENU) _current_state = PLAYMODE;
            else if (_current_state == CALIBRATION_HUB || _current_state == SCALE || _current_state == PRESETS_HUB) _current_state = MAIN_MENU;
        } else {
            _current_state = _next_state;
        }
        
        _outData.state_just_changed = true;
        rotation = 0; // Azzera la rotazione nel ciclo del click per evitare salti accidentali
    }
    
    _outData.state = _current_state;

    // --- 2. RETE DELLE USCITE (Output Logic) ---
    // Definisce cosa succede ai parametri mentri sei fermo nello stato corrente
    if (rotation != 0) {
        switch (_current_state) {
            case DELTA:
                _outData.delta += (rotation * 5.0f);
                if (_outData.delta < 1.0f) _outData.delta = 1.0f;
                if (_outData.delta > 100.0f) _outData.delta = 100.0f;
                break;

            case CURVE:
                _outData.curve += (rotation * 0.1f);
                if (_outData.curve < 0.1f) _outData.curve = 0.1f;
                if (_outData.curve > 2.0f) _outData.curve = 2.0f;
                break;

            case ROOT:
                _outData.root_note += rotation;
                if (_outData.root_note < 0) _outData.root_note = 0;
                if (_outData.root_note > 11) _outData.root_note = 11;
                break;

            case SCALE:
                _outData.scale_type += rotation;
                if (_outData.scale_type < 0) _outData.scale_type = 0;
                if (_outData.scale_type > 5) _outData.scale_type = 5;
                break;

            case OCTAVE:
                _outData.octave += rotation;
                if (_outData.octave < 1) _outData.octave = 1;
                if (_outData.octave > 8) _outData.octave = 8;
                break;

            case PRESETS_HUB:
                // Se i preset si scorrono direttamente nell'hub senza entrare in un sottomenu
                _outData.preset += rotation;
                if (_outData.preset < 0) _outData.preset = 0;
                if (_outData.preset > 10) _outData.preset = 10;
                break;

            default:
                // Negli HUB di navigazione la rotazione non modifica le variabili di uscita
                break;
        }
    }

    // --- 3. RETE DI STATO FUTURO (Next State Logic) ---
    // Definisce dove andrai al prossimo click. Se c'è una rotazione negli HUB, aggiorna il target.
    
    switch (_current_state) {
        case PLAYMODE:
            _next_state = MAIN_MENU; // Dal playmode entri sempre nel menu root
            break;

        case MAIN_MENU:
            if (click) _next_state = CALIBRATION_HUB; // Inizializzazione al momento dell'ingresso
            else if (rotation > 0) {
                if (_next_state == CALIBRATION_HUB) _next_state = SCALES;
                else if (_next_state == SCALES) _next_state = PRESETS_HUB;
                else if (_next_state == PRESETS_HUB) _next_state = BACK;
                else if (_next_state == BACK) _next_state = CALIBRATION_HUB;
            } else if (rotation < 0) {
                if (_next_state == CALIBRATION_HUB) _next_state = BACK;
                else if (_next_state == SCALES) _next_state = CALIBRATION_HUB;
                else if (_next_state == PRESETS_HUB) _next_state = SCALES;
                else if (_next_state == BACK) _next_state = PRESETS_HUB;
            }
            break;

        case CALIBRATION_HUB:
            if (click) _next_state = DELTA;
            else if (rotation > 0) {
                if (_next_state == DELTA) _next_state = CURVE;
                else if (_next_state == CURVE) _next_state = BACK;
                else if (_next_state == BACK) _next_state = DELTA;
            } else if (rotation < 0) {
                if (_next_state == DELTA) _next_state = BACK;
                else if (_next_state == CURVE) _next_state = DELTA;
                else if (_next_state == BACK) _next_state = CURVE;
            }
            break;

        case SCALES:
            if (click) _next_state = ROOT;
            else if (rotation > 0) {
                if (_next_state == ROOT) _next_state = SCALE;
                else if (_next_state == SCALE) _next_state = OCTAVE;
                else if (_next_state == OCTAVE) _next_state = BACK;
                else if (_next_state == BACK) _next_state = ROOT;
            } else if (rotation < 0) {
                if (_next_state == ROOT) _next_state = BACK;
                else if (_next_state == SCALE) _next_state = ROOT;
                else if (_next_state == OCTAVE) _next_state = SCALE;
                else if (_next_state == BACK) _next_state = OCTAVE;
            }
            break;

        case DELTA:
        case CURVE:
            _next_state = CALIBRATION_HUB; // Dalle foglie si torna al padre
            break;

        case ROOT:
        case SCALE:
        case OCTAVE:
            _next_state = SCALES; // Dalle foglie si torna al padre
            break;

        case PRESETS_HUB:
            _next_state = MAIN_MENU; // Confermando il preset si torna alla radice
            break;
            
        default:
            break;
    }
}

MenuManager::MenuData MenuManager::GetData() const {
    // Espone all'esterno l'intera struttura pre-imballata per la lettura
    MenuData data_to_return = _outData;
    
    // Aggiungiamo _next_state al pacchetto di uscita: servirà al Display 
    // per disegnare il cursore (la freccia di selezione) nel posto giusto.
    data_to_return.cursor_target = _next_state; 
    
    return data_to_return;
}