// Inclusione delle librerie necessarie
#include "daisy_seed.h" // Libreria principale per la scheda Daisy Seed
#include "daisysp.h"    // Libreria DSP per Daisy
#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico

// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;
using namespace daisysp;

// Dichiarazione delle variabili globali
DaisySeed hw;           // Oggetto principale per gestire l'hardware Daisy
AnalogDelay delay;      // Oggetto per gestire l'effetto delay

AdcChannelConfig adcConfig[3]; // Array di configurazione per 3 canali ADC

// Variabili per i parametri del delay
float delayTime;  // Tempo del delay
float feedback;   // Quantità di feedback del delay
float mix;        // Mix tra segnale dry/wet

void InitADC() {
    // Inizializzazione dei tre pin ADC per i potenziometri
    adcConfig[0].InitSingle(hw.GetPin(15)); // Primo potenziometro
    adcConfig[1].InitSingle(hw.GetPin(16)); // Secondo potenziometro
    adcConfig[2].InitSingle(hw.GetPin(17)); // Terzo potenziometro
    hw.adc.Init(adcConfig, 3);              // Inizializzazione ADC con 3 canali
    hw.adc.Start();                         // Avvio della conversione ADC
}

void ReadControls() {
    // Lettura dei valori dei potenziometri (range 0-1)
    float knob1 = hw.adc.GetFloat(0);
    float knob2 = hw.adc.GetFloat(1);
    float knob3 = hw.adc.GetFloat(2);

    // Gestione non lineare del tempo di delay
    if (knob3 < 0.5f) {
        // Prima metà della corsa: risposta esponenziale
        float normalized = knob3 * 2.0f;              // Normalizzazione a 0-1
        float exponential = normalized * normalized;   // Curva quadratica
        delayTime = 0.05f + exponential * 0.475f;    // Mapping a 0.05-0.525
    } else {
        // Seconda metà della corsa: risposta lineare
        float normalized = (knob3 - 0.5f) * 2.0f;    // Normalizzazione a 0-1
        delayTime = 0.525f + normalized * 0.475f;    // Mapping a 0.525-1.0
    }

    feedback = knob1 * 0.9f;    // Imposta il feedback (limitato a 0.9)
    mix = knob2;                // Imposta il mix dry/wet
}

// Callback audio eseguito per ogni blocco audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    ReadControls();                     // Aggiorna i controlli
    delay.setDelayTime(delayTime);      // Imposta il tempo di delay
    delay.setFeedback(feedback);        // Imposta il feedback
    delay.setMix(mix);                  // Imposta il mix
    
    // Processo audio per ogni campione
    for(size_t i = 0; i < size; i++) {
        float inL = in[0][i];           // Canale sinistro input
        float inR = in[1][i];           // Canale destro input
        
        float wetL = delay.Process(inL);
        float wetR = delay.Process(inR);
        
        // Output processed signal
        out[0][i] = wetL;
        out[1][i] = wetR;
    }
}

int main() {
    hw.Configure();      // Configurazione hardware
    hw.Init();          // Inizializzazione hardware

    hw.SetAudioBlockSize(4);    // Imposta dimensione del blocco audio
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ); // Imposta sample rate a 48kHz

    InitADC();          // Inizializza i convertitori analogico-digitali

    delay.Init(hw.AudioSampleRate()); // Inizializza il delay con il sample rate corrente

    hw.StartAudio(AudioCallback); // Avvia il processamento audio

    // Loop infinito per mantenere il programma in esecuzione
    while(1) {
        // Attendi 1ms per ridurre l'utilizzo della CPU
        System::Delay(1);
    }
}
