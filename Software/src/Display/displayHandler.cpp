#include "displayHandler.h"
#include <string.h>
#include <stdio.h>

DisplayHandler::DisplayHandler(MyOledDisplay* displayPtr, bool triggerEnabled) {
    memset((void*) _circBuffer_ptr, 0, BUFFER_SIZE * sizeof(float));
    memset((void*) _dispBuffer_ptr, 0, BUFFER_SIZE * sizeof(float));
    
    _writeHead = 0;
    _windowSize = WINDOW_SIZE; 
    _triggerEnabled = triggerEnabled;
    _displayPtr = displayPtr;
    _currentState = DisplayState::WAVEFORM_VIEWER;
    _standbyText = "PoliTeK PSP";
    _yscale = 20; // Default di sicurezza
}

void DisplayHandler::Update() {
    switch (_currentState) {
        case DisplayState::STANDBY:
            drawStandbyScreen();
            break;

        case DisplayState::WAVEFORM_VIEWER:
            drawWaveForm();
            break;

        case DisplayState::MENU_MODE:
            break;
    }
    
    _displayPtr->Update();
}

void DisplayHandler::SetState(DisplayState newState) {
    _displayPtr->Fill(false);
    _currentState = newState;
}

void DisplayHandler::SetStandbyText(const char* text) {
    if(text != nullptr) {
        _standbyText = text;
        if (_currentState == DisplayState::STANDBY) {
            _displayPtr->Fill(false);
        }
    }
}

void DisplayHandler::pushAudioSample(float sample) {
    _circBuffer_ptr[_writeHead] = sample;
    _writeHead = (_writeHead + 1) % BUFFER_SIZE;
}

int DisplayHandler::findTrigger() {
    float currVal = 0;
    float nextVal = 0;
    static int trigIdx = 0;

    int oldestSampleIdx = _writeHead + 1;

    if (_triggerEnabled) {
        for (int i = oldestSampleIdx; i < oldestSampleIdx + _windowSize; i++) {
            currVal = _circBuffer_ptr[i % BUFFER_SIZE];
            nextVal = _circBuffer_ptr[(i + 1) % BUFFER_SIZE];
             
            if (currVal < 0.0f && nextVal >= 0.0f) {
                trigIdx = i % BUFFER_SIZE;
                return trigIdx; 
            }
        }
    }           
    return trigIdx;
}

void DisplayHandler::drawWaveForm() {
    int zeroOfScreen = 32; 
    int y1, y2;
    int idx = 0;

    _displayPtr->SetCursor(0, zeroOfScreen);
    _displayPtr->Fill(false);

    idx = findTrigger();
    
    for (int i = idx; i < idx + _windowSize; i++) {
        y1 = zeroOfScreen - (int)(_circBuffer_ptr[i % BUFFER_SIZE] * _yscale);
        y2 = zeroOfScreen - (int)(_circBuffer_ptr[(i + 1) % BUFFER_SIZE] * _yscale);
        
        if (y1 < 0) { y1 = 0; } 
        if (y1 > 63) { y1 = 63; }
        if (y2 < 0) { y2 = 0; } 
        if (y2 > 63) { y2 = 63; }

        _displayPtr->DrawLine(i - idx, y1, (i + 1) - idx, y2, true);
    }   
}

void DisplayHandler::SetYscale(int yscale){
    _yscale=yscale;
}

void DisplayHandler::drawStandbyScreen(){
    _displayPtr->SetCursor(0,0);
    _displayPtr->WriteString(_standbyText, Font_11x18, true);
}

// ============================================================================
// METODI PUBBLICI PER IL MENU (SPAZIATURA OTTIMIZZATA PER 64PX)
// ============================================================================

void DisplayHandler::DrawMainMenu(int cursorIndex) {
    _displayPtr->Fill(false);
    
    _displayPtr->SetCursor(0, 0);
    _displayPtr->WriteString("- MAIN MENU -", Font_7x10, true);

    _displayPtr->SetCursor(10, 13); _displayPtr->WriteString("Calibration", Font_7x10, true);
    _displayPtr->SetCursor(10, 23); _displayPtr->WriteString("Scales", Font_7x10, true);
    _displayPtr->SetCursor(10, 33); _displayPtr->WriteString("Presets", Font_7x10, true);
    _displayPtr->SetCursor(10, 43); _displayPtr->WriteString("Exit", Font_7x10, true);

    int cursorY = 13 + (cursorIndex * 10);
    _displayPtr->SetCursor(0, cursorY);
    _displayPtr->WriteString(">", Font_7x10, true);
}

void DisplayHandler::DrawCalibrationHub(int cursorIndex) {
    _displayPtr->Fill(false);
    
    _displayPtr->SetCursor(0, 0);
    _displayPtr->WriteString("- CALIBRATION -", Font_7x10, true);

    _displayPtr->SetCursor(10, 13); _displayPtr->WriteString("Delta", Font_7x10, true);
    _displayPtr->SetCursor(10, 23); _displayPtr->WriteString("Curve", Font_7x10, true);
    _displayPtr->SetCursor(10, 33); _displayPtr->WriteString("Hysteresis", Font_7x10, true);
    _displayPtr->SetCursor(10, 43); _displayPtr->WriteString("Filter Type", Font_7x10, true); 
    _displayPtr->SetCursor(10, 53); _displayPtr->WriteString("Back", Font_7x10, true); 

    int cursorY = 13 + (cursorIndex * 10);
    _displayPtr->SetCursor(0, cursorY);
    _displayPtr->WriteString(">", Font_7x10, true);
}

void DisplayHandler::DrawScalesHub(int cursorIndex) {
    _displayPtr->Fill(false);
    
    _displayPtr->SetCursor(0, 0);
    _displayPtr->WriteString("- SCALES -", Font_7x10, true);

    _displayPtr->SetCursor(10, 13); _displayPtr->WriteString("Root Note", Font_7x10, true);
    _displayPtr->SetCursor(10, 23); _displayPtr->WriteString("Scale Type", Font_7x10, true);
    _displayPtr->SetCursor(10, 33); _displayPtr->WriteString("Octave", Font_7x10, true);
    _displayPtr->SetCursor(10, 43); _displayPtr->WriteString("Back", Font_7x10, true);

    int cursorY = 13 + (cursorIndex * 10);
    _displayPtr->SetCursor(0, cursorY);
    _displayPtr->WriteString(">", Font_7x10, true);
}

void DisplayHandler::DrawFloatParameter(const char* paramName, float value) {
    _displayPtr->Fill(false);
    
    _displayPtr->SetCursor(0, 0);
    _displayPtr->WriteString("EDIT:", Font_7x10, true);
    
    _displayPtr->SetCursor(0, 12);
    _displayPtr->WriteString(paramName, Font_11x18, true);

    int intPart = (int)value;
    int decPart = (int)((value - (float)intPart) * 100.0f);
    if (decPart < 0) decPart = -decPart;

    char valBuffer[16];
    sprintf(valBuffer, "%d.%02d", intPart, decPart);
    
    _displayPtr->SetCursor(0, 35); 
    _displayPtr->WriteString(valBuffer, Font_11x18, true); 
}

void DisplayHandler::DrawIntParameter(const char* paramName, int value) {
    _displayPtr->Fill(false);
    
    _displayPtr->SetCursor(0, 0);
    _displayPtr->WriteString("EDITING:", Font_7x10, true);
    
    _displayPtr->SetCursor(0, 15);
    _displayPtr->WriteString(paramName, Font_11x18, true);

    char valBuffer[16];

    if (strcmp(paramName, "ROOT") == 0) {
        static const char* notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        sprintf(valBuffer, "%s", notes[value % 12]);
    }
    else if (strcmp(paramName, "SCALE") == 0) {
        static const char* scales[] = {"Major", "MinorN", "MinorA", "MinorM", "Penta", "Chrom", "Esat"};
        sprintf(valBuffer, "%s", scales[value % 7]);
    }
    else if (strcmp(paramName, "PRESET") == 0) {
        static const char* presets[] = {"Pad", "Pluck", "Roygbib"};
        sprintf(valBuffer, "%s", presets[value % 3]);
    }
    else if (strcmp(paramName, "HYSTERESIS") == 0) {
        sprintf(valBuffer, "%d %%", value); 
    }
    else if (strcmp(paramName, "FILTER TYPE") == 0) { 
        static const char* filters[] = {"Btrwth 2nd", "Btrwth 4th", "Bessel 2nd", "Bessel 4th"};
        sprintf(valBuffer, "%s", filters[value % 4]);
    }
    else {
        sprintf(valBuffer, "%d", value);
    }
    
    _displayPtr->SetCursor(0, 38);
    _displayPtr->WriteString(valBuffer, Font_11x18, true); 
}