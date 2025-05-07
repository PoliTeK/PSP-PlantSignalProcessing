# PentaPianta

## Overview

The goal of these files is to implement a code capable of taking the signal coming from a plant and preparing it for the daisy motor sound, obtaining the following behavior: When the plant is touched, it sounds a tuned note of a pentatonic scale. If, after a while, the plant has not been released, the plant goes into fine-tune mode, and the sounds can be regulated by sliding on the plant.

## PentaPianta Class

The `pentaPianta` class manages the decision of the frequency of each plant. Based on the value read by the sensor, it decides which frequency to output. The principal methods are:

### `discFreq`

This method associates each value of the sensor to an output frequency based on the `soglie` vector of thresholds to form a pentatonic scale. `soglie` is a pointer to a vector of 7 numbers that has to be calibrated differently for each plant:

- **`soglie[0]`**: The release threshold below which, after the sensor has been set as touched, it is flagged as released by the hardware.
- **`soglie[1]`**: The touch threshold; when it is exceeded, the sensor is set as touched.
- **`soglie[2]` to `soglie[5]`**: The different thresholds for the scale degrees.
- **`soglie[6]`**: The maximum value expected.

All the frequencies are generated based on the `f0` parameter, which is the tonic of the scale.

### `analogFreq`

This method takes the full range of the expected value and rescales it to the frequency range in a linear way.

### Constructor

The constructor requires:
- A pointer to the vector `soglie`.
- The tonic `f0`.

## PentaPianta Tester

The `pentaPianta_tester` is the tester for the `pentaPianta` class and shows how it should be used.

### Workflow

1. **Instantiation**: At the beginning, we instantiate the object.
2. **Setup**: The setup is the same as the tester function for the sensor MPR121.
3. **Main Loop**:
   - Poll for the sensor to be touched.
   - When it happens, wait some milliseconds to give the signal time to rise (e.g., 10 ms, but the value can be calibrated, keeping in mind that the result should be a sound without delay from the touch).
   - Call the `discFreq` method and wait again.
   - Poll again if the sensor is touched. If it happens, call the `analogFreq` method until the sensor is released.
