# Effect Class Documentation

## Overview
The `Effect` class provides functionality for normalizing sensor values using predefined thresholds and coefficients. It offers two different normalization methods: `analogCoeffFine` and `analogCoeffIgnorant`, which allow for different levels of precision in value scaling.

## Header File: Effect.h

### Class Definition
```cpp
class Effect {
private:
    int* _thresholds;  // Pointer to an array of threshold values
    float _coeff[6]={0.0, 0.2, 0.4, 0.6, 0.8, 1}; // Coefficients for normalization

public:
    void EffectBegin(int* thresholdsI);
    uint16_t analogCoeffFine(uint16_t value);
    uint16_t analogCoeffIgnorant(uint16_t value);
};
```

## Implementation File: Effect.cpp

### Methods

#### `void Effect::EffectBegin(int* thresholdsI)`
Initializes the `_thresholds` array with the provided input values.

- **Parameters:**
  - `thresholdsI`: A pointer to an array of integers containing threshold values.
- **Behavior:**
  - Checks if the input array has the correct size (7 elements expected).
  - If the size is incorrect, an error message is printed.
  - If correct, the `_thresholds` pointer is assigned the input array.

#### `uint16_t Effect::analogCoeffFine(uint16_t value)`
Applies a segmented normalization using predefined coefficients.

- **Parameters:**
  - `value`: The input sensor value to be normalized.
- **Returns:**
  - A normalized value in the range `[0,1]`.
- **Normalization Logic:**
  - `_coeff` is a fixed coefficient array used for normalization.
  - Instead of applying a single normalization function, the function applies different slopes depending on the range the value falls into.
  - This method helps account for non-linear changes in sensor readings, allowing for better modulation.
  - If the computed output is greater than `1`, the function returns `1`.

#### `uint16_t Effect::analogCoeffIgnorant(uint16_t value)`
Applies a simple linear normalization using a fixed scale factor.

- **Parameters:**
  - `value`: The input sensor value to be normalized.
- **Returns:**
  - A normalized value in the range `[0,1]`.
- **Normalization Logic:**
  - Uses a straightforward scaling factor based on the min (`_thresholds[0]`) and max (`_thresholds[6]`) values.
  - If the computed output exceeds `1`, the function returns `1`.

## Notes
- The `analogCoeffFine` method provides more accurate normalization by applying different slopes for different ranges.
- The `analogCoeffIgnorant` method provides a simpler normalization approach without accounting for non-linear sensor variations.
- The `_thresholds` array is expected to have exactly 7 elements for proper functionality.

## Example Usage
```cpp
#include "Effect.h"

int thresholds[7] = {10, 20, 30, 40, 50, 60, 70};
Effect myEffect;
myEffect.EffectBegin(thresholds);

uint16_t normalizedValue = myEffect.analogCoeffFine(35);
std::cout << "Normalized Value: " << normalizedValue << std::endl;
```

This example initializes an `Effect` object with predefined thresholds and applies fine-grained normalization to an input value.

