#pragma once
#include "daisysp.h"
#include "daisy_seed.h"
#include <cstdint>

#define n 2        // Number of waveforms for each block
#define DC_MAX 0.2 // decide quanto folda (vedi documentazione). Se = 0.5 hai raddoppiato la freq

/**
 * @brief Ramo class for PSP
 * This class include tha main oscillator structure for the PSP synthesizer
 */

class Ramo
{
public:
    Ramo();
    ~Ramo();

    enum 
    {
        WAVE_SIN,
        WAVE_TRI,
        WAVE_SQUARE,
        WAVE_SAW,
        WAVE_LAST,
    };

    void Init(float sample_rate); // Initialization

    float Process(); // Processing function

    void SetWaveforms(uint8_t wf1, uint8_t wf2); // Set the waveforms

    void SetFreq(const float f); // Set the frequency

    void SetDetune(float ef1, float ef2); // Set the detune as a ratio between the two waveforms

    void SetAmp(float g1, float g2); // Set the amplitude for each waveform

    void SetShape(float s1); // Set the shape for each waveform

private:
    // Private member variables (prefixed with _)
    float CalcPhaseInc(float f); // Calculate the phase increment
    uint8_t _waveform[n];
    float _amp[n];
    float _freq;
    float _pw, _shape, _mshape;
    float _correctorGain;
    float _sr, _sr_recip, _phase[n], _phase_inc[n];
    float _last_out, _last_freq;
    bool _eor[n], _eoc[n];
};
