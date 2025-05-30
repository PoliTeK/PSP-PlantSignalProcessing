#pragma once  // Direttiva che assicura che questo header file sia incluso una sola volta

// Inclusione delle librerie necessarie per il progetto Daisy
#include "daisy_seed.h"    // Include le funzionalità base della scheda Daisy
#include "daisysp.h"       // Include le funzionalità DSP di Daisy

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
    // Variabili membro private
    float _sampleRate;        // Frequenza di campionamento
    float _delayTime;         // Tempo di delay
    float _feedback;          // Quantità di feedback
    float _mix;              // Rapporto tra segnale originale e processato
    float _depth;            // Profondità del pitch shifter
    float _smooth_factor;    // Fattore di smoothing per il tempo di delay

    
    // Oggetti per il processing del segnale
    DelayLine<float, MAX_DELAY_TIME * 48000> _delayLine; // Linea di delay (48000 samples per secondo)
    CrossFade _crossfade;     // Oggetto per il crossfade tra segnale dry e wet
    PitchShifter _ps;          // Oggetto per lo shift di pitch
    Oscillator   _lfo; // LFO per il pitch shifter
};
