#pragma once
#include "daisysp.h"
#include "daisy_seed.h"

#define n 2 // Number of waveforms for each block
#define DC_MAX 0.2    // decide quanto folda (vedi documentazione). Se = 0.5 hai raddoppiato la freq

/**
 * @brief Ramo class for PSP
 * This class include tha main oscillator structure for the PSP synthesizer
 */
class Ramo{
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

    void Init(float sample_rate);                    // Initialization

    float Process();                                 // Processing function

    void setWaveforms(uint8_t wf1, uint8_t wf2);     // Set the waveforms

    void SetFreq(const float f);                     // Set the frequency

    void SetDetune(float ef);                        // Set the detune as a ratio between the two waveforms

    void SetAmp(float g1, float g2);                 // Set the amplitude for each waveform

    void SetShape(float s1, float s2);               // Set the shape for each waveform

private:
    // Private member variables (prefixed with _)
    float   CalcPhaseInc(float f);


    uint8_t _waveform[n];
    float   _amp[n];
    float   _freq, _detune; 
    float   _pw[n], _shape[n], _mshape[n];
    float   _correctorGain[n];
    float   _sr, _sr_recip, _phase[n], _phase_inc;
    float   _last_out, _last_freq;
    bool    _eor[n], _eoc[n];
    float   _correctorGain[n];

};
