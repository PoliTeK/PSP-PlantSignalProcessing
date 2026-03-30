# Main Runtime Flow

## Overview
`main.cpp` coordina inizializzazione hardware, logica UI, conditioning del sensore, sintesi audio e aggiornamento display.

## Responsabilita' del Main
- Setup board Daisy, encoder, display e menu.
- Init di PlantConditioner e AudioEngine.
- Avvio callback audio.
- Loop infinito per aggiornare i parametri di controllo.
- Sync periodico del rendering display.

## Inizializzazione (Startup)
Ordine attuale:
1. `hw.Init()`
2. `enc.Init(...)` e `menu.Init()`
3. Init display e stato viewer
4. `pc.Init(...)`
5. `synth.Init(hw.AudioSampleRate())`
6. `synth.SetPreset(PRESET_LEAD)`
7. `hw.StartAudio(AudioCallback)`

## Audio Callback
La callback opera ad audio-rate:
1. `synth.Update(audio_controls)`
2. Per ogni sample del buffer:
   - `sig = synth.Process()`
   - push sample al display handler
   - scrittura su uscita stereo L/R

## Loop Principale (Control-rate)
Nel `while(1)` vengono eseguiti:
1. Poll encoder e transizioni stato menu.
2. Lettura `ui_data` dal menu.
3. Sync parametri UI verso DSP:
   - PlantConditioner: delta, curve, octave, scale/root
   - AudioEngine: preset
4. `plant_data = pc.Process()`.
5. Update di `audio_controls` con `plant_data._freq` e `plant_data._gate`.
6. Aggiornamento display ogni ~60 cicli.

## Flusso Integrato tra Moduli
1. `MenuManager` produce i parametri utente (`preset`, `scala`, `ottava`, `curve`, `delta`).
2. `PlantConditioner` converte i dati sensore in `PlantState` (`_freq`, `_gate`).
3. Il main copia `PlantState` in `audio_controls` (`freq`, `gate`).
4. `AudioCallback(...)` chiama `synth.Update(audio_controls)`.
5. `synth.Process()` genera il sample finale, inviato a DAC e display waveform.

## Control Rate vs Audio Rate
| Livello | Dove avviene | Funzione |
|---|---|---|
| Control-rate | Loop `while(1)` in `main.cpp` | Aggiorna parametri UI e calcola nuova `freq/gate` |
| Audio-rate | `AudioCallback(...)` | Genera campioni in tempo reale |

## Dati di Collegamento
| Dato | Producer | Consumer |
|---|---|---|
| `ui_data` | MenuManager | PlantConditioner, AudioEngine, Display |
| `plant_data._freq` | PlantConditioner | AudioEngine (via `audio_controls`) |
| `plant_data._gate` | PlantConditioner | AudioEngine (via `audio_controls`) |
| `sig` | AudioEngine | DAC output + waveform display |

## Note Operative
- Il preset viene applicato in modo continuo nel loop tramite `synth.SetPreset(...)`.
- `audio_controls` e' una struttura globale condivisa tra loop principale e callback audio.
- Il display update e' bloccante (I2C), con reset timestamp encoder per evitare salti.

## Dipendenze Chiave
- `AudioEngine` dipende da DaisySP per oscillatori, ADSR, filtro e riverbero.
- `AudioEngine` dipende da `AudioEngineConfig.h` per i valori preset.
- La logica di mapping sensori/musica e' incapsulata in `PlantConditioner`.

## Nota per Estensioni Future
Quando si aggiunge una nuova classe DSP, documentare sempre:
1. input attesi,
2. output prodotti,
3. frequenza di chiamata (control-rate/audio-rate),
4. punto preciso di integrazione nel `main` o nella callback audio.

## Riferimenti
- [PlantConditioner](PlantConditioner.md)
- [AudioEngine](AudioEngine.md)
- [MenuManager](MenuManager.md)
- [DisplayHandler](DisplayHandler.md)
