// Inclusione delle librerie necessarie
#include "daisy_seed.h" // Libreria principale per la scheda Daisy Seed
#include "daisysp.h"    // Libreria DSP per Daisy
#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico
#include "../../../Utilities/Utilities.h" // Libreria custom per le utility

#define ADC_CH 4 // Numero di canali ADC da utilizzare

// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;
using namespace daisysp;

// Dichiarazione delle variabili globali
DaisySeed hw;           // Oggetto principale per gestire l'hardware Daisy
AnalogDelay delay;      // Oggetto per gestire l'effetto delay

Utilities util;         // Oggetto per gestire le utility


AdcChannelConfig adcConfig[ADC_CH]; // Array di configurazione per 3 canali ADC

// Variabili per i parametri del delay
float delayTime;  // Tempo del delay
float feedback;   // Quantità di feedback del delay
float mix;        // Mix tra segnale dry/wet
float depth;      // Profondità del pitch shifter

// Aggiungi queste variabili globali
uint32_t lastBlink;     // Ultimo momento di cambio stato LED
bool ledState;          // Stato corrente del LED

void InitHardware() {
    
    // Inizializzazione dei tre pin ADC per i potenziometri
    adcConfig[0].InitSingle(hw.GetPin(15)); // Primo potenziometro
    adcConfig[1].InitSingle(hw.GetPin(16)); // Secondo potenziometro
    adcConfig[2].InitSingle(hw.GetPin(17)); // Terzo potenziometro
    adcConfig[3].InitSingle(hw.GetPin(18)); // Quarto potenziometro
    hw.adc.Init(adcConfig, ADC_CH);              // Inizializzazione ADC con 3 canali
    hw.adc.Start();                         // Avvio della conversione ADC
}

// Aggiungi questa funzione di utility prima di ReadControls()
float LogScale(float pos, float min_value, float max_value) {
    // Implementa una scala logaritmica tra min_value e max_value
    float min_log = logf(min_value);
    float max_log = logf(max_value);
    float exp_scale = min_log + (max_log - min_log) * pos;
    return expf(exp_scale);
}

void ReadControls() {
    float knob1 = util.SmoothAnalogRead(0, Utilities::LOW_RES, hw);
    float knob2 = hw.adc.GetFloat(1);
    float knob3 = hw.adc.GetFloat(2);
    float knob4 = hw.adc.GetFloat(3);

    // Nuova implementazione del controllo delay time
    const float MIN_DELAY = 0.02f;  // 20ms minimo delay
    const float MAX_DELAY = 1.0f;   // 1000ms massimo delay
    
    // Applica la scala logaritmica al delay time
    delayTime = LogScale(knob3, MIN_DELAY, MAX_DELAY);
    
    feedback = knob1 * 0.99f;
    mix = knob2;

    depth = knob4;


    // Aggiorna i parametri del led
    uint32_t now = System::GetNow();
    uint32_t blinkInterval = static_cast<uint32_t>(delayTime * 1000); // Converti in millisecondi
    
    if(now - lastBlink >= blinkInterval) {
        ledState = !ledState;
        hw.SetLed(ledState);
        lastBlink = now;
    }
}

// Callback audio eseguito per ogni blocco audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    ReadControls();                     // Aggiorna i controlli
    delay.setDelayTime(delayTime);      // Imposta il tempo di delay
    delay.setFeedback(feedback);        // Imposta il feedback
    delay.setMix(mix);                  // Imposta il mix
    delay.setDepth(depth);              // Imposta la profondità del pitch shifter
    
    // Processo audio per ogni campione
    for(size_t i = 0; i < size; i++) {
        float inL = in[0][i];           // Canale sinistro input
        
        float wetL = delay.Process(inL);
        
        // Output processed signal
        out[0][i] = wetL;
        out[1][i] = wetL;
    }
}

int main() {
    hw.Configure();      // Configurazione hardware
    hw.Init();          // Inizializzazione hardware

    hw.SetAudioBlockSize(4);    // Imposta dimensione del blocco audio
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ); // Imposta sample rate a 48kHz

    InitHardware();     // Inizializza LED e ADC
    
    // Inizializza le variabili del LED
    lastBlink = System::GetNow();
    ledState = false;

    delay.Init(hw.AudioSampleRate()); // Inizializza il delay con il sample rate corrente
    util.init(hw.AudioSampleRate()); // Inizializza le utility con il sample rate corrente

    hw.StartAudio(AudioCallback); // Avvia il processamento audio

    // Loop infinito per mantenere il programma in esecuzione
    while(1) {
        // Attendi 1ms per ridurre l'utilizzo della CPU
        System::Delay(1);
    }
}
