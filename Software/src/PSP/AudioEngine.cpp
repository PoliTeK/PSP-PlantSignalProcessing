#include "AudioEngine.h"

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

    SetActivePreset(_defaultPreset);
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

    _reverb.SetLpFreq(_currentPreset.reverb.revLpFreq);
    _reverb.SetFeedback(_currentPreset.reverb.revFeedback);
}


void AudioEngine::UpdateControls(const Control_s& controls) {
    _currentFreq = controls.freq;
    
    if(controls.gate && !_lastGate) {
        _amp_env.Retrigger(false);
        _filt_env.Retrigger(false);
    }
    _lastGate = controls.gate;
}

void AudioEngine::ProcessMidiCC(uint8_t cc_number, uint8_t cc_value, Preset_s& preset) {
    float val_norm = cc_value / 127.0f; 

    switch (cc_number) {
        case 74: // Cutoff
            preset.filter.Cutoff = 20.0f + (val_norm * 10000.0f);
            break;
            
        case 71: // Resonance
            preset.filter.Resonance = val_norm;
            break;
            
        case 73: // Attack
            preset.amp_env.Attack = 0.01f + (val_norm * 2.0f);
            break;
            
        case 72: // Release
            preset.amp_env.Release = 0.01f + (val_norm * 5.0f);
            break;
            
        case 91: // Reverb Dry/Wet
            preset.reverb.DryWet = val_norm;
            break;
            
        case 20: // Volume Osc 1
            preset.osc1.Amp = val_norm;
            break;
            
        case 21: // Volume Osc 2
            preset.osc2.Amp = val_norm;
            break;
            
        default:
            return; // Se il CC non è mappato, esce senza fare nulla
    }

    // Applica le modifiche al motore in esecuzione
    SetActivePreset(preset);
}

void AudioEngine::Process(float& out_l, float& out_r) {
    // 1. Elaborazione LFO e Inviluppi
    float amp_env_out = _amp_env.Process(_lastGate);
    float filt_env_out = _filt_env.Process(_lastGate);

    // 2. Aggiornamento Oscillatori
    _osc1.SetFreq(_currentFreq);
    _osc2.SetFreq(_currentFreq); 
    
    _osc1.SetAmp(_currentPreset.osc1.Amp);
    _osc2.SetAmp(_currentPreset.osc2.Amp);

    float sig_osc = _osc1.Process() + _osc2.Process();

    // 3. Calcolo del Cutoff modulato
    float target_cutoff = _currentPreset.filter.Cutoff + (filt_env_out * _currentPreset.filt_env.Amp);
    if(target_cutoff > 20000.0f) target_cutoff = 20000.0f;
    if(target_cutoff < 20.0f) target_cutoff = 20.0f;
    
    _filt.SetFreq(target_cutoff);

    // 4. Filtraggio
    float filtered_sig = _filt.Process(sig_osc);

    // 5. Applicazione VCA (Inviluppo di ampiezza, segnale mono)
    float final_sig = filtered_sig * amp_env_out;

    // 6. Riverbero Stereo
    float revL = 0.0f;
    float revR = 0.0f;
    
    // Invia il segnale mono a entrambi gli ingressi del riverbero
    _reverb.Process(final_sig, final_sig, &revL, &revR);
    
    float revDryWet = _currentPreset.reverb.DryWet;
    
    // 7. Calcolo del mix stereo
    // Scrive direttamente le variabili passate per riferimento
    out_l = (final_sig * (1.0f - revDryWet)) + (revL * revDryWet);
    out_r = (final_sig * (1.0f - revDryWet)) + (revR * revDryWet);
}