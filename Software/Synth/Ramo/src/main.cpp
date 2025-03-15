// Inclusione delle librerie necessarie
#include "daisy_seed.h" // Libreria principale per la scheda Daisy Seed
#include "daisysp.h"    // Libreria DSP per Daisy
#include "Ramo.h"       // Libreria per il synth Ramo

// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;
using namespace daisysp;



// Dichiarazione delle variabili globali
static DaisySeed hw;           // Oggetto principale per gestire l'hardware Daisy
static Ramo ramo;              // Oggetto per il synth Ramo

AdcChannelConfig adcConfig[3]; // Array di configurazione per 3 canali ADC

float f;
float shape;


void InitHardware() {
    
    // Inizializzazione dei tre pin ADC per i potenziometri
    adcConfig[0].InitSingle(hw.GetPin(15)); // Primo potenziometro per il pitch
    adcConfig[1].InitSingle(hw.GetPin(16)); // Secondo potenziometro per shape1
    adcConfig[2].InitSingle(hw.GetPin(17)); // Terzo potenziometro   per shape 2
    hw.adc.Init(adcConfig, 3);              // Inizializzazione ADC con 3 canali
    hw.adc.Start();                         // Avvio della conversione ADC
}


// Callback audio eseguito per ogni blocco audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    float sig;
    ramo.SetFreq(f);                                                            // Imposta la frequenza
    ramo.SetShape(shape);                                              // Imposta la forma d'onda

    for(size_t i = 0; i < size; i++) {
        sig = ramo.Process();                                                        // Processa il segnale audio
        // Output processed signal
        out[0][i] = sig;
        out[1][i] = sig;
    }
}

int main() {
    hw.Configure();                                                             // Configurazione hardware
    hw.Init();                                                                  // Inizializzazione hardware

    hw.SetAudioBlockSize(4);                                                    // Imposta dimensione del blocco audio
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Imposta sample rate a 48kHz


    ramo.Init(hw.AudioSampleRate());                                            // Inizializza il synth Ramo
    ramo.SetWaveforms(1,1);                                   // Imposta le forme d'onda
    ramo.SetFreq(440.0f);                                                   // Imposta la frequenza
    ramo.SetDetune(1,1);                                                  // Imposta il detune
    ramo.SetAmp(0.5f, 0.5f);                                                // Imposta l'ampiezza
    ramo.SetShape(0.5f);                                              // Imposta shape
    
    InitHardware();                                                             // Inizializza i potenziometri
    hw.StartAudio(AudioCallback);                              // Avvia il callback audio

    // Loop infinito per mantenere il programma in esecuzione
    while(1) {

        f = 220.0f + 220.0f * hw.adc.GetFloat(0);
        shape = hw.adc.GetFloat(1);
        
        // Attendi 1ms per ridurre l'utilizzo della CPU
        System::Delay(1);
    }
}
