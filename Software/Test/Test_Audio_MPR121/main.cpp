#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"
#include "../../Classes/Temperamento/PlantConditioner.h"
#include "../../Classes/Effects/AnalogDelay/src/AnalogDelay.h"
#include "../../Classes/SensorFilters/IIR/IIR.h"
#include "../../Classes/SensorFilters/MF/MF.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// Configurazioni
#define ADC_CH 2
//#define DEBUG
//#define csvAcquisition

using namespace daisy;
using namespace daisysp;

// Oggetti Globali
daisy::Mpr121I2C::Config mpr121ObjConf;
daisy::Mpr121I2C cap;
DaisySeed hw;
AdcChannelConfig adcConfig[ADC_CH];
PlantConditioner pc;
static Oscillator osc;
static Adsr env;

// Variabili di stato
float f = 440.0f; // Frequenza corrente
bool gate = false;
uint16_t lastTouched = 0;
uint16_t currTouched = 0;

// Variabili per debug/stampa
bool flag = false;
int k = 0;

static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    float envOut, oscOut;
    for (size_t i = 0; i < size; i += 2)
    {
        // Calcolo Enviluppo e Oscillatore
        envOut = env.Process(gate);
        osc.SetAmp(envOut);
        oscOut = osc.Process();

        out[i] = out[i + 1] = oscOut;
    }
}

int main()
{
    // 1. Inizializzazione Hardware
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    float sampleRate = hw.AudioSampleRate();

    // 2. Inizializzazione DSP
    pc.Init(IIR::Butterworth); // Il filtro lavora bene a 100Hz
    pc.setScale(PlantConditioner::C, PlantConditioner::MinorArm);
    pc.setOctave(2);
    pc.setCurve(50, 1.1);

    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_TRI);
    osc.SetFreq(440.0f);
    osc.SetAmp(0.5f);

    env.Init(sampleRate);
    env.SetTime(ADSR_SEG_ATTACK, 0.05f);
    env.SetTime(ADSR_SEG_DECAY, 0.1f);
    env.SetSustainLevel(0.7f);
    env.SetTime(ADSR_SEG_RELEASE, 0.5f); // Aumentato leggermente per sentire la coda

    // 3. Inizializzazione ADC e Sensori
    adcConfig[0].InitSingle(daisy::seed::A0);
    adcConfig[1].InitSingle(daisy::seed::A1);
    hw.adc.Init(adcConfig, ADC_CH);
    hw.adc.Start();

    #if defined(DEBUG) || defined(csvAcquisition)
        hw.StartLog(false);
    #endif

    // Init MPR121
    if (cap.Init(mpr121ObjConf) != daisy::Mpr121I2C::Result::OK)
    {
        // Errore: Lampeggia LED infinito
        while (1) {
            hw.SetLed(true); hw.DelayMs(200);
            hw.SetLed(false); hw.DelayMs(200);
        }
    }

    hw.StartAudio(AudioCallback);

    // 4. Loop Principale a 100Hz (Non bloccante)
    uint32_t last_tick = System::GetNow();

    while (1)
    {
        uint32_t now = System::GetNow();

        // Esegui la logica sensori ogni 10ms esatti
        if (now - last_tick >= 1)
        {
            last_tick = now;

            // A. Lettura Parametri (Knobs)
            float curve_type = hw.adc.GetFloat(1) * 2 + 1;
            float delta_max = hw.adc.GetFloat(0) * 100 + 20;
            pc.setCurve(delta_max, curve_type);

            // B. Lettura Sensore Capacitivo
            currTouched = cap.Touched();
            
            // C. Elaborazione Filtri (SEMPRE ATTIVA)
            // Calcoliamo la frequenza target continuamente per tenere i filtri "caldi"
            float target_freq = pc.Process(cap.BaselineData(0), cap.FilteredData(0));

            // D. Logica Gate e Aggiornamento Audio
            bool is_touched_now = (currTouched & _BV(0));

            if (is_touched_now)
            {
                // Se tocco: attivo il gate e aggiorno la frequenza
                if (!gate) gate = true;
                
                f = target_freq;
                osc.SetFreq(f);
            }
            else
            {
                // Se rilascio: disattivo il gate MA NON aggiorno la frequenza.
                // In questo modo la "coda" (release) dell'inviluppo suona 
                // con l'ultima nota valida, invece di cadere a 0Hz o fare glitch.
                if (gate) gate = false;
            }

            lastTouched = currTouched;

            // E. Debugging (Sicuro a 100Hz)
            #ifdef DEBUG
                if (gate) {
                    if (!flag) { hw.PrintLine("--- TOUCH ---"); flag = true; }
                    hw.PrintLine("Delta: %.2f | Freq: %.2f", pc.getDelta(), f);
                } else {
                    if (flag) { hw.PrintLine("--- RELEASE ---"); flag = false; }
                }
            #endif

            #ifdef csvAcquisition
                // Stampa CSV: DeltaRaw, DeltaFiltered, GateStatus
                hw.PrintLine("%.2f,%.2f,%d", pc.getDelta(), pc.getDeltaFilt(), gate ? 1 : 0);
            #endif
        }
        
       
    }
}