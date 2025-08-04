# Utilizzo del sensore capacitivo MPR121 con Daisy Seed

Questo progetto mostra come collegare e utilizzare il sensore capacitivo **MPR121** per rilevare il tocco su un ingresso (0), usando la piattaforma **Daisy Seed** tramite comunicazione **I2C**.

---
## ⚙️ Collegamenti Hardware

Collegare il modulo **MPR121** alla **Daisy Seed** come segue:

| Daisy Seed | MPR121 |
|------------|--------|
| **3.3V**   | VDD    |
| **GND**    | GND    |
| **Pin 13** | SDA    |
| **Pin 12** | SCL    |

> ⚠️ **Nota**: La Daisy Seed lavora a 3.3V — non fornire 5V al sensore.

## 📌 La Daisy Seed Monitora il canale 0 del sensore, in particolare:
   - stampa i valori di BaselineData, FilteredData e la loro differenza.
   - Rileva il tocco e continua a stampare i valori di  BaselineData, FilteredData e la loro differenza.
   - Rileva il release del canale
---
## Funzionamento logico sensore:
   Il sensore calcola un valore "Medio" di capacità (BaselineData) ed un valore istantaneo filtrato (FilteredData). 
   Se la loro differenza supera la una treshold impostabile (da aggiustare il fissaggio delle soglie) un tocco viene rilevato e il comando cap.Touched() da in out il codice binario hot one corrispondente al pad toccato (100000000000 nel nostro caso)
   c'è una treshold per l'attack ed una per il release (ISTERESI)
---


## 🖥️ Output Esempio

Baseline Untouched Value : 120
Filtered Untouched Value : 120
Difference Untouched Value : 0




Baseline Untouched Value : 116
Filtered Untouched Value : 116
Difference Untouched Value : 0


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


# OSSERVAZIONI

Per gestire l'audio è utile utilizzare la differenza in quanto sufficientemente stabile e indipendente dalle variazioni assolute dovute al cambio dell'ambiente

Per inizializzare il sensore e tararlo si può pensare di avere una funzione di "Taratura" che per tot cicli va a vedere la differenza (negativa se non tocchi) , nel mentre tu "leggi la sfera di cristallo" ed infine la treshold al tocco è il vlore massimo raggiunto  +1