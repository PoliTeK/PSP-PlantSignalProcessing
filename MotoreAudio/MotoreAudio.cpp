#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
#define n 3 //numero di suoni del motore audio
#define m 2 // numero di voci per ogni suono
#define detune 1.001  // coefficiente di detuning (considera che sqrt_12(2) = 1.06 è 1 semitono)
#define startingFreq 150


DaisySeed         hw;
AdcChannelConfig adcConfig[4]; // 3 canali ADC, Diventerà la lettura capacitiva
float knob[3]; // 3 potenziometri--> Diventeranno Piante


Adsr        Amp_env[n];  // inviluppi all'ampiezza
Adsr	    LPF_env[n]; // inviluppi al filtro
Oscillator   osc[n][m]; // oscillatori n suoni e m voci

float Suono[n]; // n Suoni somma di m voci
float f[n]; // n frequenze da settare







void InstrumentInit(Oscillator Voices[n][m], /*Adsr Aeg[n], Adsr Feg*/ float sr)
{
	//inizializza suoni
	for (int i = 0 ; i<n ; i++){
		for (int j = 0 ; j<m ; j++){

			uint8_t k = j + i + 5; // 5=tri 6=saw 7=square se vai a vedere nel Cpp degli Oscillatori
			if (i == 2 && j ==1) k = 5; //forza la forma d'onda a triangolare quando  i +j +5 vuole andare a 8

			//Voci 
			Voices[i][j].Init(sr);
			Voices[i][j].SetWaveform(k);
    		Voices[i][j].SetFreq(150);
    		Voices[i][j].SetAmp(0.5);

		
		}
	//Aeg[i].Init(sr);
	//Feg[i].Init(sr);
	}
	
	/* INVILUPPI
	// Amp ADSR suono 1
	Aeg[1].SetTime(ADSR_SEG_ATTACK, .5);
    Aeg[1].SetTime(ADSR_SEG_DECAY, .5);
    Aeg[1].SetTime(ADSR_SEG_RELEASE, .6);
	Aeg[1].SetSustainLevel(.5);

	// Amp ADSR suono 2
	Aeg[2].SetTime(ADSR_SEG_ATTACK, .05);
    Aeg[2].SetTime(ADSR_SEG_DECAY, .4);
    Aeg[2].SetTime(ADSR_SEG_RELEASE, 0);
	Aeg[2].SetSustainLevel(0);

	// Amp ADSR suono 3
	Aeg[3].SetTime(ADSR_SEG_ATTACK, .1);
    Aeg[3].SetTime(ADSR_SEG_DECAY, .1);
    Aeg[3].SetTime(ADSR_SEG_RELEASE, .1);
	Aeg[3].SetSustainLevel(.4);

	// LPF ADSR suono 1
	Aeg[1].SetTime(ADSR_SEG_ATTACK, .5);
    Aeg[1].SetTime(ADSR_SEG_DECAY, .5);
    Aeg[1].SetTime(ADSR_SEG_RELEASE, .6);
	Aeg[1].SetSustainLevel(.5);

	// LPF ADSR suono 2
	Aeg[2].SetTime(ADSR_SEG_ATTACK, .05);
    Aeg[2].SetTime(ADSR_SEG_DECAY, .4);
    Aeg[2].SetTime(ADSR_SEG_RELEASE, 0);
	Aeg[2].SetSustainLevel(0);

	// LPF ADSR suono 3
	Aeg[3].SetTime(ADSR_SEG_ATTACK, .1);
    Aeg[3].SetTime(ADSR_SEG_DECAY, .1);
    Aeg[3].SetTime(ADSR_SEG_RELEASE, .1);
	Aeg[3].SetSustainLevel(.4);
	
	*/

}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (int i = 0 ; i < n; i++)
	{
		osc[i][0].SetFreq(f[i]);
		osc[i][1].SetFreq(f[i]*detune); // detuna la seconda voce di ogni suono
	}

	for (size_t i = 0; i < size; i++)
	{
		for (int j = 0; j<n ; j++){
			Suono[j] = osc[j][0].Process() + osc[j][1].Process(); // raggruppa le due voci nel suono, andrà moltiplicato per l'inviluppo
		}
		float signal = Suono[0]                                // mix dei suoni
					    + Suono[1] 
						+ Suono[2];
		out[0][i] = 0.5*signal;
		out[1][i] = 0.5*signal;
	}
}

int main(void)
{
	float sample_rate;
	hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();

	adcConfig[0].InitSingle(hw.GetPin(15));
	adcConfig[1].InitSingle(hw.GetPin(16));
	adcConfig[2].InitSingle(hw.GetPin(17));
	adcConfig[3].InitSingle(hw.GetPin(18));
    hw.adc.Init(adcConfig, 4);
    hw.adc.Start();
	



	InstrumentInit(osc,sample_rate);


	hw.StartAudio(AudioCallback); // Avvia il processamento audio

	while(1) {
		// LEttura pots
		for (int i = 0; i < 3 ; i++)
		{
			knob[i] = hw.adc.GetFloat(i);
		}

		// setta le letture ai loro valori
		float f_base = startingFreq;
		for (int i = 0 ; i < n ; i++){
			f[i]= f_base + knob[i]*f_base;
			f_base += f_base;
		}
	System::Delay(1);
	}
}
