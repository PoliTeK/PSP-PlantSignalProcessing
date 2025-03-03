#include "AnalogDelay.h" // Include l'header file della classe AnalogDelay

// Alloca un buffer nella SDRAM per memorizzare i campioni del delay
static float DSY_SDRAM_BSS delayBufferStorage[96000];

// Costruttore della classe AnalogDelay
AnalogDelay::AnalogDelay() : 
    _sampleRate(48000.0f),     // Frequenza di campionamento di default
    _delayTime(0.5f),         // Tempo di delay iniziale (0.5 secondi)
    _feedback(0.4f),          // Quantità di segnale ritardato che viene rimandato in ingresso
    _mix(0.5f),              // Bilanciamento tra segnale dry e wet
    _writePos(0),              // Posizione di scrittura nel buffer
    _readPos(0.0f),           // Posizione di lettura nel buffer (floating point per interpolazione)
    _currentDelayTime(0.5f),  // Tempo di delay corrente durante lo smoothing
    _smoothingFactor(0.00005f),// Fattore di smoothing per transizioni graduali
    _delayBuffer(delayBufferStorage) {} // Inizializzazione del buffer del delay

// Distruttore
AnalogDelay::~AnalogDelay() {}

// Inizializzazione del delay con il sample rate specificato
void AnalogDelay::Init(float sampleRate) {
    _sampleRate = sampleRate;
    // Azzera tutto il buffer del delay
    for(size_t i = 0; i < _delayBufferSize; i++) {
        _delayBuffer[i] = 0.0f;
    }
}

// Imposta il feedback, limitandolo tra 0.0 e 0.9
void AnalogDelay::setFeedback(float feedback) {
    _feedback = fclamp(feedback, 0.0f, 0.97f);
}

// Imposta il mix tra segnale originale e ritardato (0.0 - 1.0)
void AnalogDelay::setMix(float mix) {
    _mix = fclamp(mix, 0.0f, 1.0f);
}

// Imposta il tempo di delay, limitandolo tra 0.02 e 1.0 secondi
void AnalogDelay::setDelayTime(float delayTime) {
    _delayTime = fclamp(delayTime, 0.02f, 1.0f);
}

// Processa un singolo campione audio
float AnalogDelay::Process(float buffer_in) {
    // Calcola gradualmente il nuovo tempo di delay
    float deltaTime = _delayTime - _currentDelayTime;
    _currentDelayTime += deltaTime * _smoothingFactor;
    
    // Calcola la posizione di lettura in base al tempo di delay
    float delaySamples = _sampleRate * _currentDelayTime;
    _readPos = _writePos - delaySamples;
    while(_readPos < 0) _readPos += _delayBufferSize; // Wrap around se negativo
    
    // Calcola gli indici per l'interpolazione cubica
    int readPos_i = (int)_readPos;
    float frac = _readPos - readPos_i; // Parte frazionaria per l'interpolazione
    
    // Calcola gli indici dei punti adiacenti
    int im1 = readPos_i - 1;
    int ip1 = readPos_i + 1;
    int ip2 = readPos_i + 2;
    
    // Gestisce il wrap around degli indici
    if (im1 < 0) im1 += _delayBufferSize;
    ip1 %= _delayBufferSize;
    ip2 %= _delayBufferSize;
    
    // Legge i valori dei quattro punti per l'interpolazione
    float xm1 = _delayBuffer[im1];
    float x0 = _delayBuffer[readPos_i];
    float x1 = _delayBuffer[ip1];
    float x2 = _delayBuffer[ip2];
    
    // Calcola i coefficienti per l'interpolazione cubica di Hermite
    float c = (x1 - xm1) * 0.5f;
    float v = x0 - x1;
    float w = c + v;
    float a = w + v + (x2 - x0) * 0.5f;
    float b = w + a;
    
    // Calcola il valore interpolato
    float delayed = ((((a * frac) - b) * frac + c) * frac + x0);
    // Mixa il segnale originale con quello ritardato
    float buffer_out = (1.0f - _mix) * buffer_in + _mix * delayed;
    
    // Aggiorna il buffer con il nuovo campione più il feedback
    _delayBuffer[_writePos] = buffer_in + delayed * _feedback;
    _writePos = (_writePos + 1) % _delayBufferSize; // Avanza la posizione di scrittura

    return buffer_out; // Restituisce il campione processato
}
