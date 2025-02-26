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

size_t num_channels;   // size-t è un tipo di intero unsigned che ha la grandezza massima disponibile

static Oscillator osc; // Oscillator è la classe degli oscillatori

float pw;
float freq;
float Pot0_in;
float Pot0_last;
float Pot0;
float threshold = 20 ;

int SmoothAnalogRead(int analogPin){
  float k = 65;
  float y;
  float Sum = 0;
  analogReadResolution(16);
  for (int i = 0; i < k; i++){
    Sum += analogRead(analogPin);
  }
  y = Sum/k;
  return y;
}

void MyCallback(float **in, float **out, size_t size) {
  //osc.SetPw(pw);
  osc.SetFreq(freq);
  for (size_t i=0; i < size; i++){
    float signal = osc.Process();
    out[0][i] = signal;
    out[1][i] = signal;
  }
}

void setup() {
  analogReadResolution(16);
  
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K); // inizializza l'hardware hw come l'oggetto Daisy Seed con SampleRate specificato
  num_channels = hw.num_channels; // prende il numero di canali proprio dell'oggetto hardware
  sample_rate = DAISY.get_samplerate(); // prende il sample rate
  osc.Init(sample_rate); // con riferimento al file Oscillator.h, a partire dal sample rate costruisce variabili utili alla costruzione del segnale e setta dei valori di default per frequenza,amp,pw,phase etc

  osc.SetWaveform(osc.WAVE_SQUARE); // ref Oscillator.h-->funzione che prende come input dei numeri associati alle forme d'onda interni alla classe Oscillator (), in base a quelli e il valore di phase, calcola il vlore numerico dell'uscita.     Ad ogni ciclo phase += phase_inc dove phase_inc = f/Samplerate(sr)
  osc.SetFreq(1000); // imposta la frequenza in Hz
  osc.SetAmp(0.5); // imposta l'ampiezza dove amp = Vpp/2 (da verificare)
  
  
  DAISY.begin(MyCallback);
}

void loop() {
  analogReadResolution(16);
  Pot0 = SmoothAnalogRead(A0);
  freq = 110 + ( Pot0/65535)*330;    
  //pw = SmoothAnalogRead(A1);
  
  
  

  
 }