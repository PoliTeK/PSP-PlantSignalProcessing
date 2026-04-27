# Bill of Materials (BOM) - PlantSignalProcessing

This document lists all the necessary components to assemble the **PSP** hardware system. The provided links point to official manufacturers or major distributors (Mouser, Digi-Key, Adafruit) for easy sourcing.

## 1. Core Processing & Power
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **Daisy Seed** | STM32H7 Microcontroller (ARM Cortex-M7) | 1 | [Electro-Smith](https://www.electro-smith.com/daisy/daisy) |
| **Micro USB Cable** | For programming and power supply | 1 | - |
| **Battery** | LiPo battery 3.7 V | 1 | [RS](https://it.rs-online.com/web/p/batterie-a-dimensioni-speciali-ricaricabili/1251266?searchId=45a1f1ae-5010-4535-9a6d-b827e8f4dcab) |

## 2. Sensors & Input
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **MPR121 Breakout** | 12-Channel I2C Capacitive Touch Sensor | 1 | [Adafruit](https://www.adafruit.com/product/1982) |
| **Rotary Encoder** | Incremental rotary encoder with integrated switch | 1 | [Bourns (Mouser)](https://www.mouser.it/ProductDetail/Bourns/PEC11R-4225F-S0024?qs=Zq5ylnUbLm4N6UNkPX3pkQ%3D%3D&srsltid=AfmBOoozIBXmp32bZYOc8GeRMGToWd_c4FxWYIGp9he8cdbeGzlQ_t11) |
| **Encoder Knob** | Aluminum or plastic (6mm shaft diameter) | 1 | - |
| **BMS** | Battery managment system. To reload the battery | 1 | [Digekey](https://www.digikey.it/it/products/detail/adafruit-industries-llc/2465/5356834?s=N4IgTCBcDaIIwFYwA4C0cDMAWAnKgcgCIgC6AvkA)

## 3. Visualization & Output
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **OLED Display** | 128x64 I2C Display (SSD1306 or SH1106) | 1 | [Adafruit](https://www.amazon.com/UCTRONICS-SSD1306-Self-Luminous-Display-Raspberry/dp/B072Q2X2LL/ref=sr_1_4?dib=eyJ2IjoiMSJ9.IJoxWF1pbbgjuFzVxunatn7JCczOu5ofMc__4Jqm73W75u7ByvPtkE58QeL05WuARik1eSz9AF9HWZWLfmOZAtgTpvNMI6-KiS-KY7Tyb4ubJwjiBcI57JND8FcWICoqp362CxXLvoX48CgG-XVvV9QskaN_sv9b6eM01rLfN6MPswHD4HRh7pFS8QI6yAuw8iP4hYEXCZRZ9PpPhVVuimwEVWNWTi5OQguqkpZSVEs.qwOIDMJkxyEZ69XAv2VrIImZ9yyNK8qAs1XRnhSwb7U&dib_tag=se&keywords=oled+128x64&qid=1777300837&sr=8-4) |
| **3.5mm Audio Jack** | TRS connectors for audio output (L/R) | 2 | [DigiKey](https://www.digikey.it/it/products/detail/kycon-inc/STX-3000/9975995?s=N4IgTCBcDa4AwE4wFoDKAVAGsgzHfyAcgCIgC6AvkA)

## 4. Passive Components & Miscellaneous
| Component | Description | Quantity | Link |
| :--- | :--- | :---: | :--- |
| **10 kΩ Resistors** | Hardware debounce for encoder| 2 | - |
| **10 nf Capacitors** | Hardware debounce for encoder| 2 | - |
| **Red LED** | Low nattery indicator | 1 | - |
| **330 Ω Resistor ** | To use with the LED | 1 | - |
| **Electrodes / Clips** | Alligator-Banana clips for plant connection | As needed | - |
| **On/off switch** | Switch tu turn off(on the system | 1 | [Mouser](https://www.mouser.it/ProductDetail/Same-Sky/SLW-1411565-9A-RA-S-D?qs=IKkN%2F947nfBIWjWV6WesVw%3D%3D&srsltid=AfmBOoq23Me8njhMRF-y-1wJ2FztqANlgy0GX4Rwlt2JTs-EZvvkz7Lh)
