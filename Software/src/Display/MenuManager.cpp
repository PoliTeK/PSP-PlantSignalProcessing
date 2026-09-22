#include "MenuManager.h"

void MenuManager::Init() {
    _lastInteractionTime = daisy::System::GetNow();

    _current_state = PLAYMODE;
    _cursor_state = PLAYMODE;

    _outData.delta = 50.0f;
    _outData.curve = 1.0f;
    _outData.root = 0;
    _outData.scale = 0;
    _outData.hysteresis = 2;
    _outData.filter_type = 0;  
    _outData.octave = 4;
    _outData.preset = 0;
    _outData.touchths_value = 1;
    _outData.relths_value = 1;
}

void MenuManager::StateUpdate() {
    _lastInteractionTime = daisy::System::GetNow(); 
    
    // 1. Uscita dal PLAYMODE
    if (_current_state == PLAYMODE) {
        _current_state = MAIN_MENU;
        _cursor_state = CALIBRATION_HUB; // Inizializza sulla prima voce del menu
        return;
    }

    // 2. USCITA DALLE FOGLIE (Conferma modifica e torna al genitore)
    // Se stiamo modificando un valore, il click ci riporta all'Hub di appartenenza.
    // _cursor_state non viene toccato, così il cursore resta sull'icona appena modificata.
    if (_current_state == DELTA || _current_state == CURVE || _current_state == HYSTERESIS || _current_state == FILTER_TYPE) {
        _current_state = CALIBRATION_HUB;
        return; 
    }
    if (_current_state == ROOT || _current_state == SCALE || _current_state == OCTAVE) {
        _current_state = SCALES_HUB;
        return;
    }
    if (_current_state == TOUCHTHS_VALUE || _current_state == RELTHS_VALUE) {
        _current_state = THRESHOLDS_HUB;
        return;
    }
    if (_current_state == SAVE_CONFIG || _current_state == LOAD_CONFIG) {
        _current_state = FLASH_HUB;
        return;
    }
    if (_current_state == PRESETS_HUB) { // Gestito come foglia del Main Menu
        _current_state = MAIN_MENU;
        return;
    }

    // 3. GESTIONE DEL TASTO BACK (Torna alla schermata precedente)
    if (_cursor_state == BACK) {
        if (_current_state == MAIN_MENU) {
            _current_state = PLAYMODE;
            _cursor_state = PLAYMODE;
        } 
        else if (_current_state == CALIBRATION_HUB || _current_state == SCALES_HUB || _current_state == FLASH_HUB) {
            MenuState previous_hub = _current_state; 
            _current_state = MAIN_MENU;
            _cursor_state = previous_hub; // Lascia il cursore sull'Hub da cui siamo appena usciti
        } 
        else if (_current_state == THRESHOLDS_HUB) {
            _current_state = CALIBRATION_HUB; 
            _cursor_state = THRESHOLDS_HUB;
        }
        return;
    }

    // 4. TRANSIZIONE NORMALE: Entriamo in un Hub o iniziamo a modificare una Foglia
    _current_state = _cursor_state;

    // 5. INIZIALIZZAZIONE CURSORE (Solo quando apriamo un nuovo Hub)
    if (_current_state == CALIBRATION_HUB) _cursor_state = DELTA;
    else if (_current_state == SCALES_HUB) _cursor_state = ROOT;
    else if (_current_state == THRESHOLDS_HUB) _cursor_state = TOUCHTHS_VALUE;
    else if (_current_state == FLASH_HUB) _cursor_state = SAVE_CONFIG;
}

bool MenuManager::IsLeafState() {
    return (_current_state == DELTA || _current_state == CURVE || 
            _current_state == HYSTERESIS || _current_state == FILTER_TYPE ||
            _current_state == ROOT || _current_state == SCALE || 
            _current_state == OCTAVE || _current_state == TOUCHTHS_VALUE || 
            _current_state == RELTHS_VALUE || _current_state == PRESETS_HUB ||
            _current_state == SAVE_CONFIG || _current_state == LOAD_CONFIG);
}


MenuManager::MenuState MenuManager::CursorUpdate(int rotation) {   
    _lastInteractionTime = daisy::System::GetNow(); 
    switch (_current_state)
    {
    case PLAYMODE:
        // In PLAYMODE, the cursor state does not change with rotation
        break;
    case MAIN_MENU:
        if (rotation > 0) {
            if (_cursor_state == CALIBRATION_HUB) _cursor_state = SCALES_HUB;
            else if (_cursor_state == SCALES_HUB) _cursor_state = PRESETS_HUB;
            else if (_cursor_state == PRESETS_HUB) _cursor_state = FLASH_HUB;
            else if (_cursor_state == FLASH_HUB) _cursor_state = BACK;
            else if (_cursor_state == BACK) _cursor_state = CALIBRATION_HUB;
        } else {
            if (_cursor_state == BACK) _cursor_state = FLASH_HUB;
            else if (_cursor_state == FLASH_HUB)  _cursor_state = PRESETS_HUB;                    
            else if (_cursor_state == PRESETS_HUB) _cursor_state = SCALES_HUB;                                       
            else if (_cursor_state == SCALES_HUB) _cursor_state = CALIBRATION_HUB;
            else if (_cursor_state == CALIBRATION_HUB) _cursor_state = BACK;
        }
        break;
    case CALIBRATION_HUB:
        if (rotation > 0) {
            if (_cursor_state == DELTA)                _cursor_state = CURVE;
            else if (_cursor_state == CURVE)           _cursor_state = HYSTERESIS;
            else if (_cursor_state == HYSTERESIS)      _cursor_state = FILTER_TYPE; 
            else if (_cursor_state == FILTER_TYPE)     _cursor_state = THRESHOLDS_HUB;
            else if (_cursor_state == THRESHOLDS_HUB)  _cursor_state = BACK;       
            else if (_cursor_state == BACK)            _cursor_state = DELTA;
        } else {
            if (_cursor_state == DELTA)                _cursor_state = BACK;
            else if (_cursor_state == CURVE)           _cursor_state = DELTA;
            else if (_cursor_state == HYSTERESIS)      _cursor_state = CURVE;
            else if (_cursor_state == FILTER_TYPE)     _cursor_state = HYSTERESIS;
            else if (_cursor_state == THRESHOLDS_HUB)  _cursor_state = FILTER_TYPE;
            else if (_cursor_state == BACK)            _cursor_state = THRESHOLDS_HUB; 
        }
        break;
    case SCALES_HUB:
        if (rotation > 0) {
            if (_cursor_state == ROOT) _cursor_state = SCALE;
            else if (_cursor_state == SCALE) _cursor_state = OCTAVE;
            else if (_cursor_state == OCTAVE) _cursor_state = BACK;
            else if (_cursor_state == BACK) _cursor_state = ROOT;
        } else {
            if (_cursor_state == ROOT) _cursor_state = BACK;
            else if (_cursor_state == SCALE) _cursor_state = ROOT;
            else if (_cursor_state == OCTAVE) _cursor_state = SCALE;
            else if (_cursor_state == BACK) _cursor_state = OCTAVE;
        }
        break;
    case THRESHOLDS_HUB:
        if (rotation > 0) {
            if (_cursor_state == TOUCHTHS_VALUE) _cursor_state = RELTHS_VALUE;
            else if (_cursor_state == RELTHS_VALUE) _cursor_state = BACK;
            else if (_cursor_state == BACK) _cursor_state = TOUCHTHS_VALUE;
        } else {
            if (_cursor_state == TOUCHTHS_VALUE) _cursor_state = BACK;
            else if (_cursor_state == RELTHS_VALUE) _cursor_state = TOUCHTHS_VALUE;
            else if (_cursor_state == BACK) _cursor_state = RELTHS_VALUE;
        }
        break;
    case FLASH_HUB:
        if (rotation > 0) {
            if (_cursor_state == SAVE_CONFIG) _cursor_state = LOAD_CONFIG;
            else if (_cursor_state == LOAD_CONFIG) _cursor_state = BACK;
            else if (_cursor_state == BACK)  _cursor_state = SAVE_CONFIG;
        } else {
            if (_cursor_state == SAVE_CONFIG) _cursor_state = BACK;
            else if (_cursor_state == LOAD_CONFIG) _cursor_state = SAVE_CONFIG;
            else if (_cursor_state == BACK) _cursor_state = LOAD_CONFIG;
        }
        break;
    default:
        break;
    }

    if (_current_state == DELTA || _current_state == CURVE || _current_state == HYSTERESIS || _current_state == FILTER_TYPE) _cursor_state = CALIBRATION_HUB;
    if (_current_state == ROOT || _current_state == SCALE || _current_state == OCTAVE) _cursor_state = SCALES_HUB;
    if (_current_state == TOUCHTHS_VALUE || _current_state == RELTHS_VALUE) _cursor_state = THRESHOLDS_HUB;
    if (_current_state == SAVE_CONFIG || _current_state == LOAD_CONFIG) _cursor_state = FLASH_HUB;
    if (_current_state == PRESETS_HUB) _cursor_state = MAIN_MENU; 


    return _cursor_state;
}

void MenuManager::ValueUpdate(int rotation){
    _lastInteractionTime = daisy::System::GetNow(); 
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
        
        case HYSTERESIS:
            _outData.hysteresis += (rotation * 1);
            if (_outData.hysteresis < 0) _outData.hysteresis = 0;
            if (_outData.hysteresis > 20) _outData.hysteresis = 20;
            break;
        
        case FILTER_TYPE:                 
            _outData.filter_type += rotation;
            if (_outData.filter_type < 0) _outData.filter_type = 0;
            if (_outData.filter_type > 3) _outData.filter_type = 3;
            break;

        case ROOT:
            _outData.root += rotation;
            if (_outData.root < 0) _outData.root = 0;
            if (_outData.root > 11) _outData.root = 11;
            break;

        case SCALE:
            _outData.scale += rotation;
            if (_outData.scale < 0) _outData.scale = 0;
            if (_outData.scale > 6) _outData.scale = 6;
            break;

        case OCTAVE:
            _outData.octave += rotation;
            if (_outData.octave < 1) _outData.octave = 1;
            if (_outData.octave > 8) _outData.octave = 8;
            break;

        case TOUCHTHS_VALUE:
            _outData.touchths_value += rotation;
            // check that value fits in an 8 bit register 
            if (_outData.touchths_value < 0) _outData.touchths_value = 0;
            if (_outData.touchths_value > 255) _outData.touchths_value = 255;
            break;

        case RELTHS_VALUE:
            _outData.relths_value += rotation;
            // check that value fits in an 8 bit register 
            if (_outData.relths_value < 0) _outData.relths_value = 0;
            if (_outData.relths_value > 255) _outData.relths_value = 255;
            break;    

        case PRESETS_HUB:
            _outData.preset += rotation;
            if (_outData.preset < 0) _outData.preset = 0;
            if (_outData.preset > 2) _outData.preset = 2;
            break;

        case SAVE_CONFIG:
            _outData.configs_idx += rotation;
            if (_outData.configs_idx < 0) _outData.configs_idx = 0;
            break;

        case LOAD_CONFIG:
            _outData.configs_idx += rotation;
            if (_outData.configs_idx < 0) _outData.configs_idx = 0;
            break;

        default:
            break; 
    }
}

    


void MenuManager::MenuTimeout(uint32_t currentTime) {
    if (_current_state != PLAYMODE && (currentTime - _lastInteractionTime >= 5000)) {
        _lastInteractionTime = currentTime;
        _current_state = PLAYMODE;
        
        _cursor_state = MAIN_MENU;
    }
}

MenuManager::MenuData MenuManager::GetData() {
    _outData.current_state = _current_state;
    _outData.cursor_state = _cursor_state;
    return _outData;
}