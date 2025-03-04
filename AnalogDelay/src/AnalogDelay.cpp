#include "AnalogDelay.h" // Include l'header file della classe AnalogDelay

// Costruttore: inizializza tutti i parametri dell'effetto delay con valori predefiniti
AnalogDelay::AnalogDelay() : 
    _sampleRate(48000.0f),     // Frequenza di campionamento standard (48kHz)
    _delayTime(0.5f),          // Tempo di delay iniziale (500ms)
    _feedback(0.4f),           // Quantità di feedback iniziale (40%)
    _mix(0.5f),               // Mix tra segnale dry/wet (50%)
    _currentDelayTime(0.5f),   // Tempo di delay corrente per smoothing
    _smoothingFactor(0.00005f) {} // Fattore di smoothing per cambiamenti graduali

AnalogDelay::~AnalogDelay() {} // Distruttore (vuoto)

// Inizializza l'effetto con la frequenza di campionamento specificata
void AnalogDelay::Init(float sampleRate) {
    _sampleRate = sampleRate;    // Imposta la frequenza di campionamento
    _delayLine.Init();           // Inizializza la linea di ritardo
    _crossfade.Init();           // Inizializza il modulo di crossfade
}

// Imposta il livello di feedback con limit tra 0 e 0.97
void AnalogDelay::setFeedback(float feedback) {
    _feedback = fclamp(feedback, 0.0f, 0.97f); // Limita il feedback per evitare auto-oscillazioni
}

// Imposta il mix tra segnale dry e wet (0 = solo dry, 1 = solo wet)
void AnalogDelay::setMix(float mix) {
    _mix = fclamp(mix, 0.0f, 1.0f);  // Limita il mix tra 0 e 1
    _crossfade.SetPos(_mix);          // Aggiorna la posizione del crossfader
}

// Imposta il tempo di delay in secondi
void AnalogDelay::setDelayTime(float delayTime) {
    _delayTime = fclamp(delayTime, 0.02f, (float)MAX_DELAY_TIME); // Limita il tempo di delay
}

// Processa un singolo campione audio
float AnalogDelay::Process(float buffer_in) {
    // Calcola gradualmente il nuovo tempo di delay per evitare artefatti audio
    float deltaTime = _delayTime - _currentDelayTime;
    _currentDelayTime += deltaTime * _smoothingFactor;
    
    // Converte il tempo di delay da secondi a campioni
    float delaySamples = _sampleRate * _currentDelayTime;
    
    // Legge il campione ritardato dalla linea di delay
    float delayed = _delayLine.Read(delaySamples);
    
    // Mixa il segnale originale con quello ritardato usando il crossfade
    float buffer_out = _crossfade.Process(buffer_in, delayed);
    
    // Scrive nella linea di delay il nuovo campione più il feedback
    _delayLine.Write(buffer_in + delayed * _feedback);

    return buffer_out; // Restituisce il campione processato
}
