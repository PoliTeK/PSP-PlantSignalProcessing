# Utilizzo del sensore capacitivo MPR121 con Daisy Seed

Questo progetto mostra come collegare e utilizzare il sensore capacitivo **MPR121** per rilevare il tocco su 12 ingressi, usando la piattaforma **Daisy Seed** tramite comunicazione **I2C**.

---

## 📌 Descrizione

Il **MPR121** è un sensore capacitivo in grado di rilevare fino a 12 punti di contatto (touch). In questo esempio:
- La Daisy Seed comunica con il sensore via I2C.
- Vengono letti i canali attivati dal tocco.
- Quando uno o più canali vengono toccati, si stampano anche i valori filtrati (Filtered Data) e di riferimento (Baseline Data) sul terminale seriale.

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



---

## 🔄 Funzionamento del Codice

Il codice è diviso in tre parti principali:

### 1. **Inizializzazione**
- Si inizializza la Daisy Seed e il modulo MPR121.
- Se il sensore non viene trovato, il LED lampeggia come avviso di errore.

### 2. **Lettura del Tocco**
- Il programma verifica continuamente quali canali sono stati toccati o rilasciati rispetto al ciclo precedente.
- Viene stampato l’indice del canale toccato.

### 3. **Diagnostica (opzionale)**
- Quando si rileva almeno un tocco, vengono stampati:
  - I **valori filtrati** (FilteredData): rappresentano la lettura grezza del canale.
  - I **valori di baseline** (BaselineData): indicano il valore di riferimento senza tocco.

---

## 🖥️ Output Esempio

