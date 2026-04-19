
## 1. System Architecture & Limits
* **Hardware:** STM32 (Daisy Seed) running at 450 MHz.
* **Audio Engine:** 48,000 Hz sample rate, 16-sample block size.
* **Absolute Deadline:** 333 us per audio block.

## 2. Task Performance Analysis

### 2.1 Audio Callback (Hardware Interrupt)
* **Min:** 106.5 us
* **Typ (Idle):** 108 us
* **Typ (Active):** 112.8 us
* **Max:** 114.7 us
* **Analysis:** At maximum load (114.7 us), the audio generation consumes only 34% of the available 333 us time window.

### 2.2 DSP Logic & PlantConditioner (Main Loop)
* **Min:** 74.0 us
* **Typ (Idle):** 75.6 us
* **Typ (Active):** 189.5 us
* **Max:** 211.2 us
* **Analysis:** The baseline processing time is stable around 75 us when the sensor signal is flat. During plant interaction, the activation of mathematical filters on rapidly fluctuating data and the recalculation for note quantization push the maximum peak to 211.2 us. Since it is confined to the background loop, this processing load does not interfere with audio signal stability.

### 2.3 Display Update (Main Loop)
* **Min:** 43563.8 us
* **Typ:** 43671.9 us
* **Max:** 43700.8 us
* **Analysis:** Sending the frame buffer via the I2C protocol is the heaviest blocking operation in the system. It consistently absorbs about 43.7 milliseconds per cycle, regardless of user interaction. This dictates the physical limit of the GUI frame rate, capping it at approximately 22-23 FPS.

### 2.4 Encoder Polling (Main Loop)
* **Min:** 0.4 us
* **Typ:** 0.4 us
* **Max:** 0.5 us
* **Analysis:** The hardware management of the user interface (pin reading, debounce, and increment logic) has a completely negligible computational impact, taking at most half a microsecond per execution.
