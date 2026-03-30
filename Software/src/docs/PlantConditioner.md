# PlantConditioner

## Overview
PlantConditioner converte il segnale del sensore capacitivo in informazione musicale per il sintetizzatore.
L'output e' una coppia `freq/gate` usata da AudioEngine.

## Responsabilita'
- Inizializzare il sensore MPR121 e i filtri di preprocessing.
- Leggere il tocco e calcolare il delta sensore.
- Filtrare e clampare il delta in un range configurabile.
- Mappare il delta su note musicali in base a scala, root, curva e ottava.
- Gestire isteresi e sticky-note per stabilita' musicale.

## API Pubblica
| Metodo | Ruolo | Quando viene chiamato |
|---|---|---|
| `Init(IIR::FilterType, daisy::DaisySeed*)` | Setup sensore/filtro e stato iniziale | Startup |
| `setCurve(float)` | Imposta la curva di mappatura soglie | Cambio parametro UI |
| `setDelta(float)` | Imposta il massimo del range delta | Cambio parametro UI |
| `setScale(Notes, ScaleType)` | Imposta root e tipo scala | Cambio parametro UI |
| `setOctave(uint8_t)` | Trasposizione di ottava | Cambio parametro UI |
| `setHisteresis(float)` | Ampiezza dead-zone tra note adiacenti | Tuning stabilita' |
| `updateThresholds()` | Rigenera le soglie nota dal range corrente | Interno ai setter |
| `Process()` | Produce output `PlantState { _freq, _gate }` | Loop main |

## Strutture Dati Principali
| Tipo | Campo | Descrizione |
|---|---|---|
| `PlantState` | `_freq` | Frequenza in Hz da inviare al synth |
| `PlantState` | `_gate` | Stato nota ON/OFF |
| `ScaleType` | `Major`, `MinorNat`, `MinorArm`, `MinorMel`, `Pentatonic`, `Chromatic`, `Esatonic` | Scale disponibili |
| `Notes` | `C ... B` | Nota fondamentale per trasposizione scala |

## Pipeline Dati
1. Lettura touch su canale 0 di MPR121.
2. Calcolo delta: baseline - filtered - touchThreshold.
3. Filtraggio (MF + IIR) e clamp nel range `_deltaMin.._deltaMax`.
4. Verifica sticky della nota corrente con isteresi.
5. Se necessario, ricerca del nuovo intervallo nota.
6. Conversione nota in frequenza con scala/root/ottava.
7. Output `PlantState` verso il main.

## Parametri Operativi
| Parametro | Significato | Default |
|---|---|---|
| `_deltaMin` | Minimo range delta | 0.0 |
| `_deltaMax` | Massimo range delta | 50.0 |
| `_curveType` | Curvatura soglie (`1` lineare) | 1.0 |
| `_histeresis` | Dead zone tra note (%) | 2.0 |
| `_octave` | Shift ottava | 4 |

## Note di Integrazione
- `Process()` e' chiamato nel loop principale (`main`) e non in callback audio.
- Il risultato `PlantState` viene convertito in `ControlsStruct` per `AudioEngine::Update()`.
- In caso di errore init MPR121, il sistema entra in loop di LED blink.

## Riferimenti
- [AudioEngine](AudioEngine.md)
- [Main Runtime Flow](Main.md)
