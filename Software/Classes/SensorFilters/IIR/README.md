IIR Sensor Filter
Classe C++ per filtro IIR del 2° ordine (Butterworth / Bessel), ottimizzata per smoothing di sensori.

⚠️ Vincoli Critici
Timing Rigido (100 Hz): Il metodo Process() DEVE essere eseguito esattamente ogni 10 ms. Tempi diversi alterano la frequenza di taglio.

Configurazione Statica: I coefficienti sono hardcoded nell'header.

Non è possibile cambiare Frequenza di Taglio (Fc) o Ordine a runtime.

Per modificare i parametri, è necessario ricalcolare i coefficienti esternamente e aggiornare iir.h.