#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "pentaPianta.h"
#include "AnalogDelay.h"
#include "config.h"
#include "Ramo.h"

#define DEBUG

#ifndef _BV // used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit))
#endif
using namespace daisy;
using namespace daisysp;
//--OLD ARDUINO: Adafruit_MPR121 cap = Adafruit_MPR121();
// creates config for mpr121 (constructor sets all to deafult)
daisy::Mpr121I2C::Config mpr121ObjConf;
// creates object for mpr121
daisy::Mpr121I2C cap;

int soglie[8] = {2, 0, 65, 70, 80, 90, 100, 105};
pentaPianta p_pluck(soglie, f_pluck), p_pad(soglie, f_pad), p_bass(soglie, f_bass);

// DaisyHardware hw;
DaisySeed hw;
// Pluck
Ramo pluck, pad, bass;

Oscillator lfo1, lfo2, lfo3;

Adsr amp_eg_pluck, amp_eg_pad, amp_eg_bass;
Adsr filt_eg_pluck, filt_eg_pad, filt_eg_bass;
// MoogLadder VCF_pluck, VCF_pad, VCF_bass; HOPE LADDER IS THE SAME AS MOOGLADDER
static LadderFilter VCF_pluck, VCF_pad, VCF_bass;

uint16_t value_pluck, value_pad, value_bass;
float now_f_pluck, now_f_pad, now_f_bass;
bool Gate_pluck, Gate_pad, Gate_bass;

float filtered_pluck, filtered_pad, filtered_bass;

AnalogDelay Delay;

float out_mix;

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size)
{

  for (size_t i = 0; i < size; i += 2)
  {
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\SHAPE
    shape_pluck = lfo1.Process();
    shape_bass = lfo2.Process();
    shape_pad = lfo3.Process();

    pluck.SetShape(shape_pluck); // Set shape modulation for pluck
    bass.SetShape(shape_bass);   // Set shape modulation for bass
    pad.SetShape(shape_pad);     // Set shape modulation for pad
                                 //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\FREQ
    pluck.SetFreq(now_f_pluck);
    bass.SetFreq(now_f_bass);
    pad.SetFreq(now_f_pad);

    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\AMP_EG
    float amp_env_pluck = amp_eg_pluck.Process(Gate_pluck); // crea inviluppo ampiezza pluck
    float amp_env_pad = amp_eg_pad.Process(Gate_pad);       // crea inviluppo ampiezza pad
    float amp_env_bass = amp_eg_bass.Process(Gate_bass);    // crea inviluppo ampiezza bass
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Filter_EG
    float filt_env_pluck = filt_eg_pluck.Process(Gate_pluck); // crea inviluppo filtro pluck
    float filt_env_pad = filt_eg_pad.Process(Gate_pad);       // crea inviluppo filtro pad
    float filt_env_bass = filt_eg_bass.Process(Gate_bass);    // crea inviluppo filtro bass

    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\VCF
    VCF_pluck.SetFreq(ft_pluck - filt_env_pluck * f_EG_pluck); // applico l'inviluppo al filtro del pluck
    VCF_pad.SetFreq(ft_pad + filt_env_pad * f_EG_pad);         // applico l'inviluppo al filtro del pad
    VCF_bass.SetFreq(ft_bass + filt_env_bass * f_EG_bass);     // applico l'inviluppo al filtro del bass
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\VCA
    pluck.SetGain(amp_env_pluck);
    pad.SetGain(amp_env_pad);
    bass.SetGain(amp_env_bass);
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\AUDIO PROCESSING
    float raw_pluck = pluck.Process();
    float raw_pad = pad.Process();
    float raw_bass = bass.Process();

    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\FILTERING
    filtered_pluck = VCF_pluck.Process(raw_pluck);
    filtered_pad = VCF_pad.Process(raw_pad);
    filtered_bass = VCF_bass.Process(raw_bass);
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\DELAY
    /*
        float delay_pluck = Delay.Process(filt_pluck);
        // float out_mix = (delay_pluck + filt_pad + filt_bass) * master;
        out_mix = (delay_pluck + filt_pad + filt_bass) * master;
    */
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\OUT

    out_mix = (filtered_pluck * gain_pluck + filtered_pad * gain_pad + filtered_bass * gain_bass) * master;

    out[i] = out[i + 1] = out_mix;
  }
}

int main()
{

  // hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(64);
  sample_rate = hw.AudioSampleRate();

#ifdef DEBUG
  hw.StartLog(true);
#endif // DEBUG
       //-----------------------------------------------------------PLUCK
       // OSC
  pluck.Init(sample_rate);
  pluck.SetWaveforms(wf_pluck);
  pluck.SetFreq(f_pluck);
  pluck.SetAmp(mix_amp_pluck);
  pluck.SetDetune(0);          // No detune for pluck
  pluck.SetGain(1.0f);         // Set gain to 1.0 for pluck
  pluck.SetShape(shape_pluck); // Set shape modulation for pluck

  // amp_envelope
  amp_eg_pluck.Init(sample_rate);

  amp_eg_pluck.SetTime(ADSR_SEG_ATTACK, pluck_Amp_ADSR[0]);
  amp_eg_pluck.SetTime(ADSR_SEG_DECAY, pluck_Amp_ADSR[1]);
  amp_eg_pluck.SetSustainLevel(pluck_Amp_ADSR[2]);
  amp_eg_pluck.SetTime(ADSR_SEG_RELEASE, pluck_Amp_ADSR[3]);

  // filter_envelope
  filt_eg_pluck.Init(sample_rate);

  filt_eg_pluck.SetTime(ADSR_SEG_ATTACK, pluck_Filter_ADSR[0]);
  filt_eg_pluck.SetTime(ADSR_SEG_DECAY, pluck_Filter_ADSR[1]);
  filt_eg_pluck.SetSustainLevel(pluck_Filter_ADSR[2]);
  filt_eg_pluck.SetTime(ADSR_SEG_RELEASE, pluck_Filter_ADSR[3]);

  // VCF
  VCF_pluck.Init(sample_rate);
  VCF_pluck.SetFreq(ft_pluck);
  VCF_pluck.SetRes(res_pluck);

  //-----------------------------------------------------------PAD

  // OSC

  pad.Init(sample_rate);
  pad.SetWaveforms(wf_pad);
  pad.SetFreq(f_pad);
  pad.SetAmp(mix_amp_pad);
  pad.SetDetune(0);        // No detune for pad
  pad.SetGain(1.0f);       // Set gain to 1.0 for pad
  pad.SetShape(shape_pad); // Set shape modulation for pad

  // amp_envelope
  amp_eg_pad.Init(sample_rate);

  amp_eg_pad.SetTime(ADSR_SEG_ATTACK, pad_Amp_ADSR[0]);
  amp_eg_pad.SetTime(ADSR_SEG_DECAY, pad_Amp_ADSR[1]);
  amp_eg_pad.SetSustainLevel(pad_Amp_ADSR[2]);
  amp_eg_pad.SetTime(ADSR_SEG_RELEASE, pad_Amp_ADSR[3]);

  // filter_envelope
  filt_eg_pad.Init(sample_rate);

  filt_eg_pad.SetTime(ADSR_SEG_ATTACK, pad_Filter_ADSR[0]);
  filt_eg_pad.SetTime(ADSR_SEG_DECAY, pad_Filter_ADSR[1]);
  filt_eg_pad.SetSustainLevel(pad_Filter_ADSR[2]);
  filt_eg_pad.SetTime(ADSR_SEG_RELEASE, pad_Filter_ADSR[3]);

  // VCF
  VCF_pad.Init(sample_rate);
  VCF_pad.SetFreq(ft_pad);
  VCF_pad.SetRes(res_pad);

  //-----------------------------------------------------------BASS
  // OSC
  bass.Init(sample_rate);
  bass.SetWaveforms(wf_bass);
  bass.SetFreq(f_bass);
  bass.SetAmp(mix_amp_bass);
  bass.SetDetune(0);         // No detune for bass
  bass.SetGain(1.0f);        // Set gain to 1.0 for
  bass.SetShape(shape_bass); // Set shape modulation for bass

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
  //-----------------------------------------------------------LFOs
  lfo1.Init(sample_rate);
  lfo1.SetFreq(lfo1_freq);                // Set LFO frequency for pluck
  lfo1.SetAmp(lfo1_amp);                  // Set LFO amplitude for pluck
  lfo1.SetWaveform(Oscillator::WAVE_SIN); // Set LFO waveform for pluck
  lfo1.PhaseAdd(lfo1_phase);              // Set initial phase for LFO pluck

  lfo2.Init(sample_rate);
  lfo2.SetFreq(lfo2_freq);                // Set LFO frequency for pad
  lfo2.SetAmp(lfo2_amp);                  // Set LFO amplitude for pad
  lfo2.SetWaveform(Oscillator::WAVE_SIN); // Set LFO waveform for pad
  lfo2.PhaseAdd(lfo2_phase);              // Set initial phase for LFO pad

  lfo3.Init(sample_rate);
  lfo3.SetFreq(lfo3_freq);                // Set LFO frequency for bass
  lfo3.SetAmp(lfo3_amp);                  // Set LFO amplitude for bass
  lfo3.SetWaveform(Oscillator::WAVE_SIN); // Set LFO waveform for bass
  lfo3.PhaseAdd(lfo3_phase);              // Set initial phase for LFO bass

  // DELAY
  Delay.Init(sample_rate);
  Delay.setDelayTime(delay_time);
  Delay.setFeedback(delay_fb);
  Delay.setMix(delay_mix);

  hw.StartAudio(AudioCallback);

#ifdef DEBUG

if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
{
  hw.PrintLine("MPR121 not found, check wiring?");
  while (1)
  {
    hw.SetLed(true);
    hw.DelayMs(500);
    hw.SetLed(false);
    hw.DelayMs(500);
  }
}
hw.PrintLine("MPR121 found!");

#endif // DEBUG
  while (1)
  {

    // Pluck
    
    currtouched = cap.Touched();

    if ((currtouched & _BV(0)) && !(lasttouched & _BV(0)))
    {
      Gate_pluck = true;
      value_pluck = cap.FilteredData(0);
      now_f_pluck = p_pluck.discFreq(value_pluck);
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 0 touched");
        hw.PrintLine("%d", (int)cap.FilteredData(0));
        hw.PrintLine("--------------------------------------------------------------------------------");
      #endif // DEBUG
      
    }

    if (!(currtouched & _BV(0)) && (lasttouched & _BV(0)))
    {
      Gate_pluck = false;
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 0 released");
        hw.PrintLine("--------------------------------------------------------------------------------");
       #endif // DEBUG
    }

    if ((currtouched & _BV(1)) && !(lasttouched & _BV(1)))
    {
      Gate_pad = true;
      value_pad = cap.FilteredData(1);
      now_f_pad = p_pad.discFreq(value_pad);
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 1 touched");
        hw.PrintLine("%d", (int)cap.FilteredData(1));
        hw.PrintLine("--------------------------------------------------------------------------------");
      #endif // DEBUG
    }

    if (!(currtouched & _BV(1)) && (lasttouched & _BV(1)))
    {
      Gate_pad = false;
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 1 released");
        hw.PrintLine("--------------------------------------------------------------------------------");
       #endif // DEBUG
    }

    if ((currtouched & _BV(2)) && !(lasttouched & _BV(2)))
    {
      Gate_bass = true;
      value_bass = cap.FilteredData(2);
      now_f_bass = p_bass.discFreq(value_bass);
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 2 touched");
        hw.PrintLine("%d", (int)cap.FilteredData(2));
        hw.PrintLine("--------------------------------------------------------------------------------");
      #endif // DEBUG
    }
    if (!(currtouched & _BV(2)) && (lasttouched & _BV(2)))
    {
      Gate_bass = false;
      #ifdef DEBUG
        hw.PrintLine("--------------------------------------------------------------------------------");
        hw.PrintLine("CAP 2 released");
        hw.PrintLine("--------------------------------------------------------------------------------");
       #endif // DEBUG
    }
    lasttouched = currtouched;


    hw.DelayMs(1);
  }

}