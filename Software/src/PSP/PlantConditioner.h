#pragma once
#include <cstdint>
#include <cmath>
#include "../../libs/PoliTeKDSP/Utilities/DataFilter/IIR/iir.h"
#include "../../libs/PoliTeKDSP/Utilities/DataFilter/MF/MF.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
struct TuningStruct{
    float _deltaMax;
    float _curve;
};
class PlantConditioner {
public:
    // --- Constructor and Destructor ---
    PlantConditioner();
    ~PlantConditioner();

    // --- PlantConditioner Output Struct---
    struct PlantState {
        float _freq;
        bool  _gate;
    };

    

    // --- Enums for Scale Types and Notes ---
    enum ScaleType {
        Major = 0,
        MinorNat,
        MinorArm,
        MinorMel,
        Pentatonic,
        Chromatic,
        Esatonic,
    };
    enum Notes {
        C = 0, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B,
    };

    /*
        * Initializes the PlantConditioner with the specified filter type and hardware reference.
            * @param filter_type The type of IIR filter to use for processing the plant signal.
            * @param hw A pointer to the DaisySeed hardware object for sensor interfacing and control.
    */
    void Init(IIR::FilterType filter_type, daisy::DaisySeed* hw);

    /*
        * Sets the curve type for mapping the plant signal to musical notes.
            * @param curve_type A float value where 1.0 is linear, >1 is exponential, and <1 is logarithmic.
    */
    void setCurve(float curve_type);

    /*
        * Sets the maximum delta value for the plant signal, which defines the upper bound of the input range.
            * @param delta_max The maximum delta value. Must be greater than delta_min.
    */
    void setDelta(float delta_max);

    /*
        * Updates the note thresholds based on the current delta range and curve type. This should be called whenever
        * the delta range, curve type or scale is changed to ensure that the note mapping remains accurate.
    */
    void updateThresholds();

    /*
        * Sets the hysteresis value for note changes to prevent rapid toggling between adjacent notes when the plant signal is near a threshold.
            * @param histeresis The hysteresis value in Hz. A larger value creates a wider dead zone between notes.
    */
    void setHisteresis(float histeresis);


    /*
        * Sets the musical scale and root note for mapping the plant signal to frequencies.
            * @param root_note The root note of the scale (e.g., C, Cs, D, etc.).
            * @param scale_type The type of musical scale to use (e.g., Major, Minor, Pentatonic, etc.).
    */
    void setScale(enum Notes root_note, enum ScaleType scale_type);

    /*
        * Transposes the current scale by a specified number of semitones to match the octave.
            * @param semitones The number of semitones to transpose the scale. Positive values transpose up, negative values transpose down.
    */
    void setOctave(uint8_t octave);

    /*
        * Processes the current plant signal and returns the corresponding frequency and gate state based on the defined thresholds and scale.
            * @return A PlantState struct containing the float frequency (in Hz) and a bool gate state (true for on, false for off).
    */
    PlantState Process();

    // --- Debug & Getters ---
    float getDelta()     { return _delta; }
    float getDeltaFilt() { return _deltaFilt; }
    float getDeltaMin()  { return _deltaMin; }
    float getDeltaMax()  { return _deltaMax; }
    float getRange()     { return _range; }

private:
    // --- Hardware & Sensors ---
    daisy::DaisySeed* _hw;
    daisy::Mpr121I2C::Config _mpr121ObjConf;
    daisy::Mpr121I2C         _cap;
    const uint8_t            _touchThreshold = MPR121_TOUCH_THRESHOLD_DEFAULT;
    
    // --- DSP Filters ---
    IIR _deltaFilter;
    MF  _deltaFilterMF;

    // --- State Variables and Logic ---
    bool  _isTouched     = false; // Previous touch state
    int   _lastNoteIndex = -1;    // Last note index for hysteresis
    float _lastFreq      = 0.0f;
    
    // --- Configuration Parameters ---
    float _histeresis  = 2.0f; // Dead zone between notes
    float       _curveType   = 1.0f; // 1.0 linear, >1 exponential, <1 logarithmic
    uint8_t     _octave      = 4;
    uint8_t     _scaleLength = 7;
    float       _deltaMin    = 0.0f;
    float       _deltaMax    = 50.0f;
    float       _range       = 50.0f;
    float _delta             = 0.0f;
    float _deltaFilt         = 0.0f;

    // --- Buffers and Internal Data ---
    float _noteThresholds[13];
    float _scale[12]; 

    // --- Scale Constants (Hz at Octave 0) ---
    const float _majorScale[7]      = {16.35f, 18.35f, 20.60f, 21.83f, 24.50f, 27.50f, 30.87f}; 
    const float _minorNScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 24.50f, 25.96f, 29.14f}; 
    const float _minorAScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 24.50f, 25.96f, 30.87f}; 
    const float _minorMScale[7]     = {16.35f, 18.35f, 19.45f, 21.83f, 23.12f, 27.50f, 30.87f}; // Melodic Ascending
    const float _pentatonicScale[5] = {16.35f, 18.35f, 21.83f, 24.50f, 27.50f};                 
    const float _chromaticScale[12] = {16.35f, 17.32f, 18.35f, 19.45f, 20.60f, 21.83f, 23.12f, 24.50f, 25.96f, 27.50f, 29.14f, 30.87f}; 
    const float _esatonic[6]        = {16.35f, 18.35f, 20.60f, 21.83f, 24.50f, 27.50f};         // Custom Hexatonic
};