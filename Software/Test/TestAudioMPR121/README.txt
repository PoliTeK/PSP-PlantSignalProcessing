Cartella contenente test di componenti ecc, NOMINARE OGNI TEST : "Test_....."## ⚙️ Collegamenti Hardware

Collegare il modulo **MPR121** alla **Daisy Seed** come segue:

| Daisy Seed | MPR121 |
|------------|--------|
| **3.3V**   | VDD    |
| **GND**    | GND    |
| **Pin 13** | SDA    |
| **Pin 12** | SCL    |

> ⚠️ **Nota**: Non alimentare il sensore a 5V. La Daisy Seed funziona a 3.3V.

---

## 🧠 Funzionamento

Il codice:

- Inizializza un **oscillatore audio** (forma d’onda: triangolare) con frequenza base 440 Hz.
- Monitora **il canale 0** del sensore capacitivo.
- Calcola la differenza tra il valore **Baseline** e il valore **Filtered** del sensore (Δ).
- Quando viene rilevato un tocco:
  - La frequenza dell’oscillatore viene **modificata dinamicamente** in base a Δ.
  - Il suono viene riprodotto finché il tocco è presente.
- Quando il dito viene rimosso, l'audio si interrompe.

---

## 🔁 Logica del Tocco

Il sensore MPR121 fornisce due valori per ogni canale:
- `BaselineData`: valore medio di riferimento.
- `FilteredData`: valore istantaneo filtrato.

Il **delta** viene calcolato come:

```cpp
delta = FilteredData(0) - BaselineData(0) - touchThreshold;

Se il canale 0 è toccato, la variabile gate viene attivata, generando audio.

La frequenza dell’oscillatore è controllata dinamicamente:

```cpp
f = 440 + (delta / 40) * 440;

🖥️ Output di Debug (con #define DEBUG)
Quando è attiva la modalità debug, sulla seriale viene stampato:

--------------------------------------------------------------------------------
CAP 0 touched
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 109 |
| Difference Touched Value : 7 |
|
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 108 |
| Difference Touched Value : 8 |
|
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 107 |
| Difference Touched Value : 9 |
|
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 107 |
| Difference Touched Value : 9 |
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 107 |
| Difference Touched Value : 9 |
|
|
| Baseline Touched Value : 116 |
| Filtered Touched Value : 108 |
| Difference Touched Value : 8 |
|
CAP 0 released
--------------------------------------------------------------------------------



Baseline Untouched Value : 116
Filtered Untouched Value : 120
Difference Untouched Value : -4




Baseline Untouched Value : 116
Filtered Untouched Value : 119
Difference Untouched Value : -3

# Next Steps

- Trovare modo per impostare le soglie dinamicamente
- Algoritmo di controllo delle soglie per rendere lk'esperienza sempre uguale
- Rendere stabile la lettura per una generazione di toni più lineare e stabile
- introdurre controlli sul suono
-
