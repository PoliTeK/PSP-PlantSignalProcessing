#include "AnalogDelay.h" // Include l'header file della classe AnalogDelay

// Variabile globale per il tempo di delay corrente
static float currentDelayTime = 0.0f;

// Costruttore: inizializza tutti i parametri dell'effetto delay con valori predefiniti
AnalogDelay::AnalogDelay() : 
    _sampleRate(48000.0f),     // Frequenza di campionamento standard (48kHz)
    _delayTime(0.5f),          // Tempo di delay iniziale (500ms)
    _feedback(0.4f),           // Quantità di feedback iniziale (40%)
    _mix(0.5f),                // Mix tra segnale dry/wet (50%)
    _depth(0.0f),              // Profondità del pitch shifter (50%)
    _smooth_factor(0.00008f)       // Fattore di smoothing iniziale (0)
{}

AnalogDelay::~AnalogDelay() {} // Distruttore (vuoto)

// Inizializza l'effetto con la frequenza di campionamento specificata
void AnalogDelay::Init(float sampleRate) {
    _sampleRate = sampleRate;    // Imposta la frequenza di campionamento
    _delayLine.Init();           // Inizializza la linea di ritardo
    _crossfade.Init();           // Inizializza il modulo di crossfade
    _ps.Init(sampleRate);           // Inizializza il pitch shifter
    _lfo.Init(sampleRate);       // Inizializza l'LFO
    _lfo.SetFreq(0.2f);       // Imposta la frequenza dell'LFO a 0.5 Hz
    _lfo.SetWaveform(Oscillator::WAVE_SIN); // Imposta l'LFO come onda sinusoidale
}

// Imposta il livello di feedback con limit tra 0 e 0.99
void AnalogDelay::setFeedback(float feedback) {
    _feedback = fclamp(feedback, 0.0f, 0.99f); // Limita il feedback per evitare auto-oscillazioni
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

// Imposta la profondità del pitch shifter
void AnalogDelay::setDepth(float depth) {
    _depth = fclamp(depth, 0.0f, 1.0f); // Limita la profondità tra 0 e 1
}

// Imposta il fattore di smoothing per il tempo di delay
void AnalogDelay::setSmoothFactor(float smooth_factor) {
    _smooth_factor = fmap(smooth_factor, 0.00001f, 0.0001f, Mapping::LINEAR); // Limita il fattore di smoothing
}

// Processa un singolo campione audio
float AnalogDelay::Process(float buffer_in) {
    // Applica smoothing al tempo di delay usando fonepole
    fonepole(currentDelayTime, _delayTime, _smooth_factor);
    
    // Converte il tempo di delay da secondi a campioni
    float delaySamples = _sampleRate * currentDelayTime;
    
    // Legge il campione ritardato dalla linea di delay
    float delayed = _delayLine.Read(delaySamples);

    _lfo.SetFreq(1 - _delayTime);
    float lfo = _lfo.Process() * _depth; // Processa l'LFO per il pitch shifter
    _ps.SetTransposition(lfo * 2.0f);  // Imposta la trasposizione a +- 5 semitoni
    float delayed_shifted = _ps.Process(delayed);
    
    // Mixa il segnale originale con quello ritardato usando il crossfade
    float buffer_out = _crossfade.Process(buffer_in, delayed_shifted);
    
    // Scrive nella linea di delay il nuovo campione più il feedback
    _delayLine.Write(buffer_in + delayed_shifted * _feedback);

    return buffer_out; // Restituisce il campione processato
}
