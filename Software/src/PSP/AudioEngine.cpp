#include "AudioEngine.h"

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}

void AudioEngine::Init(float sample_rate) {
    // Inizializzazione moduli
    _osc1.Init(sample_rate);
    _osc2.Init(sample_rate);
    _lfo.Init(sample_rate);
    _Aenv.Init(sample_rate);
    _Fenv.Init(sample_rate);
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
            _osc1.SetWaveform(0);
            _Aenv.SetTime(daisysp::ADSR_SEG_ATTACK, 0.5f);
            _Aenv.SetTime(daisysp::ADSR_SEG_DECAY, 0.2f);
            _Aenv.SetTime(daisysp::ADSR_SEG_RELEASE, 1.2f);
            _Aenv.SetSustainLevel(0.8f);
            _filter.SetFreq(6000.0f);
            _filter.SetRes(0.1f);
            break;

        case PRESET_PLUCK:
            _Aenv.SetTime(daisysp::ADSR_SEG_ATTACK, 0.01f);
            _Aenv.SetTime(daisysp::ADSR_SEG_DECAY, 0.15f);
            _Aenv.SetTime(daisysp::ADSR_SEG_RELEASE, 0.3f);
            _Aenv.SetSustainLevel(0.0f); // Pluck non ha sustain
            
            _filter.SetFreq(1500.0f);
            _filter.SetRes(0.4f);
            break;

        case PRESET_LEAD:
            _osc1.SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SQUARE);
            _osc2.SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SQUARE);
            _lfo.SetWaveform(daisysp::Oscillator::WAVE_TRI);
            _lfo.SetFreq(0.7f);
            _lfo.SetAmp(0.005f);
            _Aenv.SetTime(daisysp::ADSR_SEG_ATTACK, 0.05f);
            _Aenv.SetTime(daisysp::ADSR_SEG_DECAY, 2.8f);
            _Aenv.SetTime(daisysp::ADSR_SEG_RELEASE, 1.3f);
            _Aenv.SetSustainLevel(0.6f);
            _Fenv.SetTime(daisysp::ADSR_SEG_ATTACK, 0.1f);
            _Fenv.SetTime(daisysp::ADSR_SEG_DECAY, 1.8f);
            _Fenv.SetTime(daisysp::ADSR_SEG_RELEASE, 2.0f);
            _Fenv.SetSustainLevel(0.4f);
            _filter.SetFreq(8000.0f);
            _filter.SetRes(0.3f);
            break;

        default:
            break;
    }
}

void AudioEngine::Update(ControlsStruct Controls) {

    _currentFreq = Controls.freq;
    if (Controls.gate && !_lastGate) {
        _Aenv.Retrigger(false);
    }
    _lastGate = Controls.gate;
}

float AudioEngine::Process() {
    float Aenv_out;
    float Fenv_out;
    float sig;

    switch (_currentPreset)
    {
    case PRESET_PAD:
        /* PAD code */
        break;
    case PRESET_PLUCK:
        /*PLUCK code*/
        break;
    case PRESET_LEAD:
        _osc1.SetFreq(_currentFreq * (1 +  _lfo.Process()));
        _osc2.SetFreq(_currentFreq * 2.0f * (1 +  _lfo.Process()));
        _osc1.SetPw(0.1f);
        _osc2.SetPw(0.1f);

        Aenv_out = _Aenv.Process(_lastGate);
        Fenv_out = _Fenv.Process(_lastGate);
        _filter.SetFreq(4000 * (1 + Fenv_out));
    
    default:
        break;
    }
    sig = (_osc1.Process() + _osc2.Process())/2.0f;
    sig *= Aenv_out;

    _filter.Process(sig);

    // Ritorna l'uscita passa-basso
    return _filter.Low();
}