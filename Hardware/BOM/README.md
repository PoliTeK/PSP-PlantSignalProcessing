# Bill of Materials (BOM) - PlantSignalProcessing

This document lists all the necessary components to assemble the **PSP** hardware system. The provided links point to official manufacturers or major distributors (Mouser, Digi-Key, Adafruit) for easy sourcing.

## 1. Core Processing & Power
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **Daisy Seed** | STM32H7 Microcontroller (ARM Cortex-M7) | 1 | [Electro-Smith](https://www.electro-smith.com/daisy/daisy) |
| **Micro USB Cable** | For programming and power supply | 1 | - |

## 2. Sensors & Input
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **MPR121 Breakout** | 12-Channel I2C Capacitive Touch Sensor | 1 | [Adafruit](https://www.adafruit.com/product/1982) |
| **Rotary Encoder** | Incremental rotary encoder with integrated switch | 1 | [Bourns (Mouser)](https://www.mouser.it/c/?q=PEC11R) |
| **Encoder Knob** | Aluminum or plastic (6mm shaft diameter) | 1 | - |

## 3. Visualization & Output
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **OLED Display** | 128x64 I2C Display (SSD1306 or SH1106) | 1 | [Adafruit](https://www.adafruit.com/product/326) |
| **3.5mm Audio Jack** | TRS connectors for audio output (L/R) | 2 | 

## 4. Passive Components & Miscellaneous
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **XkΩ Resistors** | Hardware debounce for encoder| 2 | - |
| **X nf Capacitors** | Hardware debounce for encoder| 2 | - |
| **Electrodes / Clips** | Alligator-Banana clips for plant connection | As needed | - |

