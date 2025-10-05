/// @file startup.h 
/// @brief header file for startup utility functions

#ifndef STARTUP_H
#define STARTUP_H

#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include <cstdint>

// if true, logs status of startup to serial console
// a define is used to avoid compiling useless string
// that occupy memory
#define LOGGING 

/// @brief Reads from the sensor some values and estimate the maximum
/// @param maxDestination where to store the maximum value
/// @param cap_sensor reference to object of cap sensor
/// @param hw hardware object to make usage of adc reading and serial
/// @returns Error value if is < 0
/// @todo understand why cannot pass *hw to function
int startup_getMaxValue (
    float* maxDestination,
    daisy::Mpr121<daisy::Mpr121I2CTransport>* cap_sensor,
    daisy::DaisySeed& hw
);

#endif //STARTUP_H
