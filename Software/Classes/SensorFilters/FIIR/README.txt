# 🧰 CapFir – FIR Filter per Segnali Capacitivi

`CapFir` è una classe C++ per implementare un **filtro FIR** (Finite Impulse Response) a risoluzione variabile, pensato per stabilizzare la lettura dei segnali da sensori capacitivi (es. MPR121).

--- Codice Matlab calcola i coefficienti in base all'ordine del filtro, frequenza di campionamento fs e frequenza di taglio ft

## 🔧 Funzionalità

- Tre modalità di risoluzione: `LOW` (16 tap), `MID` (32 tap), `HIGH` (64 tap)
- Coefficienti precalcolati per filtro passa basso
- Adatto per filtrare letture noisy e ottenere segnali stabili per audio interattivo
- fc = 1kHz (hw.DelayMs(1))
- ft = 5Hz (Matlab)

---

## 🚀 Utilizzo

### 1. Includi la classe

```cpp
#include "CapFir.h"
