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
static VariableShapeOscillator osc; // Oscillator è la classe degli oscillatori


const int on = 1;
float Pot0;
float Pot1;
float Pot2;

float freq;
float DC;
float shape;





int SmoothAnalogRead(int analogPin, int type){ //0 = low res 1 = mid res, 2 =  high res)
  float y;
  float Sum = 0;
  int k = 16;
  
  switch(type){
    case 0:
      k = 16;
      break;
    case 1:
      k = 64;
      break;
    case 2:
      k = 256;
      break;
  }
 
  analogReadResolution(16);
  for (int i = 0; i < k; i++){
    Sum += analogRead(analogPin);
  }
  y = Sum/k;
  return y;
}

void MyCallback(float **in, float **out, size_t size) {
  osc.SetSyncFreq(freq);
  osc.SetWaveshape(shape);
  osc.SetPW(DC);
  for (size_t i=0; i < size; i++){
    float signal = osc.Process();
    out[0][i] = signal;
    out[1][i] = signal;
  }
}

void setup() {
  analogReadResolution(16);
  pinMode(on, INPUT);
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K); // inizializza l'hardware hw come l'oggetto Daisy Seed con SampleRate specificato
  num_channels = hw.num_channels; // prende il numero di canali proprio dell'oggetto hardware
  sample_rate = DAISY.get_samplerate(); // prende il sample rate
  osc.Init(sample_rate); // con riferimento al file Oscillator.h, a partire dal sample rate costruisce variabili utili alla costruzione del segnale e setta dei valori di default per frequenza,amp,pw,phase etc

  osc.SetWaveshape(0); // ref Oscillator.h-->funzione che prende come input dei numeri associati alle forme d'onda interni alla classe Oscillator (), in base a quelli e il valore di phase, calcola il vlore numerico dell'uscita.     Ad ogni ciclo phase += phase_inc dove phase_inc = f/Samplerate(sr)
  osc.SetFreq(110); // imposta la frequenza in Hz
  //osc.SetAmp(0.5); // imposta l'ampiezza dove amp = Vpp/2 (da verificare)
  
  
  DAISY.begin(MyCallback);
}

void loop() {
  analogReadResolution(16);
  Pot0 = SmoothAnalogRead(A0,2);
  Pot1 = SmoothAnalogRead(A1,0);
  Pot2 = SmoothAnalogRead(A2,0);

  freq = 110 + ( Pot0/65535)*330; 
  DC = 0.5 + Pot1/65535*0.45;
  shape =0.25 +  Pot2/65535*0.75;

  /*if (digitalRead(on) == HIGH){
    osc.SetAmp(1);
  } else {
    osc.SetAmp(0);
  }*/
  }   
  //pw = SmoothAnalogRead(A1);
  
  
  

  
 