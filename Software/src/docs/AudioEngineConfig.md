# AudioEngineConfig

## Overview
Questo file documenta i parametri principali definiti in `AudioEngineConfig.h` per i preset timbrici e per il blocco riverbero.

## Struttura Generale
I parametri sono organizzati per preset:
- PAD
- PLUCK
- LEAD

Ogni preset include:
- 2 oscillatori audio
- 2 LFO
- inviluppo ADSR ampiezza
- inviluppo ADSR filtro
- filtro (cutoff/resonance)

## Preset PAD
| Categoria | Parametri principali | Valori default |
|---|---|---|
| Oscillatori | `pad_waveform1`, `pad_waveform2`, `pad_amp1`, `pad_amp2`, `pad_shape1`, `pad_shape2` | SAW + POLYBLEP_SAW, amp 1.0, shape 0.5 |
| LFO | `lfo_pad_freq1`, `lfo_pad_freq2`, `lfo_pad_Amp1`, `lfo_pad_Amp2` | 0.2 Hz, 0.8 Hz, amp 0.005 |
| Env Amp | `pad_attack`, `pad_decay`, `pad_sustain`, `pad_release` | 2.8, 1.0, 0.8, 1.2 |
| Env Filtro | `pad_fattack`, `pad_fdecay`, `pad_fsustain`, `pad_frelease` | 2.8, 1.8, 0.4, 2.0 |
| Filtro | `pad_cutoff_freq`, `pad_res` | 1000 Hz, 0.4 |

## Preset PLUCK
| Categoria | Parametri principali | Valori default |
|---|---|---|
| Oscillatori | `pluck_waveform1`, `pluck_waveform2`, `pluck_amp1`, `pluck_amp2`, `pluck_shape1`, `pluck_shape2` | TRI + TRI, amp 1.0, shape 0.5 |
| LFO | `lfo_pluck_freq1`, `lfo_pluck_freq2`, `lfo_pluck_Amp1`, `lfo_pluck_Amp2` | 7.0 Hz, 0.7 Hz, amp 0.05/0.005 |
| Env Amp | `pluck_attack`, `pluck_decay`, `pluck_sustain`, `pluck_release` | 0.01, 0.15, 0.0, 0.8 |
| Env Filtro | `pluck_fattack`, `pluck_fdecay`, `pluck_fsustain`, `pluck_frelease` | 0.1, 1.8, 0.4, 2.0 |
| Filtro | `pluck_cutoff_freq`, `pluck_res` | 8000 Hz, 0.4 |

## Preset LEAD
| Categoria | Parametri principali | Valori default |
|---|---|---|
| Oscillatori | `lead_waveform1`, `lead_waveform2`, `lead_amp1`, `lead_amp2`, `lead_shape1`, `lead_shape2` | POLYBLEP_SQUARE + POLYBLEP_SQUARE, amp 1.0, shape 0.1 |
| LFO | `lfo_lead_freq1`, `lfo_lead_freq2`, `lfo_lead_Amp1`, `lfo_lead_Amp2` | 0.7 Hz, 0.7 Hz, amp 0.003/0.005 |
| Env Amp | `lead_attack`, `lead_decay`, `lead_sustain`, `lead_release` | 0.05, 2.8, 0.6, 1.3 |
| Env Filtro | `lead_fattack`, `lead_fdecay`, `lead_fsustain`, `lead_frelease`, `lead_fAmp` | 1.0, 0.8, 0.6, 0.8, 2.0 |
| Filtro | `lead_cutoff_freq`, `lead_res` | 1000 Hz, 0.4 |

## Riverbero Globale
| Parametro | Descrizione | Valore default |
|---|---|---|
| `revLpFreq` | Low-pass interno del riverbero | 18000 Hz |
| `revFeedback` | Feedback del riverbero | 0.85 |
| `revDryWet` | Mix segnale dry/wet | 0.3 |

## Note Operative
- I parametri sono globali e applicati in `AudioEngine::SetPreset()`.
- Dopo ogni modifica dei valori in `AudioEngineConfig.h`, e' necessario ricompilare.
- Le unita' temporali ADSR sono in secondi (come usato da DaisySP Adsr).
