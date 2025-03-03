#pragma once  // Direttiva che assicura che questo header file venga incluso una sola volta

// Inclusione degli header necessari per il funzionamento con Daisy
#include "daisy_seed.h"    // Include il core della piattaforma Daisy
#include "daisysp.h"       // Include la libreria DSP di Daisy

#define MAX_DELAY_TIME 1 // Tempo massimo di delay in secondi

// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;     // Namespace per le funzionalità core di Daisy
using namespace daisysp;   // Namespace per le funzionalità DSP

// Definizione della classe AnalogDelay
class AnalogDelay {
public:
    AnalogDelay();         // Costruttore della classe
    ~AnalogDelay();        // Distruttore della classe

    // Metodi pubblici
    void Init(float sampleRate);              // Inizializza il delay con il sample rate specificato
    float Process(float buffer_in);           // Processa un singolo campione audio
    void setFeedback(float feedback);         // Imposta il valore di feedback del delay
    void setMix(float mix);                   // Imposta il mix tra segnale dry e wet
    void setDelayTime(float delayTime);       // Imposta il tempo di delay

private:
    // Costanti
    static const size_t _delayBufferSize = 48000 * MAX_DELAY_TIME;  // Dimensione del buffer di delay (1 secondo a 48kHz)
    
    // Variabili membro private
    float _sampleRate;         // Frequenza di campionamento
    float _delayTime;          // Tempo di delay desiderato
    float _feedback;           // Quantità di feedback del delay
    float _mix;                // Bilanciamento tra segnale dry e wet
    uint32_t _writePos;        // Posizione di scrittura nel buffer
    float _readPos;            // Posizione di lettura nel buffer (float per interpolazione)
    float _currentDelayTime;   // Tempo di delay corrente durante lo smoothing
    float _smoothingFactor;    // Fattore per lo smoothing del tempo di delay
    float* _delayBuffer;       // Puntatore al buffer circolare del delay
};
