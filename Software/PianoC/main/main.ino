// Title: oscillator
// Description: Control a sine wave freq with a knob
// Hardware: Daisy Seed
// Author: Ben Sergentanis
// Breadboard
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_bb.png
// Schematic:
// https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/seed/Osc/resources/Osc_schem.png

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator Pluck1, Pluck2, Pluck3, Pluck4,;
static VariableShapeOscillator  Pad5, Pad6, Bass7, Bass8;

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



void MyCallback(float **in, float **out, size_t size) {
  // Convert Pitchknob MIDI Note Number to frequency

  for (size_t i = 0; i < size; i++) {
  

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

    Pluck1.Init(sample_rate);
    Pluck2.Init(sample_rate);                                            // Initialize the Ramo synth
    Pluck3.Init(sample_rate);                                            // Initialize the Ramo synth
    Pluck4.Init(sample_rate));                                            // Initialize the Ramo synth
    Bass1.Init(sample_rate);                                            // Initialize the Ramo synth
    Bass2.Init(sample_rate);                                            // Initialize the Ramo synth
    Pad1.Init(sample_rate);                                            // Initialize the Ramo synth
    Pad2.Init(sample_rate);                                            // Initialize the Ramo synth                                        // Initialize the Ramo synth

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
    lfo_Pluck1.Init(sample_rate);                                      
    lfo_Pluck2.Init(sample_rate);                                          // Initialize LFO
    lfo_Pluck3.Init(sample_rate);                                          // Initialize LFO
    lfo_Pluck4.Init(sample_rate);                                          // Initialize LFO
    lfo_Bass1.Init(sample_rate);                                          // Initialize LFO
    lfo_Bass2.Init(sample_rate);                                          // Initialize LFO
    lfo_Pad1.Init(sample_rate);                                          // Initialize LFO
    lfo_Pad2.Init(sample_rate);                                          // Initialize LFO

   

    lfo_Pluck1.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck2.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck3.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Pluck4.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass1.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Bass2.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad1.SetWaveform(WAVE_SIN);                                 // Set LFO waveform
    lfo_Pad2.SetWaveform(WAVE_SIN);                                 // Set LFO waveform

    

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
    AmpEnv_Pluck.Init(sample_rate);                                       // Initialize envelope
    AmpEnv_Bass.Init(sample_rate);                                       // Initialize envelope
    AmpEnv_Pad.Init(sample_rate);                                       // Initialize envelope

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
    filter_Pluck1.Init(sample_rate);                                        // Initialize filter
    filter_Pluck2.Init(sample_rate);                                        // Initialize filter
    filter_Pluck3.Init(sample_rate);                                        // Initialize filter
    filter_Pluck4.Init(sample_rate);                                        // Initialize filter
    filter_Bass1.Init(sample_rate);                                        // Initialize filter
    filter_Bass2.Init(sample_rate);                                        // Initialize filter
    filter_Pad1.Init(sample_rate);                                        // Initialize filter
    filter_Pad2.Init(sample_rate);                                        // Initialize filter

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
    FilterEnv_Pluck.Init(sample_rate);                                   // Initialize envelope
    FilterEnv_Bass.Init(sample_rate);                                   // Initialize envelope
    FilterEnv_Pad.Init(sample_rate);                                   // Initialize envelope

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



  




  DAISY.begin(MyCallback);
}

void loop() { 
  
 }
