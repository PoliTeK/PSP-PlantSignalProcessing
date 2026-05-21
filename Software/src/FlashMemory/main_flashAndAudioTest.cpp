// this is used to test the effects of a flash read with audio generation
// seems that interrupts are disabled during flash read.

#include "flash_utils.h"
#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

#define ELAPSED_TIME_FOR_FLASH_READ 1e3
using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// test oscillator
static Oscillator osc;

static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i += 2)
    {
        out[i] = out[i + 1] = osc.Process();               
    }
}

int main (){

    hw.Init();

     /** Start Log */
    hw.StartLog(true);
    hw.PrintLine("Inizio Test Flash QSPI...");
    
    hw.SetAudioBlockSize(16);
    osc.Init(48000);
	int oscFreq = 440;
    osc.SetFreq(oscFreq);
    osc.SetAmp(0.01f);
    hw.StartAudio(AudioCallback);

	uint32_t value;
    int i = 440;
    unsigned int counter = 0;
    
    // every once in a while there will be a flash read
    while (1){
        i = (i + 1) % 440;
        oscFreq = (oscFreq +  1) % 440;
        osc.SetFreq(oscFreq + 440);
        hw.DelayMs(1);
        
        if (counter == ELAPSED_TIME_FOR_FLASH_READ){
	        FlashUtils::read(0, value);
	        hw.PrintLine("Valore letto: %d", value);
	        value++;
	        hw.PrintLine("Scrivo valore: %d", value);
	        FlashUtils::write(0, value, hw);
            counter = 0;
        }
        counter++;
    }
    
    return 0;

 }