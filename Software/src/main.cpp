#include "../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"
#include "../libs/PoliTeKDSP/libs/DaisySP/Source/daisysp.h"
#include "../libs/PoliTeKDSP/Effects/AnalogDelay/src/AnalogDelay.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;




// --- AUDIO CALLBACK (High Priority) ---
static void AudioCallback(daisy::AudioHandle::InputBuffer  in,
                          daisy::AudioHandle::OutputBuffer out,
                          size_t                           size)
{
    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}

int main(){
    hw.Init();

    hw.StartAudio(AudioCallback);

    while(1)
    {
    }
}