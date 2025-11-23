// TO DO: 
//      - add const char with instantiation (e.g. name of param of set param)

#include "../../Classes/Effect/sigChain_handler.hpp"
#include "../../Classes/Effect/effectWrapper.hpp"
#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

using namespace daisy;
DaisySeed hw;
float outSample;
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
        outSample = chain1.Process();
		out[0][i] = outSample;
		out[1][i] = outSample;
	}
}


/* MAIN BLOCKS INSTANTIATION */
// here the block of the signal chain are instantiated
daisysp::Oscillator lfo1;




/* SIGNAL CHAIN CREATION */

// instantiation of the chain
sigChain_handler chain1;


int main (){

	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
    int sample_rate = 44100;

    /* MAIN BLOCKS INITIALIZATION */
    // here the blocks of the signal chain are initialized
  
    lfo1.Init(sample_rate);

    /* WRAPPER INSTANTIATION*/

    // instantiation of effect wrapper:
    // - the first templated argument tells it the return value
    //   of the process function
    // - the second templated argument is the type of process arg
    // - the third templated argument tells it the argument value of
    //   the setter function, in this case SetFreq(float);
    // - the fourth templated argument is not needed now but would be
    //   the second argument of the setter function that takes 2 args
    // - the fifth templated argument would be the return type of setters   
    EffectWrapper <daisysp::Oscillator, float, float> oscWrapper;
    
    // creation of function pointers to pass to effect wrapper
    // can be done before to use constructor (cleaner)     
    void (daisysp::Oscillator::* setPtr)(float) = &daisysp::Oscillator::SetFreq;
    float (daisysp::Oscillator::* processPtr)() = &daisysp::Oscillator::Process; 

    // setting of internal pointers of wrapper
    oscWrapper.AddSetParam(setPtr);
    oscWrapper.AddProcess (processPtr);

    // first block inserted: works because of POLYMORPHISM
    chain1.addEffect (&oscWrapper);
    
    
   

    return 0;
}