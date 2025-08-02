//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                               Pluck Synth Configuration
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Parametri Oscillatori, AmpEG, FilterEG per il pluck
float f_pluck = 440.0f;                // Frequenza dell'oscillatore pluck

uint8_t wf_pluck[2] = {1,1};           // Array for waveforms

// parametri shape pluck
float shape_pluck = 0.5f;              // Shape modulation for pluck





// Parametri del filtro e Gate
float ft_pluck = 3500.0f;              // Frequenza di taglio del filtro pluck
float res_pluck = 0.1f;                // Resistenza del filtro pluck


// Frequenza dell'inviluppo del filtro
float f_EG_pluck = 1000.0f;           // Frequenza dell'inviluppo del filtro

// Parametri ADSR per il pluck (Ampiezza e Filtro)
float pluck_Amp_ADSR[4] = {0.005f, 0.1f, 0.0f, 0.5f};    // Attack, Decay, Sustain, Release
float pluck_Filter_ADSR[4] = {0.05f, 0.03f, 0.0f, 0.5f};  // Attack, Decay, Sustain, Release

// Ampiezza di mixaggio per il pluck
float mix_amp_pluck[2] = {1,1};
float gain_pluck = 1.0f; // Gain for pluck


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                               Pad Synth Configuration
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Parametri Oscillatori, AmpEG, FilterEG per il pad
float f_pad = 220.0f;                  // Frequenza dell'oscillatore pad

// parametri shape pluck
float shape_pad = 0.5f;              



// Parametri dell'LFO per il pad
float amp_lfo_pad = 0.3f;              // Ampiezza dell'LFO

uint8_t wf_pad[2] = {1, 1}; 

// Parametri del filtro e Gate
float ft_pad = 500.0f;                 // Frequenza di taglio del filtro pad
float res_pad = 0.1f;                  // Resistenza del filtro pad
float drive_pad = 0.5f;                // Drive del filtro pad

// Frequenza dell'inviluppo del filtro
float f_EG_pad = 8000.0f;              // Frequenza dell'inviluppo del filtro per il pad

// Parametri ADSR per il pad (Ampiezza e Filtro)
float pad_Amp_ADSR[4] = {0.05f, 0.27f, 0.4f, 0.5f};    // Attack, Decay, Sustain, Release
float pad_Filter_ADSR[4] = {0.1f, 0.1f, 0.1f, 0.5f};  // Attack, Decay, Sustain, Release

// Ampiezza di mixaggio per il pad
float mix_amp_pad[2] = {0.5, 0.5};
float gain_pad = 1.0f; // Gain for pad

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                              Bass Synth Configuration
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Parametri Oscillatori, AmpEG, FilterEG per il bass
float f_bass = 55.0f;                  // Frequenza dell'oscillatore bass

uint8_t wf_bass[2] = {1,1}; 

// parametri shape bass
float shape_bass = 0.5f;              // Shape modulation for pluck

// Parametri dell'LFO per il bass
float f_lfo_bass = 10.0f;              // Frequenza dell'LFO
float amp_lfo_bass = 0.3f;             // Ampiezza dell'LFO

// Parametri del filtro e Gate
float ft_bass = 200.0f;                // Frequenza di taglio del filtro bass
float res_bass = 0.1f;                 // Resistenza del filtro bass
float drive_bass = 0.5f;               // Drive del filtro bass

// Frequenza dell'inviluppo del filtro
float f_EG_bass = 5000.0f;             // Frequenza dell'inviluppo del filtro per il bass

// Parametri ADSR per il bass (Ampiezza e Filtro)
float bass_Amp_ADSR[4] = {0.15f, 0.5f, 0.75f, 0.33f};   // Attack, Decay, Sustain, Release
float bass_Filter_ADSR[4] = {10.0f, 1.0f, 0.6f, 0.5f};  // Attack, Decay, Sustain, Release

// Ampiezza di mixaggio per il bass
float mix_amp_bass[2] = {0.5, 0.5};
float gain_bass = 1.0f; // Gain for bass

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                                     Master Control
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Parametro master per il mixaggio globale
float master = 0.5f;

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                                          Delay Settings
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


float delay_time = 0.5;
float delay_fb = 0.2;
float delay_mix = 0.3;

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//                                           LFO Settings
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

float lfo1_freq = 0.5f; 
float lfo2_freq = 0.5f;  
float lfo3_freq = 0.5f; 

float lfo1_amp = 0.5f;  
float lfo2_amp = 0.5f;  
float lfo3_amp = 0.5f;  

float lfo1_phase = 0.0f;
float lfo2_phase = 0.0f; 
float lfo3_phase = 0.0f; 

float lfo1_shape = 0.5f;
float lfo2_shape = 0.5f; 
float lfo3_shape = 0.5f;

