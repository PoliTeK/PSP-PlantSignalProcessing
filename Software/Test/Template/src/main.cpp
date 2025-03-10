// Inclusione delle librerie necessarie
#include "daisy_seed.h" // Libreria principale per la scheda Daisy Seed
#include "daisysp.h"    // Libreria DSP per Daisy


// Utilizzo dei namespace per semplificare la scrittura del codice
using namespace daisy;
using namespace daisysp;

// Dichiarazione delle variabili globali
DaisySeed hw;           // Oggetto principale per gestire l'hardware Daisy



// Callback audio eseguito per ogni blocco audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // Processo audio per ogni campione
    for(size_t i = 0; i < size; i++) {
        float inL = in[0][i];           // Canale sinistro input
        float inR = in[1][i];           // Canale destro input
        
        // Output processed signal
        out[0][i] = inL;
        out[1][i] = inR;
    }
}

int main() {
    hw.Configure();      // Configurazione hardware
    hw.Init();          // Inizializzazione hardware

    hw.SetAudioBlockSize(4);    // Imposta dimensione del blocco audio
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ); // Imposta sample rate a 48kHz

    hw.StartAudio(AudioCallback); // Avvia il processamento audio

    // Loop infinito per mantenere il programma in esecuzione
    while(1) {
        // Attendi 1ms per ridurre l'utilizzo della CPU
        System::Delay(1);
    }
}
