# DisplayHandler

## Overview
DisplayHandler gestisce il rendering su OLED SSD130x e funge da macchina a stati grafica.
Supporta visualizzazione waveform in realtime e schermate menu/parametri.

## Responsabilita'
- Astrarre l'accesso al display tramite `MyOledDisplay`.
- Gestire stato grafico (`STANDBY`, `WAVEFORM_VIEWER`, `MENU_MODE`).
- Ricevere campioni audio e mantenerli in circular buffer.
- Disegnare la forma d'onda con trigger opzionale per stabilita' visiva.
- Disegnare menu e schermate di editing parametri.

## API Pubblica
| Metodo | Ruolo | Quando viene chiamato |
|---|---|---|
| `Update()` | Esegue rendering in base allo stato corrente e aggiorna display hardware | Loop principale |
| `SetState(DisplayState)` | Cambia stato grafico e pulisce lo schermo | Cambio modalita' UI |
| `SetYscale(int)` | Scala verticale waveform | Setup / tuning visuale |
| `SetStandbyText(const char*)` | Testo standby personalizzato | Setup UI |
| `pushAudioSample(float)` | Inserisce campione nel buffer waveform | Audio callback |
| `DrawMainMenu(int)` | Disegna menu principale con cursore | MENU_MODE |
| `DrawCalibrationHub(int)` | Disegna hub calibrazione | MENU_MODE |
| `DrawScalesHub(int)` | Disegna hub scale | MENU_MODE |
| `DrawFloatParameter(const char*, float)` | Disegna editor parametro float | MENU_MODE |
| `DrawIntParameter(const char*, int)` | Disegna editor parametro intero/enum | MENU_MODE |

## Strutture e Stati
| Elemento | Descrizione |
|---|---|
| `DisplayState::STANDBY` | Schermata statica con testo |
| `DisplayState::WAVEFORM_VIEWER` | Disegno automatico waveform |
| `DisplayState::MENU_MODE` | Disegno controllato da chiamate esplicite menu |
| `_circBuffer_ptr[256]` | Buffer circolare campioni audio |
| `_windowSize` | Finestra visualizzazione (128 sample) |
| `_writeHead` | Indice di scrittura nel buffer |

## Pipeline Waveform
1. `pushAudioSample()` scrive il sample nel buffer circolare.
2. `Update()` richiama `drawWaveForm()` in stato viewer.
3. `findTrigger()` cerca crossing negativo->positivo per allineare il disegno.
4. Viene disegnata una linea per ogni coppia di sample consecutivi nella finestra.
5. Il frame viene inviato al display con `_displayPtr->Update()`.

## Parametri Visuali
| Parametro | Significato | Default |
|---|---|---|
| `BUFFER_SIZE` | Dimensione buffer campioni | 256 |
| `WINDOW_SIZE` | Numero sample mostrati a schermo | 128 |
| `_yscale` | Amplificazione verticale waveform | 1 (in main viene impostato a 100) |
| `_triggerEnabled` | Ricerca trigger per stabilita' | true |

## Note di Integrazione
- I campioni arrivano dalla callback audio tramite `pushAudioSample(sig)`.
- L'aggiornamento display avviene nel loop principale e non in callback audio.
- In `MENU_MODE` la funzione `Update()` non disegna contenuti automatici: il chiamante deve invocare esplicitamente le funzioni `Draw...`.

## Riferimenti
- [Main Runtime Flow](Main.md)
- [MenuManager](MenuManager.md)
