#include "daisy_seed.h"
#include "daisysp.h"
#include "mpr121.h"

using namespace daisy;
using namespace daisysp;

uint16_t currTouched;
uint16_t lastTouched;

bool led_state = true;


// Define the frequencies for  two octaves surrounding middle C
double scale[][12] = {
    // Octave below middle C
    {130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94},
    // Middle C and the octave above
    {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88},
    // Octave above middle C
    {523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77}};

DaisySeed hw;
Mpr121I2C mpr121;
Oscillator osc;


static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    float sig;
    for(size_t i = 0; i < size; i += 2)
    {
        sig = osc.Process();

        // left out
        out[i] = sig;

        // right out
        out[i + 1] = sig;
    }

}

int main(void)
{
	hw.Init();
    hw.StartLog();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

    //How many samples we'll output per second
    float samplerate = hw.AudioSampleRate();

	// Initialize MPR121 with I2C configuration
    Mpr121I2C::Config mprConfig;
    mprConfig.transport_config.periph = I2CHandle::Config::Peripheral::I2C_1;
    mprConfig.transport_config.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    mprConfig.transport_config.scl = Pin(PORTB,8);  // Replace with your SCL pin
    mprConfig.transport_config.sda = Pin(PORTB,9);  // Replace with your SDA pin

	// Set additional MPR121 configuration if needed
    mprConfig.touch_threshold = 12;
    mprConfig.release_threshold = 6;

    //Set up oscillator
    osc.Init(samplerate);
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(1.f);
    //osc.SetFreq(440);

	if(mpr121.Init(mprConfig) != Mpr121I2C::OK)
    {
        // Handle initialization error
        for(;;)
        {
            // Set the onboard LED
            hw.SetLed(led_state);
    
            // Toggle the LED state for the next time around.
            led_state = !led_state;
    
            // Wait 500ms
            System::Delay(500);
        }
    }
	// Setup to print 
	hw.StartLog();

	while(1) 
	{
		// Read touch status
        currTouched = mpr121.Touched();

        // Process the touch status as needed
        for(int i = 0; i < 12; i++)
        {
            if((currTouched & (1 << i)) && !(lastTouched& (1 << i)))
            {
                // Channel i is touched, print the channel number
                hw.PrintLine("Channel %d touched", i);
            }
            if(!(currTouched & (1 << i)) && (lastTouched& (1 << i)))
            {
                // Channel i is released, print the channel number
                hw.PrintLine("Channel %d released", i);
            }
        }
        lastTouched = currTouched;

        switch (currTouched)
        {
        case 0:
            osc.SetFreq(scale[0][0]);
            break;
        
        case 1:
            osc.SetFreq(scale[0][1]);
            break;
        case 2:
            osc.SetFreq(scale[0][2]);
            break;
        case 3:
            osc.SetFreq(scale[0][3]);
            break;
        case 4:
            osc.SetFreq(scale[0][5]);
            break;
        case 5:
            osc.SetFreq(scale[0][7]);
            break;
        default:
            break;
        }
        // Add necessary delay or other code here...
    }
}