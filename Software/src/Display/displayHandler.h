/// @file displayHandler.hpp
/// @brief handler for ssd130x

#pragma once

/// @todo make buffer size statically asserted as a power of 2
/// @todo improve drawPotValue screen with parameter name
#include "../../libs/PoliTeKDSP/libs/libDaisy/src/dev/oled_ssd130x.h"
#include "../../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"

#define BUFFER_SIZE 256
#define WINDOW_SIZE 128
// Setting the display type (should be changed based on display) 
using MyOledDisplay = daisy::OledDisplay<daisy::SSD130xI2c128x64Driver>;

// The display is implemented as an FSM

enum class DisplayState : uint8_t {
    STANDBY,
    WAVEFORM_VIEWER,
    POT_VALUE
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

    // might not be needed      
    float* _potValuePtr;
    const char* _standbyText = "PoliTeK PSP";

    int findTrigger();
    /* internal drawing functions */
    void drawStandbyScreen();
    int _yscale = 1;
    int _xscale = 1;
    void drawWaveForm();

    // RIGHT NOW IT ONLY SHOWS THE NUMBER
    void drawPotValue();
    
public:

    DisplayHandler (MyOledDisplay* displayPtr, bool triggerEnabled = true) ;

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

};