// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth
#include "config1.h"
#include "pentaPianta.h"
#include "Effect.h"

//#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico



// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;


static DaisySeed hw;                                                // Main object for managing Daisy hardware



static Ramo Pluck;                                                  // Object for the Ramo synth
static Ramo Bass;
static Ramo Pad;                                                    // Object for the Ramo synth
 
static Oscillator lfo_Pluck;
static Oscillator lfo_Bass;
static Oscillator lfo_Pad;                                          // LFO object for modulation

static Adsr AmpEnv_Pluck;                                          // Envelope object for amplitude
static Adsr AmpEnv_Bass;
static Adsr AmpEnv_Pad;                                            // Envelope object for amplitude

static Adsr FilterEnv_Pluck;                                       // Envelope object for filter
static Adsr FilterEnv_Bass;
static Adsr FilterEnv_Pad;                                        // Envelope object for filter

static Svf filter_Pluck;                                          // Filter object for the synth
static Svf filter_Bass;
static Svf filter_Pad;                                           // Filter object for the synth




float Raw_Pluck, Raw_Bass, Raw_Pad;                                   // Raw signal variables
float out_noFX;                                                       // Output signal variable
                                                     




float now_f_lfo_Bass ;
float now_f_lfo_Pluck ;
float now_f_lfo_Pad ;  

float now_f_Pluck ;
float now_f_Bass ;
float now_f_Pad ;

float knob1, knob2, knob3, knob4, knob5, knob6, knob7; 

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {

                                                    
    for (size_t i = 0; i < size; i++) {
//--------------------------------------------------------------------Setta DelayTime
    //delay.setDelayTime(delayTime);      // Imposta il tempo di delay
//-------------------------------------------------------------------Setta Shape
    lfo_Pluck.SetFreq(f_lfo_Pluck);
    lfo_Bass.SetFreq(f_lfo_Bass);
    lfo_Pad.SetFreq(f_lfo_Pad);
    
    lfo_Pad.SetAmp(amp_lfo_Pad); // Set LFO amplitude for Pad
    lfo_Bass.SetAmp(amp_lfo_Bass); // Set LFO amplitude for Bass
    lfo_Pluck.SetAmp(amp_lfo_Pluck); // Set LFO amplitude for Pluck


    shape_Pluck = lfo_Pluck.Process();                                   
    shape_Bass = lfo_Bass.Process();                                   
    shape_Pad = lfo_Pad.Process();                                   

    Pluck.SetShape(shape_Pluck);                                     
    Bass.SetShape(shape_Bass);                                     
    Pad.SetShape(shape_Pad);                                     
    
//---------------------------------------------------------------Setta Freq
    Pluck.SetFreq(f_Pluck);                                         
    Bass.SetFreq(f_Bass);                                         
    Pad.SetFreq(f_Pad);                 

//--------------------------------------------------------------------------------------------------Setta Inviluppo Amplitude
    amp_env_Pluck = AmpEnv_Pluck.Process(Gate_Pluck);                             
    amp_env_Bass = AmpEnv_Bass.Process(Gate_Bass);                             
    amp_env_Pad = AmpEnv_Pad.Process(Gate_Pad);                             

//--------------------------------------------------------------------------------------------------Setta iinviluppo filtro
    filt_env_Pluck = FilterEnv_Pluck.Process(Gate_Pluck);                          
    filt_env_Bass = FilterEnv_Bass.Process(Gate_Bass);                           
    filt_env_Pad = FilterEnv_Pad.Process(Gate_Pad);                          
//---------------------------------------------------------------------------------------------------Setta ft Filtri
    filter_Pluck.SetFreq(ft_pluck - filt_env_Pluck*f_EG_pluck);                             
    filter_Bass.SetFreq(ft_bass + filt_env_Bass*f_EG_bass);
    filter_Pad.SetFreq(ft_pad + filt_env_Pad*f_EG_pad);                            
//----------------------------------------------------------------------------------------------------Setta Amp 
    Pluck.SetGain(amp_env_Pluck);                                     
    Bass.SetGain(amp_env_Bass);                                     
    Pad.SetGain(amp_env_Pad);                                     
//-----------------------------------------------------------------------------------------------------Processa rami
    Raw_Pluck = Pluck.Process();
    Raw_Bass = Bass.Process();
    Raw_Pad = Pad.Process();
//------------------------------------------------------------------------------------------------------Filtra Rami
    filter_Pluck.Process(Raw_Pluck);
    filter_Bass.Process(Raw_Bass);
    filter_Pad.Process(Raw_Pad);
//------------------------------------------------------------------------------------------------------Somma tutto con guadagni
    out_noFX = filter_Pluck.Low() * mix_amp_Pluck + filter_Bass.Low() * mix_amp_Bass + filter_Pad.Low() * mix_amp_Pad; // Output processed signal (left channel)
//-------------------------------------------------------------------------------------------------------Applica effetto
    //float out_FX = delay.Process(out_noFX); // Process the output signal with the delay effect

//-------------------------------------------------------------------------------------------------------Output
        out[0][i] = out_noFX;                                         // Output processed signal (left channel)
        out[1][i] = out_noFX;                                         // Output processed signal (right channel)
    }
}


UartHandler uart;
UartHandler::Config config;
DaisySeed hw;
pentaPianta penta[3]; // Array of pentaPianta objects

Effect effect[4]; // Array of Effect objects
void getData()
{
	char start_bit = 'S';
	uint8_t data[3];
	while (start_bit != 'B') // Wait for the start bit
	{
		uart.BlockingReceive((uint8_t*)&start_bit, sizeof(start_bit), 100);
	}
	for (size_t i = 0; i < 3; i++)
	{
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			penta[i].zeroCount(); // Set the first parameter of pentaPianta object i
		}
		else
		{
			penta[i].incCount();					  // Increment the count of the number of times the sensor has been read
		}
		penta[i].setRawValue(((uint16_t)data[2]<<8)+(uint16_t)data[1]);
	}
		for (size_t i = 0; i < 4; i++)
	{
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			effect[i].setGate(false); // Set the first parameter of pentaPianta object i
		}
		else
		{
			effect[i].setGate(true);					  // Increment the count of the number of times the sensor has been read// Set the second parameter of pentaPianta object i
		}
		penta[i].setRawValue(((uint16_t)data[2]<<8)+(uint16_t)data[1]);
	}
}


int main() {
    hw.Configure();                                                             // Hardware configuration
    hw.Init();                                                                  // Hardware initialization

    hw.SetAudioBlockSize(4);                                                    // Set audio block size
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Set sample rate to 48kHz
//------------------------------------------------------------------------------Inizializza Ogetti audio e parametri
    Pluck.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Bass.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pad.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth

    Pluck.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Bass.SetWaveforms(wf_Bass);                                                      // Set waveforms
    Pad.SetWaveforms(wf_Pad);                                                      // Set waveforms

    Pluck.SetFreq(f_Pluck);                                                       // Set frequency
    Bass.SetFreq(f_Bass);                                                       // Set frequency
    Pad.SetFreq(f_Pad);                                                       // Set frequency

    Pluck.SetDetune(ef_Pluck);                                                         // Set detune
    Bass.SetDetune(ef_Bass);                                                         // Set detune
    Pad.SetDetune(ef_Pad);                                                         // Set detune

    Pluck.SetAmp(g_Pluck);                                                             // Set amplitude
    Bass.SetAmp(g_Bass);                                                             // Set amplitude
    Pad.SetAmp(g_Pad);                                                             // Set amplitude

    Pluck.SetShape(0);                                                        // Set shape modulation
    Bass.SetShape(0);                                                        // Set shape modulation
    Pad.SetShape(0);                                                        // Set shape modulation
//----------------------------------------------------------------------------------Inizializza LFO e parametri
    lfo_Pluck.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Bass.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pad.Init(hw.AudioSampleRate());                                          // Initialize LFO

    lfo_Pluck.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform

    lfo_Pluck.SetFreq(f_lfo_Pluck);                                                    // Set LFO frequency
    lfo_Bass.SetFreq(f_lfo_Bass);                                                    // Set LFO frequency
    lfo_Pad.SetFreq(f_lfo_Pad);                                                    // Set LFO frequency

    lfo_Pluck.SetAmp(1);                                                    // Set LFO amplitude
    lfo_Bass.SetAmp(1);                                                    // Set LFO amplitude
    lfo_Pad.SetAmp(1);                                                    // Set LFO amplitude

//-----------------------------------------------------------------------------Inizializza AMP ADSR e parametri
    AmpEnv_Pluck.Init(hw.AudioSampleRate());                                       // Initialize envelope
    AmpEnv_Bass.Init(hw.AudioSampleRate());                                       // Initialize envelope
    AmpEnv_Pad.Init(hw.AudioSampleRate());                                       // Initialize envelope

    AmpEnv_Pluck.SetTime(ADSR_SEG_ATTACK, Pluck_Amp_ADSR[0]);                                  // Set attack time
    AmpEnv_Bass.SetTime(ADSR_SEG_ATTACK, Bass_Amp_ADSR[0]);                                  // Set attack time
    AmpEnv_Pad.SetTime(ADSR_SEG_ATTACK, Pad_Amp_ADSR[0]);                                  // Set attack time

    AmpEnv_Pluck.SetTime(ADSR_SEG_DECAY, Pluck_Amp_ADSR[1]);                                   // Set decay time
    AmpEnv_Bass.SetTime(ADSR_SEG_DECAY, Bass_Amp_ADSR[1]);                                   // Set decay time
    AmpEnv_Pad.SetTime(ADSR_SEG_DECAY, Pad_Amp_ADSR[1]);                                   // Set decay time

    AmpEnv_Pluck.SetSustainLevel(Pluck_Amp_ADSR[2]);                                          // Set sustain level
    AmpEnv_Bass.SetSustainLevel(Bass_Amp_ADSR[2]);                                          // Set sustain level
    AmpEnv_Pad.SetSustainLevel(Pad_Amp_ADSR[2]);                                          // Set sustain level

    AmpEnv_Pluck.SetTime(ADSR_SEG_RELEASE, Pluck_Amp_ADSR[3]);                              // Set release time
    AmpEnv_Bass.SetTime(ADSR_SEG_RELEASE, Bass_Amp_ADSR[3]);                              // Set release time
    AmpEnv_Pad.SetTime(ADSR_SEG_RELEASE, Pad_Amp_ADSR[3]);                              // Set release time
    
//-----------------------------------------------------------------------------------Inizializza FILTER e parametri
    filter_Pluck.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Bass.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pad.Init(hw.AudioSampleRate());                                        // Initialize filter

    filter_Pluck.SetFreq(ft_pluck);                                               // Set cutoff frequency
    filter_Bass.SetFreq(ft_bass);                                               // Set cutoff frequency
    filter_Pad.SetFreq(ft_pad);                                               // Set cutoff frequency

    filter_Pluck.SetRes(res_pluck);                                             // Set resonance
    filter_Bass.SetRes(res_bass);                                             // Set resonance
    filter_Pad.SetRes(res_pad);                                             // Set resonance

    filter_Pluck.SetDrive(drive_pluck);                                               // Set filter amplitude
    filter_Bass.SetDrive(drive_bass);                                               // Set filter amplitude
    filter_Pad.SetDrive(drive_pad);                                               // Set filter amplitude

//-----------------------------------------------------------------------------------Inizializza FILTER ADSR e parametri
    FilterEnv_Pluck.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Bass.Init(hw.AudioSampleRate());                                   // Initialize envelope
    FilterEnv_Pad.Init(hw.AudioSampleRate());                                   // Initialize envelope

    FilterEnv_Pluck.SetTime(ADSR_SEG_ATTACK, Pluck_Filter_ADSR[0]);                              // Set attack time
    FilterEnv_Bass.SetTime(ADSR_SEG_ATTACK, Bass_Filter_ADSR[0]);                              // Set attack time
    FilterEnv_Pad.SetTime(ADSR_SEG_ATTACK, Pad_Filter_ADSR[0]);                              // Set attack time

    FilterEnv_Pluck.SetTime(ADSR_SEG_DECAY, Pluck_Filter_ADSR[1]);                               // Set decay time
    FilterEnv_Bass.SetTime(ADSR_SEG_DECAY, Bass_Filter_ADSR[1]);                               // Set decay time
    FilterEnv_Pad.SetTime(ADSR_SEG_DECAY, Pad_Filter_ADSR[1]);                               // Set decay time

    FilterEnv_Pluck.SetSustainLevel(Pluck_Filter_ADSR[2]);                                      // Set sustain level
    FilterEnv_Bass.SetSustainLevel(Bass_Filter_ADSR[2]);                                      // Set sustain level
    FilterEnv_Pad.SetSustainLevel(Pad_Filter_ADSR[2]);                                      // Set sustain level

    FilterEnv_Pluck.SetTime(ADSR_SEG_RELEASE, Pluck_Filter_ADSR[3]);                          // Set release time
    FilterEnv_Bass.SetTime(ADSR_SEG_RELEASE, Bass_Filter_ADSR[3]);                          // Set release time
    FilterEnv_Pad.SetTime(ADSR_SEG_RELEASE, Pad_Filter_ADSR[3]);                          // Set release time
//--------
    //delay.Init(hw.AudioSampleRate());                                       // Inizializza il delay con il sample rate corrente
    
    //delay.setDelayTime(1000);      // Imposta il tempo di delay
    //delay.setFeedback(0.5f);        // Imposta il feedback
    //delay.setMix(0.5f);                  // Imposta il mix
    //delay.setDepth(0.5f);              // Imposta la profondità del pitch shifter
    
    Pin pin_rx = {PORTB, 7};
	Pin pin_tx = {PORTB, 6};
	config.baudrate = 9600 ;
 	config.periph   = UartHandler::Config::Peripheral::USART_1;
	config.stopbits      = UartHandler::Config::StopBits::BITS_1;
	config.parity        = UartHandler::Config::Parity::NONE;
	config.mode          = UartHandler::Config::Mode::RX;
	config.wordlength    = UartHandler::Config::WordLength::BITS_8;
	config.pin_config.rx = pin_rx;//{DSY_GPIOB, 7};  // (USART_1 RX) Daisy pin 15
	config.pin_config.tx = pin_tx;//{DSY_GPIOB, 6};  // (USART_1 TX) Daisy pin 14
	uart.Init(config);
	hw.StartLog(false);
	uint8_t start_bit = 'S';
	while (start_bit != 'I') // Wait for the start bit
	{ 
		uart.BlockingReceive(&start_bit, sizeof(start_bit), 100);
	}
    
    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {
        getData(); // Call the function to get data from the UART
		for ( int i = 0; i < 3; i++){
			hw.PrintLine (" penta %d : %d",i, penta[i].getRaw());
		}
		for ( int i = 0; i < 4; i++){
			hw.PrintLine ( "effect %d gate %f",i, effect[i].getGate());
			hw.PrintLine ( "effect %d value %f",i, effect[i].getValue());
		}
        Gate_Pluck = penta[0].getGate(); // Get the gate value for Pluck
        Gate_Bass = penta[1].getGate(); // Get the gate value for Bass
        Gate_Pad = penta[2].getGate(); // Get the gate value for Pad    

        knob1 = (float) penta[0].getRaw()/65535.0f;
        knob2 = (float) penta[1].getRaw()/65535.0f;
        knob3 = (float) penta[2].getRaw()/65535.0f;
        knob4 = (float) effect[0].getValue()/65535.0f;
        knob5 = (float) effect[1].getValue()/65535.0f;
        knob6 = (float) effect[2].getValue()/65535.0f;
        knob7 = (float) effect[3].getValue()/65535.0f;

        now_f_lfo_Bass = knob1*f_lfo_Bass; // Frequency of LFO for Bass
        now_f_lfo_Pluck = knob5*f_lfo_Bass; // Frequency of LFO for Pluck
        now_f_lfo_Pad = knob3*f_lfo_Pad; // Frequency of LFO for Pad

        now_f_Pluck = f_Pluck+ knob6*f_Pluck; // Frequency of Pluck
        now_f_Bass = f_Bass+ knob2*f_Bass; // Frequency of Bass
        now_f_Pad = f_Pad+ knob4*f_Pad; // Frequency of Pad

        //Gate_Pluck = ..
        //Gate_Bass = ..
        //Gate_Pad = ..

        //time =..
        //delayTime =logscale(time, MIN_DELAY, MAX_DELAY); // Imposta il tempo di delay
        

        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
