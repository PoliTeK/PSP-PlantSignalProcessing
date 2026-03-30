# MenuManager

## Overview
MenuManager implementa la logica di navigazione menu tramite encoder (click + rotazione) e pubblica una struttura dati unica con stato UI e parametri correnti.

## Responsabilita'
- Gestire la macchina a stati della UI (play, hub, pagine parametro).
- Aggiornare i parametri editabili (`delta`, `curve`, `root`, `scale`, `octave`, `preset`).
- Gestire il cursore di navigazione nei menu hub.
- Esporre dati coerenti al loop principale tramite `GetData()`.

## API Pubblica
| Metodo | Ruolo | Quando viene chiamato |
|---|---|---|
| `Init()` | Inizializza stato menu e valori default | Startup |
| `StateTransition(bool click, int rotation, bool timeout)` | Aggiorna stato/cursore/parametri con input encoder e timeout | Loop principale |
| `GetData()` | Restituisce snapshot `MenuData` | Loop principale |

## Strutture Dati Principali
| Tipo | Campo | Descrizione |
|---|---|---|
| `MenuData` | `state` | Stato UI corrente |
| `MenuData` | `cursor_state` | Prossimo stato selezionato dal cursore |
| `MenuData` | `delta`, `curve` | Parametri calibrazione sensore |
| `MenuData` | `root`, `scale`, `octave` | Parametri musicali per PlantConditioner |
| `MenuData` | `preset` | Preset synth per AudioEngine |

## Stati UI
| Stato | Ruolo |
|---|---|
| `PLAYMODE` | Visualizzazione waveform |
| `MAIN_MENU` | Hub principale |
| `CALIBRATION_HUB` | Accesso a `DELTA` e `CURVE` |
| `SCALES_HUB` | Accesso a `ROOT`, `SCALE`, `OCTAVE` |
| `PRESETS_HUB` | Selezione preset synth |
| `DELTA`, `CURVE`, `ROOT`, `SCALE`, `OCTAVE` | Pagine foglia di editing |
| `BACK` | Navigazione verso livello superiore |

## Logica di Transizione (Sintesi)
1. Timeout forza ritorno a `PLAYMODE`.
2. Click conferma `_next_state` e aggiorna il nuovo contesto.
3. Rotazione modifica parametro (se pagina foglia) oppure sposta il cursore nei menu hub.
4. `cursor_state` viene allineato a `_next_state` per il rendering della freccia nei menu.

## Range Parametri (Implementazione Corrente)
| Parametro | Range |
|---|---|
| `delta` | 1.0 .. 100.0 |
| `curve` | 0.1 .. 2.0 |
| `root` | 0 .. 11 |
| `scale` | 0 .. 6 |
| `octave` | 1 .. 8 |
| `preset` | 0 .. 2 |

## Nota di Coerenza
I range in tabella sono allineati alle enum usate dai moduli DSP:
- `ScaleType` in PlantConditioner: 0..6
- `SynthPreset` in AudioEngine: 0..2

## Note di Integrazione
- `GetData()` alimenta il loop principale in `main.cpp`.
- `main` inoltra i valori verso PlantConditioner, AudioEngine e DisplayHandler.
- La visualizzazione del cursore usa `state` e `cursor_state` per decidere quale voce evidenziare.

## Riferimenti
- [Main Runtime Flow](Main.md)
- [PlantConditioner](PlantConditioner.md)
- [AudioEngine](AudioEngine.md)
- [DisplayHandler](DisplayHandler.md)
