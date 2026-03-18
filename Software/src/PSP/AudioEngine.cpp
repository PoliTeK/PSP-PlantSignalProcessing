#include "AudioEngine.h"

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}

void AudioEngine::Init(float sample_rate) {
    // Inizializzazione moduli
    _osc.Init(sample_rate);
    _env.Init(sample_rate);
    _filter.Init(sample_rate);

    _lastGate = false;
    _currentFreq = 440.0f;
    
    // Configura un preset di default
    SetPreset(PRESET_PAD);
}

void AudioEngine::SetPreset(SynthPreset preset) {
    _currentPreset = preset;

    switch (preset) {
        case PRESET_PAD:
            _osc.SetWaveform(0);
            _env.SetTime(daisysp::ADSR_SEG_ATTACK, 0.5f);
            _env.SetTime(daisysp::ADSR_SEG_DECAY, 0.2f);
            _env.SetTime(daisysp::ADSR_SEG_RELEASE, 1.2f);
            _env.SetSustainLevel(0.8f);
            _filter.SetFreq(6000.0f);
            _filter.SetRes(0.1f);
            break;

        case PRESET_PLUCK:
            _osc.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
            _env.SetTime(daisysp::ADSR_SEG_ATTACK, 0.01f);
            _env.SetTime(daisysp::ADSR_SEG_DECAY, 0.15f);
            _env.SetTime(daisysp::ADSR_SEG_RELEASE, 0.3f);
            _env.SetSustainLevel(0.0f); // Pluck non ha sustain
            _filter.SetFreq(1500.0f);
            _filter.SetRes(0.4f);
            break;

        case PRESET_LEAD:
            _osc.SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SAW);
            _env.SetTime(daisysp::ADSR_SEG_ATTACK, 0.05f);
            _env.SetTime(daisysp::ADSR_SEG_DECAY, 0.1f);
            _env.SetTime(daisysp::ADSR_SEG_RELEASE, 0.4f);
            _env.SetSustainLevel(0.7f);
            _filter.SetFreq(8000.0f);
            _filter.SetRes(0.3f);
            break;

        default:
            break;
    }
}

void AudioEngine::Update(ControlsStruct Controls) {
    // Aggiorna la frequenza in modo continuo o usa un portamento se necessario
    _currentFreq = Controls.freq;
    _osc.SetFreq(_currentFreq);

    // Gestione del trigger dell'inviluppo (Fronte di salita)
    if (Controls.gate && !_lastGate) {
        _env.Retrigger(false);
    }
    
    // Aggiornamento dello stato del gate
    _lastGate = Controls.gate;
}

float AudioEngine::Process() {
    // 1. Processa l'inviluppo passando lo stato del gate
    float env_out = _env.Process(_lastGate);

    // 2. Processa l'oscillatore
    float sig = _osc.Process();

    // 3. Applica l'inviluppo all'ampiezza
    sig *= env_out;

    // 4. (Opzionale) Modula il filtro con l'inviluppo per dare dinamica
    // _filter.SetFreq(200.0f + (env_out * 4000.0f));

    // 5. Processa il segnale attraverso il filtro
    _filter.Process(sig);

    // Ritorna l'uscita passa-basso
    return _filter.Low();
}