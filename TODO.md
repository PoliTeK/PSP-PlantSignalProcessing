## TODO
1. Src/Display/DisplayManager  @Matteo
-  Classe per gestire tutte le funzioni del display. Deve esporre dei metodi per disegnare     linee, scrivere testi ed elementi grafici ricevendo testi e posizioni in pixel, in output scrive sul display.
2. MenuManager        @Matteo
- Macchina a stati che gestisce il Menu Diving. All'interno ha Display manager e lo gestisce. Riceve dall'encoder (attraverso il main)
--- MenùDiving.
PLAYMODE(Default)
├─Calibration
|  ├─CurveType
|  ├─ Delta
|  ├─Back
├─Scale
|  ├─Root
|  | ├─...
|  |
|  ├─Scale
|  | ├─...
|  ├─Back
|
├─Sound
  ├─Pluck
  ├─Lead
  ├─Bass
  ├─Back
'''
3. src/PSP/PlantConditioner V Audio Engine                  @Leo
PLantConditioner: Ha all'interno l'MPR121 e da lui riceve info su toccata e (fuga) valore. Riceve info su ROOT, SCALE, Curve type e Delta. Manda veso l'esterno is_touched e Target Frequency
4. src/Components              @Angelo   
5. main                        @Angelo
6. HARDWARE                    @Marcello
7. README                      @Marcello