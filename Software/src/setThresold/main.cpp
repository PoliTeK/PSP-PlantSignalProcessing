#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "PlantConditioner.h"

using namespace daisy;
using namespace daisysp;
// Istanze hardware e software
DaisySeed hw;
PlantConditioner pc;
static Oscillator osc;


// Callback Audio: eseguita dal processore ogni volta che serve riempire il buffer audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {

    // 2. Genera l'audio per la dimensione del buffer corrente
    for (size_t i = 0; i < size; i++) {
        // Calcola il singolo campione
        float sample = osc.Process();
        
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
    osc.Init(48000);
    pc.Init(IIR::Butterworth, &hw);
    pc.setDelta(50.0f);
    pc.setCurve(1.0f);
    pc.setScale(PlantConditioner::G, PlantConditioner::Major);
    pc.setOctave(4);
    pc.setThreshold (20,1, 0);

    osc.SetFreq(440);
    // Avvia la callback audio in background
    hw.StartAudio(AudioCallback);

    PlantConditioner::PlantState plant_data;
    while(1) {

        plant_data = pc.Process();
        plant_data._gate ? osc.SetAmp(0.5f) : osc.SetAmp(0);

        // 3. Pausa di 1 ms per stabilizzare il bus I2C e non saturare la CPU
        System::Delay(5);
    }
}