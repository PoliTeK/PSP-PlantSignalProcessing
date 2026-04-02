#include "../../libs/libDaisy/src/daisy_seed.h"
#include "PlantConditioner.h"
#include "AudioEngine.h"

using namespace daisy;

// Istanze hardware e software
DaisySeed hw;
PlantConditioner pc;
AudioEngine synth;

// Variabile globale condivisa tra il main loop e la callback audio
ControlsStruct audio_controls = {440.0f, false};

// Callback Audio: eseguita dal processore ogni volta che serve riempire il buffer audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // 1. Aggiorna il sintetizzatore con gli ultimi dati disponibili letti dalla pianta
    synth.Update(audio_controls);

    // 2. Genera l'audio per la dimensione del buffer corrente
    for (size_t i = 0; i < size; i++) {
        // Calcola il singolo campione
        float sample = synth.Process();
        
        // Invia lo stesso campione su entrambi i canali di uscita (Dual Mono)
        out[0][i] = sample; // Left
        out[1][i] = sample; // Right
    }
}

int main() {
    // Inizializzazione Hardware
    hw.Init();
    
    // Configurazione blocco audio per bassa latenza (opzionale ma consigliato per i synth)
    hw.SetAudioBlockSize(16);

    // Inizializzazione PlantConditioner
    pc.Init(IIR::BUTTERWORTH2, &hw);
    pc.setDelta(50.0f);
    pc.setCurve(1.0f);
    pc.setScale(PlantConditioner::, PlantConditioner::Major);
    pc.setOctave(4);

    // Inizializzazione AudioEngine
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    // Avvia la callback audio in background
    hw.StartAudio(AudioCallback);

    while(1) {

        PlantConditioner::PlantState plant_data = pc.Process();

        audio_controls.freq = plant_data._freq;
        audio_controls.gate = plant_data._gate;

        // 3. Pausa di 1 ms per stabilizzare il bus I2C e non saturare la CPU
        System::Delay(5);
    }
}