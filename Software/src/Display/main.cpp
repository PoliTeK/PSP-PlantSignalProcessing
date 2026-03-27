/**
 * @brief Test main function for display
 * 
 * This is the main entry point for the display test application.
 * It serves as a test harness to verify display functionality
 * and demonstrate display-related operations.
 * 
 * @return int Exit status code (0 for success, non-zero for failure)
 */
 
#include "displayHandler.h"
#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// test oscillator
static Oscillator osc;

// real disp obj, will be handled not called directly
MyOledDisplay disp;
 
DisplayHandler disp_handle (&disp);


static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    float oscOut = 0;
    for (size_t i = 0; i < size; i += 2)
    {
        oscOut = osc.Process();

        disp_handle.pushAudioSample (oscOut);
        // uncomment to decimate -> frequency displayed is double the true one 
        //  bool decimate;
        //  if (decimate) {
        //  // also applies some scale factor
        //  disp_handle.pushAudioSample (oscOut/1.25);
        //  decimate = !decimate;
        // }
        out[i] = out[i + 1] = osc.Process();               
    }
}

int main (){

    hw.Init();    
    hw.SetAudioBlockSize(16);
    osc.Init(48000);
    int oscFreq = 440;
    osc.SetFreq(oscFreq);
    osc.SetAmp(0.05f);
    MyOledDisplay::Config disp_cfg;
    disp.Init(disp_cfg);
    int yscale = 300;
    disp_handle.SetYscale(yscale);
    disp_handle.SetState(DisplayState::WAVEFORM_VIEWER); 
    hw.StartAudio(AudioCallback);
    int i = 440;
    while (1){
        i = (i + 1) % 440;
        oscFreq = (oscFreq +  1) % 440;
        osc.SetFreq(oscFreq + 440);
        disp_handle.Update();
        // hw.DelayMs(10); --> NO! this function creates noise
    }
    
    return 0;

 }