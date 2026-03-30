# AudioEngine

## Overview
`AudioEngine` e' il motore di sintesi principale del progetto.
Riceve controlli musicali (`freq`, `gate`) e genera un campione audio per volta usando:
- 2 oscillatori audio
- 2 LFO di modulazione
- 2 inviluppi ADSR (ampiezza e filtro)
- filtro ladder
- riverbero finale

## Responsabilita'
- Inizializzare i moduli DSP con il sample rate della board.
- Caricare i preset timbrici (`PAD`, `PLUCK`, `LEAD`).
- Aggiornare i controlli provenienti dalla logica di alto livello.
- Produrre il sample audio in tempo reale.

## API Pubblica
| Metodo | Ruolo | Quando viene chiamato |
|---|---|---|
| `Init(float sample_rate)` | Inizializza tutti i moduli DSP e carica preset di default | Startup |
| `SetPreset(SynthPreset preset)` | Applica i parametri preset a oscillatori/LFO/ADSR/filtro/reverb | Cambio preset da UI |
| `Update(ControlsStruct controls)` | Aggiorna frequenza/gate e gestisce retrigger inviluppi su fronte di salita | Callback audio |
| `Process()` | Genera e restituisce un singolo campione audio | Ogni sample audio |

## Strutture Dati Principali
| Tipo | Campo | Descrizione |
|---|---|---|
| `ControlsStruct` | `freq` | Frequenza target in Hz |
| `ControlsStruct` | `gate` | Stato nota ON/OFF |
| `Oscillator_str` | `Waveform`, `Amp`, `Shape`, `Freq` | Parametri oscillatore/LFO |
| `Adsr_str` | `Attack`, `Decay`, `Sustain`, `Release`, `Amp` | Parametri inviluppo |
| `Filter_str` | `Cutoff`, `Resonance` | Parametri filtro |

## Preset Supportati
L'enum `SynthPreset` supporta:
- `PRESET_PAD`
- `PRESET_PLUCK`
- `PRESET_LEAD`

I valori dei parametri sono definiti in [AudioEngineConfig.md](AudioEngineConfig.md).

## Pipeline di Elaborazione (per sample)
1. Modulazione di frequenza dei due oscillatori con i due LFO.
2. Elaborazione inviluppo ampiezza e inviluppo filtro.
3. Mix oscillatori (media dei due segnali).
4. Applicazione inviluppo di ampiezza.
5. Filtro ladder con cutoff modulata dall'inviluppo filtro.
6. Riverbero e mix dry/wet.
7. Ritorno del sample finale.

## Note di Integrazione
- `Update()` e `Process()` sono usati nella callback audio.
- `SetPreset()` viene invocato dal loop principale in base al valore selezionato a menu.
- La conversione sensori -> `freq/gate` avviene in `PlantConditioner`.

Dettagli sul flusso tra moduli: [Main Runtime Flow](Main.md).

## Template Rapido per le Prossime Classi
Per mantenere coerenza, usa queste sezioni in ogni nuovo file documentazione classe:
1. Overview
2. Responsabilita'
3. API Pubblica
4. Strutture Dati / Parametri
5. Pipeline o Data Flow
6. Note di Integrazione
