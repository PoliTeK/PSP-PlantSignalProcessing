#pragma once  // Direttiva che assicura che questo header file sia incluso una sola volta

// Inclusione delle librerie necessarie per il progetto Daisy
// #include "DaisyDuino.h" NON SERVE IN VSCODE

#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"  
#include <stdint.h>

// Definizione della costante per il tempo massimo di delay in secondi
#define MAX_DELAY_TIME 1 

// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;     // Namespace per le funzionalità Daisy
using namespace daisysp;   // Namespace per le funzionalità DSP

// Definizione della classe AnalogDelay
class AnalogDelay {
public:
    AnalogDelay();        // Costruttore della classe
    ~AnalogDelay();       // Distruttore della classe

    // Metodi pubblici della classe
    void Init(float sampleRate);              // Inizializza il delay con il sample rate
    float Process(float buffer_in);           // Processa il segnale in ingresso
    void setFeedback(float feedback);         // Imposta il feedback del delay
    void setMix(float mix);                   // Imposta il mix tra segnale dry/wet
    void setDelayTime(float delayTime);       // Imposta il tempo di delay
    void setDepth(float depth);               // Imposta la profondità del pitch shifter
    void setSmoothFactor(float smooth_factor); // Imposta il fattore di smoothing per il tempo di delay
    // Valori più bassi producono uno smoothing più lento

private:
    static const size_t _delayBufferSize = 48000 * MAX_DELAY_TIME;

    // Variabili membro private
    float _sampleRate;        // Frequenza di campionamento
    float _delayTime;         // Tempo di delay
    float _feedback;          // Quantità di feedback
    float _mix;              // Rapporto tra segnale originale e processato
    float _depth;            // Profondità del pitch shifter
    float _smooth_factor;    // Fattore di smoothing per il tempo di delay

    uint32_t _writePos;      // Posizione di scrittura nel buffer di delay
    float _readPos;          // Posizione di lettura nel buffer di delay
    float* _delayBuffer;     // Buffer di delay allocato in SDRAM

    // Oggetti per il processing del segnale
    CrossFade _crossfade;     // Oggetto per il crossfade tra segnale dry e wet
    PitchShifter _ps;          // Oggetto per lo shift di pitch
    Oscillator   _lfo;         // LFO per il pitch shifter
};
