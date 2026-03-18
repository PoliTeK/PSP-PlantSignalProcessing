#pragma once
#include "daisy_seed.h"
#include "PlantConditioner.h"
#include <cstdint>
#include <cmath>


#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif


class ControlPanel {
public:
    
    void Init(daisy::DaisySeed* hw);

    // Gestisce la logica dei menu e dei sensori (da chiamare nel loop @ 100-200Hz)
    void Update();

    /*
        * \return Target freqeuncy in Hz, calculated based on the current sensor readings and sensitivity settings.
    */
    float GetTargetFreq() const { return _target_freq; }

    /*
        * \return True if the sensor is currently being touched, false otherwise.
    */
    bool  IsTouched()     const { return _is_touched; }

private:
    // --- Menu States ---
    enum MenuMode {
        MODE_DELTA,
        MODE_CURVE,
        MODE_IDLE
    };
    MenuMode _current_mode = MODE_DELTA;

    daisy::DaisySeed* _hw;
    daisy::Encoder _encoder;

    PlantConditioner    _pc;
  
    // --- Variabili di Stato (Dati privati scambiati internamente) ---
    float    _target_freq = 440.0f;
    bool     _is_touched  = false;
    float    _delta_max   = 50.0f;
    float    _curve_type  = 1.1f;
    

    uint8_t _tarature_menu_index = 0;
    void HandleEncoder();
    void UserInterface();
};