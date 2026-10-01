#include "AudioEngine.h"
#include "AudioEngineConfig.h"

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}

void AudioEngine::Init(float sample_rate) {
    _osc1.Init(sample_rate);
    _osc2.Init(sample_rate);
    _lfo1.Init(sample_rate);
    _lfo2.Init(sample_rate);
    _amp_env.Init(sample_rate);
    _filt_env.Init(sample_rate);
    _filt.Init(sample_rate);
    _reverb.Init(sample_rate);

    _lastGate = false;
    _currentFreq = 440.0f;
}

void AudioEngine::SetActivePreset(const Preset_s& preset) {
    _currentPreset = preset;
    
    // Aggiorna subito i parametri che non necessitano di calcoli per sample
    _osc1.SetWaveform(_currentPreset.osc1.Waveform);
    _osc2.SetWaveform(_currentPreset.osc2.Waveform);
    _lfo1.SetWaveform(_currentPreset.lfo1.Waveform);
    _lfo2.SetWaveform(_currentPreset.lfo2.Waveform);
    
    
    _amp_env.SetTime(daisysp::ADSR_SEG_ATTACK, _currentPreset.amp_env.Attack);
    _amp_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.amp_env.Decay);
    _amp_env.SetSustainLevel(_currentPreset.amp_env.Sustain);
    _amp_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.amp_env.Release);

    _filt_env.SetTime(daisysp::ADSR_SEG_ATTACK, _currentPreset.filt_env.Attack);
    _filt_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.filt_env.Decay);
    _filt_env.SetSustainLevel(_currentPreset.filt_env.Sustain);
    _filt_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.filt_env.Release);


    _filt.SetRes(_currentPreset.filter.Resonance);
}


void AudioEngine::UpdateControls(const Control_s& controls) {
    _currentFreq = controls.freq;
    
    if(controls.gate && !_lastGate) {
        _amp_env.Retrigger(false);
        _filt_env.Retrigger(false);
    }
    _lastGate = controls.gate;
}

float AudioEngine::Process() {
    float amp_env_out = _amp_env.Process(_lastGate);
    float filt_env_out = _filt_env.Process(_lastGate);

    _osc1.SetFreq(_currentFreq);
    _osc2.SetFreq(_currentFreq);

    _osc1.SetAmp(_currentPreset.osc1.Amp);
    _osc2.SetAmp(_currentPreset.osc2.Amp);

    
    float sig_osc = _osc1.Process() + _osc2.Process();

    float target_cutoff = _currentPreset.filter.Cutoff + (filt_env_out * _currentPreset.filt_env.Amp);
    if(target_cutoff > 20000.0f) target_cutoff = 20000.0f;
    if(target_cutoff < 20.0f) target_cutoff = 20.0f;
    
    _filt.SetFreq(target_cutoff);

    float sig_filt = _filt.Process(sig_osc);

    float final_sig = sig_filt * amp_env_out;

    // NOTA: Il riverbero ReverbSc è stereo. Se Process() restituisce un solo float,
    // dovrai gestire il riverbero fuori (nel callback principale) o cambiare il ritorno di Process().
    // Per ora restituiamo il segnale dry.
    
    return final_sig;
}

