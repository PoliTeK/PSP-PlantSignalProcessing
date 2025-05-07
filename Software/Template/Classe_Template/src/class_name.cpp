#include "template.h"

Template::Template()
{
    // Initialize member variables with default values
    _parameter = 0.0f;
}

Template::~Template()
{
    // Cleanup if needed
}

void Template::Init() // initialization
{

}

float Template::Process(float input)
{
    // Example processing
    float output = input;
    
    return output;
}

void Template::SetParameter(float value)
{
    _parameter = value;
}