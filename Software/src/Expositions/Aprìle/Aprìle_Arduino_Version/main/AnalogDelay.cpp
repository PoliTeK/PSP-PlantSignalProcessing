#include "AnalogDelay.h" // Include l'header file della classe AnalogDelay

// Variabile globale per il tempo di delay corrente
static float currentDelayTime = 0.0f;

// Alloca il buffer nella SDRAM
static float DSY_SDRAM_BSS delayBufferStorage[48000 * MAX_DELAY_TIME];

// Costruttore: inizializza tutti i parametri dell'effetto delay con valori predefiniti
AnalogDelay::AnalogDelay() : 
    _sampleRate(48000.0f),     // Frequenza di campionamento standard (48kHz)
    _delayTime(0.5f),          // Tempo di delay iniziale (500ms)
    _feedback(0.4f),           // Quantità di feedback iniziale (40%)
    _mix(0.5f),                // Mix tra segnale dry/wet (50%)
    _depth(0.0f),              // Profondità del pitch shifter (50%)
    _smooth_factor(0.00008f),  // Fattore di smoothing iniziale (0)
    _writePos(0),              // Posizione di scrittura nel buffer
    _readPos(0.0f),            // Posizione di lettura nel buffer
    _delayBuffer(delayBufferStorage) // Buffer di delay
{}

AnalogDelay::~AnalogDelay() {} // Distruttore (vuoto)

// Inizializza l'effetto con la frequenza di campionamento specificata
void AnalogDelay::Init(float sampleRate) {
    _sampleRate = sampleRate;    // Imposta la frequenza di campionamento
    _crossfade.Init();           // Inizializza il modulo di crossfade
    _ps.Init(sampleRate);        // Inizializza il pitch shifter
    _lfo.Init(sampleRate);       // Inizializza l'LFO
    _lfo.SetFreq(0.2f);          // Imposta la frequenza dell'LFO a 0.5 Hz
    _lfo.SetWaveform(Oscillator::WAVE_SIN); // Imposta l'LFO come onda sinusoidale
    
    // Azzera il buffer
    for(size_t i = 0; i < _delayBufferSize; i++) {
        _delayBuffer[i] = 0.0f;
    }
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
    // Applica smoothing al tempo di delay
    fonepole(currentDelayTime, _delayTime, _smooth_factor);
    
    // Calcola la posizione di lettura
    float delaySamples = _sampleRate * currentDelayTime;
    _readPos = _writePos - delaySamples;
    while(_readPos < 0) _readPos += _delayBufferSize;
    
    // Interpolazione cubica di Hermite
    int readPos_i = (int)_readPos;
    float frac = _readPos - readPos_i;
    
    // Calcola gli indici per l'interpolazione
    int im1 = readPos_i - 1;
    int ip1 = readPos_i + 1;
    int ip2 = readPos_i + 2;
    
    // Wrap degli indici
    if (im1 < 0) im1 += _delayBufferSize;
    readPos_i %= _delayBufferSize;
    ip1 %= _delayBufferSize;
    ip2 %= _delayBufferSize;
    
    // Leggi i campioni per l'interpolazione
    float xm1 = _delayBuffer[im1];
    float x0 = _delayBuffer[readPos_i];
    float x1 = _delayBuffer[ip1];
    float x2 = _delayBuffer[ip2];
    
    // Interpolazione di Hermite
    float c = (x1 - xm1) * 0.5f;
    float v = x0 - x1;
    float w = c + v;
    float a = w + v + (x2 - x0) * 0.5f;
    float b = w + a;
    float delayed = ((((a * frac) - b) * frac + c) * frac + x0);
    
    _lfo.SetFreq(1 - _delayTime);
    float lfo = _lfo.Process() * _depth;
    _ps.SetTransposition(lfo * 2.0f);
    float delayed_shifted = _ps.Process(delayed);
    
    float buffer_out = _crossfade.Process(buffer_in, delayed_shifted);
    
    _delayBuffer[_writePos] = buffer_in + delayed_shifted * _feedback;
    _writePos = (_writePos + 1) % _delayBufferSize;
    
    return buffer_out;
}
