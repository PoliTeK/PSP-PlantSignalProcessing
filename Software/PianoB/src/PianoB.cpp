// Inclusion of necessary libraries
#include "daisy_seed.h" // Main library for the Daisy Seed board
#include "daisysp.h"    // DSP library for Daisy
#include "Ramo.h"       // Library for the Ramo synth
#include "config1.h"


//#include "AnalogDelay.h" // Libreria custom per l'effetto delay analogico



// Using namespaces to simplify code writing
using namespace daisy;
using namespace daisysp;


static DaisySeed hw;                                                // Main object for managing Daisy hardware



static Ramo Pluck1, Pluck2, Pluck3, Pluck4;                                                  // Object for the Ramo synth
static Ramo Bass1, Bass2;
static Ramo Pad1, Pad2;                                                    // Object for the Ramo synth
 
static Oscillator lfo_Pluck1, lfo_Pluck2, lfo_Pluck3, lfo_Pluck4;                                 // LFO object for modulation
static Oscillator lfo_Bass1, lfo_Bass2;
static Oscillator lfo_Pad1, lfo_Pad2;                                          // LFO object for modulation

static Adsr AmpEnv_Pluck;                                          // Envelope object for amplitude
static Adsr AmpEnv_Bass;
static Adsr AmpEnv_Pad;                                            // Envelope object for amplitude

static Adsr FilterEnv_Pluck ;                                       
static Adsr FilterEnv_Bass;
static Adsr FilterEnv_Pad;                                        // Envelope object for filter

static Svf filter_Pluck1, filter_Pluck2, filter_Pluck3, filter_Pluck4;                                 // Filter object for the synth
static Svf filter_Bass1, filter_Bass2;
static Svf filter_Pad1, filter_Pad2;                                          // Filter object for the synth;                                          



float Raw_Pluck1, Raw_Pluck2, Raw_Pluck3, Raw_Pluck4;                                   // Raw signal variables
float Raw_Bass1, Raw_Bass2;                                   // Raw signal variables
float Raw_Pad1, Raw_Pad2;                            // Raw signal variables
float out_noFX;                                                       // Output signal variable
                                                     




float now_f_lfo_Bass1, now_f_lfo_Bass2; // Frequency of LFO for Bass;
float now_f_lfo_Pluck1, now_f_lfo_Pluck2, now_f_lfo_Pluck3, now_f_lfo_Pluck4; // Frequency of LFO for Pluck
float now_f_lfo_Pad1, now_f_lfo_Pad2; // Frequency of LFO for Pad;  

float now_f_Pluck1, now_f_Pluck2, now_f_Pluck3, now_f_Pluck4; // Frequency of Pluck
float now_f_Bass1, now_f_Bass2; // Frequency of Bass;
float now_f_Pad1, now_f_Pad2; // Frequency of Pad;

float shape_Pluck1, shape_Pluck2, shape_Pluck3, shape_Pluck4; // Shape modulation for Pluck
float shape_Bass1, shape_Bass2; // Shape modulation for Bass
float shape_Pad1, shape_Pad2; // Shape modulation for Pad

float amp_env_Pluck1, amp_env_Pluck2, amp_env_Pluck3, amp_env_Pluck4; // Amplitude envelope for Pluck
float amp_env_Bass1, amp_env_Bass2; // Amplitude envelope for Bass
float amp_env_Pad1, amp_env_Pad2; // Amplitude envelope for Pad

float filt_env_Pluck1, filt_env_Pluck2, filt_env_Pluck3, filt_env_Pluck4; // Filter envelope for Pluck
float filt_env_Bass1, filt_env_Bass2; // Filter envelope for Bass
float filt_env_Pad1, filt_env_Pad2; // Filter envelope for Pad



float Gate[8]; // Gate variables for the synths

float knob[8]; 

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {

                                                    
    for (size_t i = 0; i < size; i++) {
//--------------------------------------------------------------------Setta DelayTime
    //delay.setDelayTime(delayTime);      // Imposta il tempo di delay
//-------------------------------------------------------------------Setta Shape
    lfo_Pluck1.SetFreq(now_f_lfo_Pluck1);
    lfo_Pluck2.SetFreq(now_f_lfo_Pluck2);
    lfo_Pluck3.SetFreq(now_f_lfo_Pluck3);
    lfo_Pluck4.SetFreq(now_f_lfo_Pluck4);
    lfo_Bass1.SetFreq(now_f_lfo_Bass1);
    lfo_Bass2.SetFreq(now_f_lfo_Bass2);
    lfo_Pad1.SetFreq(now_f_lfo_Pad1);
    lfo_Pad2.SetFreq(now_f_lfo_Pad2);


    shape_Pluck1 = lfo_Pluck1.Process();       
    shape_Pluck2 = lfo_Pluck2.Process();
    shape_Pluck3 = lfo_Pluck3.Process();
    shape_Pluck4 = lfo_Pluck4.Process();
    shape_Bass1 = lfo_Bass1.Process();
    shape_Bass2 = lfo_Bass2.Process();
    shape_Pad1 = lfo_Pad1.Process();
    shape_Pad2 = lfo_Pad2.Process();

                                

    Pluck1.SetShape(shape_Pluck1);   
    Pluck2.SetShape(shape_Pluck2);
    Pluck3.SetShape(shape_Pluck3);
    Pluck4.SetShape(shape_Pluck4);
    Bass1.SetShape(shape_Bass1);
    Bass2.SetShape(shape_Bass2);
    Pad1.SetShape(shape_Pad1);
    Pad2.SetShape(shape_Pad2);

                                    
    
//---------------------------------------------------------------Setta Freq
    Pluck1.SetFreq(now_f_Pluck1);   
    Pluck2.SetFreq(now_f_Pluck2);
    Pluck3.SetFreq(now_f_Pluck3);
    Pluck4.SetFreq(now_f_Pluck4);
    Bass1.SetFreq(now_f_Bass1);
    Bass2.SetFreq(now_f_Bass2);
    Pad1.SetFreq(now_f_Pad1);
    Pad2.SetFreq(now_f_Pad2);                                     
                  

//--------------------------------------------------------------------------------------------------Setta Inviluppo Amplitude
    amp_env_Pluck1 = AmpEnv_Pluck.Process(Gate[0]);  
    amp_env_Pluck2 = AmpEnv_Pluck.Process(Gate[1]);
    amp_env_Pluck3 = AmpEnv_Pluck.Process(Gate[2]);
    amp_env_Pluck4 = AmpEnv_Pluck.Process(Gate[3]);
    amp_env_Pad1 = AmpEnv_Pad.Process(Gate[4]);
    amp_env_Pad2 = AmpEnv_Pad.Process(Gate[5]);
    amp_env_Bass1 = AmpEnv_Bass.Process(Gate[6]);
    amp_env_Bass2 = AmpEnv_Bass.Process(Gate[7]);
                      

//--------------------------------------------------------------------------------------------------Setta iinviluppo filtro
    filt_env_Pluck1 = FilterEnv_Pluck.Process(Gate[0]);  
    filt_env_Pluck2 = FilterEnv_Pluck.Process(Gate[1]);
    filt_env_Pluck3 = FilterEnv_Pluck.Process(Gate[2]);
    filt_env_Pluck4 = FilterEnv_Pluck.Process(Gate[3]);
    filt_env_Pad1 = FilterEnv_Pad.Process(Gate[4]);
    filt_env_Pad2 = FilterEnv_Pad.Process(Gate[5]);
    filt_env_Bass1 = FilterEnv_Bass.Process(Gate[6]);
    filt_env_Bass2 = FilterEnv_Bass.Process(Gate[7]);                               
//---------------------------------------------------------------------------------------------------Setta ft Filtri
    filter_Pluck1.SetFreq(ft_pluck - filt_env_Pluck1*f_EG_pluck);
    filter_Pluck2.SetFreq(ft_pluck - filt_env_Pluck2*f_EG_pluck);
    filter_Pluck3.SetFreq(ft_pluck - filt_env_Pluck3*f_EG_pluck);
    filter_Pluck4.SetFreq(ft_pluck - filt_env_Pluck4*f_EG_pluck);
    filter_Bass1.SetFreq(ft_bass + filt_env_Bass1*f_EG_bass);                            
    filter_Bass2.SetFreq(ft_bass + filt_env_Bass2*f_EG_bass);
    filter_Pad1.SetFreq(ft_pad + filt_env_Pad1*f_EG_pad);
    filter_Pad2.SetFreq(ft_pad + filt_env_Pad2*f_EG_pad);                               
//----------------------------------------------------------------------------------------------------Setta Amp 
    Pluck1.SetGain(amp_env_Pluck1); 
    Pluck2.SetGain(amp_env_Pluck2);
    Pluck3.SetGain(amp_env_Pluck3);
    Pluck4.SetGain(amp_env_Pluck4);
    Bass1.SetGain(amp_env_Bass1);
    Bass2.SetGain(amp_env_Bass2);
    Pad1.SetGain(amp_env_Pad1);
    Pad2.SetGain(amp_env_Pad2);                                    
                                     
//-----------------------------------------------------------------------------------------------------Processa rami
    Raw_Pluck1 = Pluck1.Process();
    Raw_Pluck2 = Pluck2.Process();
    Raw_Pluck3 = Pluck3.Process();
    Raw_Pluck4 = Pluck4.Process();
    Raw_Bass1 = Bass1.Process();
    Raw_Bass2 = Bass2.Process();
    Raw_Pad1 = Pad1.Process();
    Raw_Pad2 = Pad2.Process();
   
//------------------------------------------------------------------------------------------------------Filtra Rami
    filter_Pluck1.Process(Raw_Pluck1);
    filter_Pluck2.Process(Raw_Pluck2);
    filter_Pluck3.Process(Raw_Pluck3);
    filter_Pluck4.Process(Raw_Pluck4);
    filter_Bass1.Process(Raw_Bass1);
    filter_Bass2.Process(Raw_Bass2);
    filter_Pad1.Process(Raw_Pad1);
    filter_Pad2.Process(Raw_Pad2);

//------------------------------------------------------------------------------------------------------Somma tutto con guadagni
    out_noFX = (filter_Pluck1.Low() + filter_Pluck2.Low() + filter_Pluck3.Low() + filter_Pluck4.Low()) * mix_amp_Pluck + 
                + (filter_Pad1.Low() + filter_Pad2.Low())*mix_amp_Pad +
                + (filter_Bass1.Low() + filter_Bass2.Low())*mix_amp_Bass; 
//-------------------------------------------------------------------------------------------------------Applica effetto
    //float out_FX = delay.Process(out_noFX); // Process the output signal with the delay effect

//-------------------------------------------------------------------------------------------------------Output
        out[0][i] = out_noFX;                                         // Output processed signal (left channel)
        out[1][i] = out_noFX;                                         // Output processed signal (right channel)
    }
}





int main() {
    hw.Configure();                                                             // Hardware configuration
    hw.Init();                                                                  // Hardware initialization

    hw.StartLog (true); /* true == wait for PC: will block until a terminal is connected */
	hw.DelayMs(100);
    Pin pin_scl = {PORTB, 8};
	Pin pin_sda = {PORTB, 9};
	I2CHandle::Config i2c_conf;
	i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
	i2c_conf.speed  = I2CHandle::Config::Speed::I2C_400KHZ;
	i2c_conf.mode   = I2CHandle::Config::Mode::I2C_MASTER;
	i2c_conf.pin_config.scl  = pin_scl; // D11
	i2c_conf.pin_config.sda  = pin_sda; // D12
	// initialise the peripheral
	I2CHandle i2c;
	i2c.Init(i2c_conf);
    daisy::Mpr121I2C mpr121Obj;
    daisy::Mpr121I2C::Config mpr121ObjConf;
    mpr121Obj.Init(mpr121ObjConf);


    hw.SetAudioBlockSize(4);                                                    // Set audio block size
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);            // Set sample rate to 48kHz
//------------------------------------------------------------------------------Inizializza Ogetti audio e parametri
    Pluck1.Init(hw.AudioSampleRate());
    Pluck2.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pluck3.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pluck4.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Bass1.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Bass2.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pad1.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth
    Pad2.Init(hw.AudioSampleRate());                                            // Initialize the Ramo synth                                        // Initialize the Ramo synth

    Pluck1.SetWaveforms(wf_Pluck);                                                      
    Pluck2.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Pluck3.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Pluck4.SetWaveforms(wf_Pluck);                                                      // Set waveforms
    Bass1.SetWaveforms(wf_Bass);                                                      // Set waveforms
    Bass2.SetWaveforms(wf_Bass);                                                      // Set waveforms
    Pad1.SetWaveforms(wf_Pad);                                                      // Set waveforms
    Pad2.SetWaveforms(wf_Pad);                                                      // Set waveforms

                                            
    Pluck1.SetFreq(f_Pluck);                                                    
    Pluck2.SetFreq(f_Pluck);                                                       // Set frequency
    Pluck3.SetFreq(f_Pluck);                                                       // Set frequency
    Pluck4.SetFreq(f_Pluck);                                                       // Set frequency
    Bass1.SetFreq(f_Bass);                                                       // Set frequency
    Bass2.SetFreq(f_Bass);                                                       // Set frequency
    Pad1.SetFreq(f_Pad);                                                       // Set frequency
    Pad2.SetFreq(f_Pad);                                                       // Set frequency

                                              

    Pluck1.SetAmp(g_Pluck);                             
    Pluck2.SetAmp(g_Pluck);                                                       // Set amplitude
    Pluck3.SetAmp(g_Pluck);                                                       // Set amplitude
    Pluck4.SetAmp(g_Pluck);
    Bass1.SetAmp(g_Bass);                                                       // Set amplitude
    Bass2.SetAmp(g_Bass);                                                       // Set amplitude
    Pad1.SetAmp(g_Pad);                                                       // Set amplitude
    Pad2.SetAmp(g_Pad);                                                       // Set amplitude


    Pluck1.SetShape(0);                                                        // Set shape modulation
    Pluck2.SetShape(0);
    Pluck3.SetShape(0);
    Pluck4.SetShape(0);
    Bass1.SetShape(0);
    Bass2.SetShape(0);
    Pad1.SetShape(0);
    Pad2.SetShape(0);

//----------------------------------------------------------------------------------Inizializza LFO e parametri
    lfo_Pluck1.Init(hw.AudioSampleRate());                                      
    lfo_Pluck2.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pluck3.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pluck4.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Bass1.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Bass2.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pad1.Init(hw.AudioSampleRate());                                          // Initialize LFO
    lfo_Pad2.Init(hw.AudioSampleRate());                                          // Initialize LFO

   

    lfo_Pluck1.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck2.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck3.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck4.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass1.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass2.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad1.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad2.SetWaveform(Oscillator::WAVE_SIN);                                 // Set LFO waveform

    

    lfo_Pluck1.SetFreq(f_lfo_Pluck);                                                    // Set LFO frequency
    lfo_Pluck2.SetFreq(f_lfo_Pluck);                                                    // Set LFO frequency
    lfo_Pluck3.SetFreq(f_lfo_Pluck);
    lfo_Pluck4.SetFreq(f_lfo_Pluck);                                                    // Set LFO frequency
    lfo_Bass1.SetFreq(f_lfo_Bass);
    lfo_Bass2.SetFreq(f_lfo_Bass);                                                    // Set LFO frequency
    lfo_Pad1.SetFreq(f_lfo_Pad);                                                    // Set LFO frequency
    lfo_Pad2.SetFreq(f_lfo_Pad);                                                    // Set LFO frequency
                                                 // Set LFO frequency

    lfo_Pluck1.SetAmp(amp_lfo_Pluck);                                                    // Set LFO amplitude
    lfo_Pluck2.SetAmp(amp_lfo_Pluck);                                                    // Set LFO amplitude
    lfo_Pluck3.SetAmp(amp_lfo_Pluck);                                                    // Set LFO amplitude
    lfo_Pluck4.SetAmp(amp_lfo_Pluck);                                                    // Set LFO amplitude
    lfo_Bass1.SetAmp(amp_lfo_Bass);                                                    // Set LFO amplitude
    lfo_Bass2.SetAmp(amp_lfo_Bass);                                                    // Set LFO amplitude
    lfo_Pad1.SetAmp(amp_lfo_Pad);                                                    // Set LFO amplitude
    lfo_Pad2.SetAmp(amp_lfo_Pad);                                                    // Set LFO amplitude


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
    filter_Pluck1.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pluck2.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pluck3.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pluck4.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Bass1.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Bass2.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pad1.Init(hw.AudioSampleRate());                                        // Initialize filter
    filter_Pad2.Init(hw.AudioSampleRate());                                        // Initialize filter

    filter_Pluck1.SetFreq(ft_pluck);                                               // Set cutoff frequency
    filter_Pluck2.SetFreq(ft_pluck);                                               // Set cutoff frequency
    filter_Bass1.SetFreq(ft_bass);                                               // Set cutoff frequency
    filter_Bass2.SetFreq(ft_bass);                                               // Set cutoff frequency
    filter_Pad1.SetFreq(ft_pad);                                               // Set cutoff frequency
    filter_Pad2.SetFreq(ft_pad);                                               // Set cutoff frequency

    filter_Pluck1.SetRes(res_pluck);                                             // Set resonance
    filter_Pluck2.SetRes(res_pluck);                                             // Set resonance
    filter_Pluck3.SetRes(res_pluck);                                             // Set resonance
    filter_Pluck4.SetRes(res_pluck);                                             // Set resonance
    filter_Bass1.SetRes(res_bass);                                             // Set resonance
    filter_Bass2.SetRes(res_bass);                                             // Set resonance
    filter_Pad1.SetRes(res_pad);                                             // Set resonance
    filter_Pad2.SetRes(res_pad);                                             // Set resonance

                                      

    filter_Pluck1.SetDrive(drive_pluck);                                               // Set filter amplitude
    filter_Pluck2.SetDrive(drive_pluck);                                               // Set filter amplitude
    filter_Pluck3.SetDrive(drive_pluck);                                               // Set filter amplitude
    filter_Pluck4.SetDrive(drive_pluck);                                               // Set filter amplitude
    filter_Bass1.SetDrive(drive_bass);                                               // Set filter amplitude
    filter_Bass2.SetDrive(drive_bass);                                               // Set filter amplitude
    filter_Pad1.SetDrive(drive_pad);                                               // Set filter amplitude
    filter_Pad2.SetDrive(drive_pad);                                               // Set filter amplitude

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
    
    
    hw.StartAudio(AudioCallback);                                               // Start the audio callback

    // Infinite loop to keep the program running
    while (1) {
        for (int i = 0; i < 8; i++) {
            Gate[i] = mpr121Obj.Touched() & (1 << i) ? true : false; // Get the gate value for each pad
            if (Gate[i]) knob[i] = rand()/32767; // Se tocco la pianta i-esima mi aggiorna frequenza e lfo i-esimi attraverso knob[i]
        }
        
        

        now_f_lfo_Pluck1 = knob[0]*f_lfo_Pluck; // Frequency of LFO for Pluck
        now_f_lfo_Pluck2 = knob[1]*f_lfo_Pluck; 
        now_f_lfo_Pluck3 = knob[2]*f_lfo_Pluck; 
        now_f_lfo_Pluck4 = knob[3]*f_lfo_Pluck; 
        now_f_lfo_Bass1 = knob[4]*f_lfo_Pad; // Frequency of LFO for Pad
        now_f_lfo_Bass2 = knob[5]*f_lfo_Pad; 
        now_f_lfo_Pad1 = knob[6]*f_lfo_Bass;  // Frequency of LFO for Bass
        now_f_lfo_Pad2 = knob[7]*f_lfo_Bass; 


        now_f_Pluck1 = f_Pluck+ knob[0]*f_Pluck; // Frequency of Pluck
        now_f_Pluck2 = f_Pluck+ knob[1]*f_Pluck; 
        now_f_Pluck3 = f_Pluck+ knob[2]*f_Pluck; 
        now_f_Pluck4 = f_Pluck+ knob[3]*f_Pluck; 
        now_f_Pad1 = f_Pad+ knob[4]*f_Pad; // Frequency of Pad
        now_f_Pad2 = f_Pad+ knob[5]*f_Pad; 
        now_f_Bass1 = f_Bass+ knob[6]*f_Bass; // Frequency of Bass
        now_f_Bass2 = f_Bass+ knob[7]*f_Bass; 


        // Wait 1ms to reduce CPU usage
        System::Delay(1);
    }
}
