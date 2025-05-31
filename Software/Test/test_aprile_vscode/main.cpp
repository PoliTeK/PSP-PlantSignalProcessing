#include "../../libs/libDaisy/src/daisy_seed.h"  
#include "../../libs/DaisySP/Source/daisysp.h"
#include "pentaPianta.h"
#include "AnalogDelay.h"
#include "config.h"

#define DEBUG

#ifndef _BV  //used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit)) 
#endif
using namespace daisy;
using namespace daisysp;
//--OLD ARDUINO: Adafruit_MPR121 cap = Adafruit_MPR121();
// creates config for mpr121 (constructor sets all to deafult)
daisy::Mpr121I2C::Config mpr121ObjConf;
// creates object for mpr121
daisy::Mpr121I2C cap;

int soglie[8] = {2,0,65, 70, 80, 90,100, 105};
pentaPianta p_pluck(soglie,f_pluck), p_pad(soglie,f_pad), p_bass(soglie,f_bass);

// DaisyHardware hw;
DaisySeed hw;
//Pluck
Oscillator pluck, pad, bass;
Oscillator pluck_modulator;
Adsr amp_eg_pluck, amp_eg_pad, amp_eg_bass;
Adsr filt_eg_pluck, filt_eg_pad, filt_eg_bass;
// MoogLadder VCF_pluck, VCF_pad, VCF_bass; HOPE LADDER IS THE SAME AS MOOGLADDER
LadderFilter VCF_pluck, VCF_pad, VCF_bass;

uint16_t value_pluck, value_pad, value_bass;
float now_f_pluck, now_f_pad, now_f_bass;
bool Gate_pluck, Gate_pad, Gate_bass;

AnalogDelay Delay;

float out_mix;

static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
  pluck_modulator.SetFreq(now_f_pluck * ratio);
    
  for (size_t i = 0; i < size; i+=2) {

    float amp_env_pluck = amp_eg_pluck.Process(Gate_pluck);                 //crea inviluppo ampiezza pluck
    float amp_env_pad = amp_eg_pad.Process(Gate_pad);
    float amp_env_bass = amp_eg_bass.Process(Gate_bass); // Process amplitude envelope for bass

    float filt_env_pluck = filt_eg_pluck.Process(Gate_pluck);
    float filt_env_pad = filt_eg_pad.Process(Gate_pad);                     //crea inviluppo ampiezza pad
    float filt_env_bass = filt_eg_bass.Process(Gate_bass); // Process filter envelope for bass

                      
    float raw_pluck = pluck.Process()*pluck_modulator.Process()*5;                                    //ho il suono del  pluck
    float raw_pad = pad.Process();                                        // ho il suono del pad
    float raw_bass = bass.Process(); // Process raw sound for bass

    VCF_pluck.SetFreq(ft_pluck - filt_env_pluck*f_EG_pluck);       //applico l'inviluppo al filtro del pluck
    VCF_pad.SetFreq(ft_pad + filt_env_pad*f_EG_pad);               //applico l'inviluppo al filtro del pad
    VCF_bass.SetFreq(ft_bass + filt_env_bass * f_EG_bass); // Apply filter envelope to bass

    float filt_pluck =VCF_pluck.Process(raw_pluck)* amp_env_pluck;       //filtro e applico inviluppo ampiezza al pluck
    float filt_pad =VCF_pad.Process(raw_pad)* amp_env_pad;               //filtro e applico inviluppo ampiezza al pad
    float filt_bass = VCF_bass.Process(raw_bass) * amp_env_bass; // Filter and apply amplitude envelope to bass

    raw_pluck *= amp_env_pluck;                      //non filtro e applico inviluppo
    raw_pad *= amp_env_pad;                      //non filtro e applico inviluppo


    float delay_pluck = Delay.Process(filt_pluck);
    // float out_mix = (delay_pluck + filt_pad + filt_bass) * master;
    out_mix = (delay_pluck + filt_pad + filt_bass) * master; 
    
    out[i] = out[i + 1] = out_mix;
  }
}


int main () {

  // hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
   float sample_rate;
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();
    

    #ifdef DEBUG
      hw.StartLog(true);
    #endif // DEBUG
//-----------------------------------------------------------PLUCK
//OSC
  pluck.Init(sample_rate);
  pluck.SetWaveform(pluck.WAVE_TRI);
  pluck.SetFreq(f_pluck);
  pluck.SetAmp(mix_amp_pluck);

//amp_envelope
  amp_eg_pluck.Init(sample_rate);

  amp_eg_pluck.SetTime(ADSR_SEG_ATTACK, pluck_Amp_ADSR[0]);
  amp_eg_pluck.SetTime(ADSR_SEG_DECAY, pluck_Amp_ADSR[1]);  
  amp_eg_pluck.SetSustainLevel(pluck_Amp_ADSR[2]);
  amp_eg_pluck.SetTime(ADSR_SEG_RELEASE, pluck_Amp_ADSR[3]); 


//filter_envelope
  filt_eg_pluck.Init(sample_rate);

  filt_eg_pluck.SetTime(ADSR_SEG_ATTACK, pluck_Filter_ADSR[0]);
  filt_eg_pluck.SetTime(ADSR_SEG_DECAY, pluck_Filter_ADSR[1]);  
  filt_eg_pluck.SetSustainLevel(pluck_Filter_ADSR[2]);
  filt_eg_pluck.SetTime(ADSR_SEG_RELEASE, pluck_Filter_ADSR[3]); 


//VCF
  VCF_pluck.Init(sample_rate);
  VCF_pluck.SetFreq(ft_pluck); 
  VCF_pluck.SetRes(res_pluck); 
  
  pluck_modulator.Init(sample_rate);
  pluck_modulator.SetWaveform(pluck_modulator.WAVE_SIN);
  
  pluck.SetAmp(mix_amp_pluck_modulator);


//-----------------------------------------------------------PAD 

//OSC

  pad.Init(sample_rate);
  pad.SetWaveform(pad.WAVE_SAW);
  pad.SetFreq(f_pad);
  pad.SetAmp(mix_amp_pad);

//amp_envelope
  amp_eg_pad.Init(sample_rate);

  amp_eg_pad.SetTime(ADSR_SEG_ATTACK, pad_Amp_ADSR[0]);
  amp_eg_pad.SetTime(ADSR_SEG_DECAY, pad_Amp_ADSR[1]);  
  amp_eg_pad.SetSustainLevel(pad_Amp_ADSR[2]);
  amp_eg_pad.SetTime(ADSR_SEG_RELEASE, pad_Amp_ADSR[3]); 

//filter_envelope
  filt_eg_pad.Init(sample_rate);

  filt_eg_pad.SetTime(ADSR_SEG_ATTACK, pad_Filter_ADSR[0]);
  filt_eg_pad.SetTime(ADSR_SEG_DECAY, pad_Filter_ADSR[1]);  
  filt_eg_pad.SetSustainLevel(pad_Filter_ADSR[2]);
  filt_eg_pad.SetTime(ADSR_SEG_RELEASE, pad_Filter_ADSR[3]); 


//VCF
  VCF_pad.Init(sample_rate);
  VCF_pad.SetFreq(ft_pad); 
  VCF_pad.SetRes(res_pad); 

//-----------------------------------------------------------BASS
  // OSC
  bass.Init(sample_rate);
  bass.SetWaveform(bass.WAVE_SQUARE);
  bass.SetFreq(f_bass);
  bass.SetAmp(mix_amp_bass);

  // amp_envelope
  amp_eg_bass.Init(sample_rate);
  amp_eg_bass.SetTime(ADSR_SEG_ATTACK, bass_Amp_ADSR[0]);
  amp_eg_bass.SetTime(ADSR_SEG_DECAY, bass_Amp_ADSR[1]);
  amp_eg_bass.SetSustainLevel(bass_Amp_ADSR[2]);
  amp_eg_bass.SetTime(ADSR_SEG_RELEASE, bass_Amp_ADSR[3]);

  // filter_envelope
  filt_eg_bass.Init(sample_rate);
  filt_eg_bass.SetTime(ADSR_SEG_ATTACK, bass_Filter_ADSR[0]);
  filt_eg_bass.SetTime(ADSR_SEG_DECAY, bass_Filter_ADSR[1]);
  filt_eg_bass.SetSustainLevel(bass_Filter_ADSR[2]);
  filt_eg_bass.SetTime(ADSR_SEG_RELEASE, bass_Filter_ADSR[3]);

  // VCF
  VCF_bass.Init(sample_rate);
  VCF_bass.SetFreq(ft_bass);
  VCF_bass.SetRes(res_bass);

  // DELAY
  Delay.Init(sample_rate);
  Delay.setDelayTime(delay_time);
  Delay.setFeedback(delay_fb);
  Delay.setMix(delay_mix);

  

  hw.StartAudio(AudioCallback);
  
    #ifdef DEBUG
      
    
  // Serial.begin(9600);

  //while (!Serial) { // needed to keep micro from starting too fast!
  //  delay(10);
  //}
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK ) {
    hw.PrintLine("MPR121 not found, check wiring?");
    while (1);
  }
  hw.PrintLine("MPR121 found!");  
  // cap.SetThresholds(2, 0);
#endif // DEBUG
while (1){

//Pluck
  if(cap.Touched() & _BV(0)){
    Gate_pluck = true;
    value_pluck =cap.FilteredData(0);
    now_f_pluck = p_pluck.discFreq(value_pluck);
    pluck.SetFreq(now_f_pluck);                                       //setto frequenza pluck
  }
  else {
    Gate_pluck = false;
  }

  if(Gate_pluck){
    hw.PrintLine("Hai premuto il gate pluck (0) ");
    hw.PrintLine(" ed il suo valore è : ");
    hw.PrintLine("%d", (int)value_pluck);
    hw.PrintLine("Frequenza: ");
    hw.PrintLine("%f", now_f_pluck); // DA AGGIUNGERE FLAG NEL MAKEFILE

  }
//Pad
  if(cap.Touched() & _BV(1)){
    Gate_pad = true;
    value_pad =cap.FilteredData(1);
    now_f_pad = p_pad.discFreq(value_pad);
    pad.SetFreq(now_f_pad); 
  }
  else {
    Gate_pad = false;
  }

  if(Gate_pad){
    hw.PrintLine("Hai premuto il gate pad (1) ");
    hw.PrintLine(" ed il suo valore è : ");
    hw.PrintLine("%d", (int)value_pad);
    hw.PrintLine("Frequenza: ");
    // hw.PrintLine("%f", now_f_pad); // DA AGGIUNGERE FLAG NEL MAKEFILE
  }

  // Pluck 3
  if (cap.Touched() & _BV(2)) {
    Gate_bass = true;
    value_bass = cap.FilteredData(2);
    now_f_bass = p_bass.discFreq(value_bass);
    bass.SetFreq(now_f_bass); // Set frequency for bass
  } else {
    Gate_bass = false;
  }

 

  if (Gate_bass) {
    hw.PrintLine("Hai premuto il gate bass (2) ");
    hw.PrintLine(" ed il suo valore è : ");
    hw.PrintLine("%d", (int)value_bass);
    hw.PrintLine("Frequenza: ");
    // hw.PrintLine("%f", now_f_bass); // DA AGGIUNGERE FLAG NEL MAKEFILE
  }
  hw.PrintLine("end of loop, %f", out_mix);
  hw.DelayMs(100);
  }
}