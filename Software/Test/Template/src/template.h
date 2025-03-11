#pragma once
#include "daisysp.h"
#include "daisy_seed.h"

/**
 * @brief Template class for PSP project
 * This template provides a basic structure for audio processing classes
 */
class Template {
public:
    Template();
    ~Template();


    void Init();                    // Initialization
    float Process(float input);    // Processing function

    void SetParameter(float value); // Set a parameter


private:
    // Private member variables (prefixed with _)
    float _parameter;            // Example parameter

};
