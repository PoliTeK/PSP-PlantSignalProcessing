/// @file displayHandler.hpp
/// @brief handler for ssd130x

#pragma once

/// @todo make buffer size statically asserted as a power of 2
#include "oled_ssd130x_DMA.h"
#include "../../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"

#define BUFFER_SIZE 256
#define WINDOW_SIZE 128
// Setting the display type (should be changed based on display) 
using MyOledDisplay = daisy::OledDisplay<daisy::SSD130xI2c128x64Driver>;

// The display is implemented as an FSM
enum class DisplayState : uint8_t {
    STANDBY,
    WAVEFORM_VIEWER,
    MENU_MODE // Sospende il disegno automatico per permettere il controllo manuale dei menu
};

class DisplayHandler {
private:

    // pointer to true display object
    MyOledDisplay* _displayPtr = nullptr;

    DisplayState _currentState = DisplayState::STANDBY;

    // used for waveform viewer
    float _circBuffer_ptr [BUFFER_SIZE];
    
    // used as local stable version of circ buffer
    float _dispBuffer_ptr [BUFFER_SIZE];

    int _windowSize = WINDOW_SIZE;
    int _writeHead = 0;
    bool _triggerEnabled;

    const char* _standbyText = "PoliTeK PSP";

    int findTrigger();
    
    /* internal drawing functions */
    void drawStandbyScreen();
    int _yscale = 1;
    int _xscale = 1;
    void drawWaveForm();
    
public:

    DisplayHandler (MyOledDisplay* displayPtr, bool triggerEnabled = true);

    // Update fsm
    void Update();

    // Set new fsm state
    void SetState (DisplayState newState);

    // Sets yscale 
    void SetYscale (int yscale);

    // Set text displayed in STANDBY state
    void SetStandbyText(const char* text);

    // push audio sample to buffer
    void pushAudioSample (float sample);

    // --- METODI PUBBLICI PER LA GRAFICA DEI MENU ---
    // Queste funzioni disegnano direttamente i menu sullo schermo.
    // L'argomento cursorIndex (es. 0, 1, 2) decide su quale riga disegnare la freccia '>'.
    
    void DrawMainMenu(int cursorIndex);
    void DrawCalibrationHub(int cursorIndex);
    void DrawScalesHub(int cursorIndex);
    
    // Metodi generici per l'editing dei parametri (Foglie)
    void DrawFloatParameter(const char* paramName, float value);
    void DrawIntParameter(const char* paramName, int value);

};