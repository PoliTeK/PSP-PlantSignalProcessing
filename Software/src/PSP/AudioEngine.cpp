#include "AudioEngine.h"
#include "Utility/dsp.h"

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
    
    _osc1.SetWaveform(_currentPreset.osc1.waveform);
    _osc1.SetAmp(_currentPreset.osc1.amp);
    _osc1.SetShape(_currentPreset.osc1.shape);
    _osc1.SetDetune(_currentPreset.osc1.detune);


    _osc2.SetWaveform(_currentPreset.osc2.waveform);
    _osc2.SetAmp(_currentPreset.osc2.amp);
    _osc2.SetShape(_currentPreset.osc2.shape);
    _osc2.SetDetune(_currentPreset.osc2.detune);


    _lfo1.SetWaveform(_currentPreset.lfo1.waveform);
    _lfo2.SetAmp(_currentPreset.lfo2.amp);
    _lfo2.SetFreq(_currentPreset.lfo2.freq);

    _lfo2.SetWaveform(_currentPreset.lfo2.waveform);
    _lfo2.SetAmp(_currentPreset.lfo2.amp);
    _lfo2.SetFreq(_currentPreset.lfo2.freq);

    _dust.SetDensity(_currentPreset.noise.color);
    
    _amp_env.SetTime(daisysp::ADSR_SEG_ATTACK, _currentPreset.amp_env.attack);
    _amp_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.amp_env.decay);
    _amp_env.SetSustainLevel(_currentPreset.amp_env.sustain);
    _amp_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.amp_env.release);

    _filt_env.SetTime(daisysp::ADSR_SEG_ATTACK, _currentPreset.filt_env.attack);
    _filt_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.filt_env.decay);
    _filt_env.SetSustainLevel(_currentPreset.filt_env.sustain);
    _filt_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.filt_env.release);

    _filt.SetRes(_currentPreset.filter.resonance);
    _filt.SetFreq(_currentPreset.filter.cutoff);

    _reverb.SetLpFreq(_currentPreset.reverb.lpFreq);
    _reverb.SetFeedback(_currentPreset.reverb.feedback);
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
    // Normalizzazione (0.0 - 1.0)
    float val_norm = static_cast<float>(cc_value) / 127.0f; 

    // Helper per le forme d'onda (mappa 0-127 su 8 valori interi: 0-7)
    uint8_t lfo_wave_sel = static_cast<uint8_t>(val_norm * 3.999f); 
    uint8_t osc_wave_sel = static_cast<uint8_t>(val_norm * 2.999f); 
    switch (cc_number) {
        
        // ==========================================
        // LFO 1 (CC 14 - 16)
        // ==========================================
        case 14: preset.lfo1.waveform = lfo_wave_sel; break;
        case 15: preset.lfo1.amp = val_norm; break;
        case 16: preset.lfo1.freq = 0.1f + (val_norm * 20.0f); break;

        // ==========================================
        // LFO 2 (CC 17 - 19)
        // ==========================================
        case 17: preset.lfo2.waveform = lfo_wave_sel; break;
        case 18: preset.lfo2.amp = val_norm; break;
        case 19: preset.lfo2.freq = 0.1f + (val_norm * 20.0f); break;

        // ==========================================
        // OSCILLATOR 1 (CC 20 - 23)
        // ==========================================
        case 20: preset.osc1.waveform = osc_wave_sel; break;
        case 21: preset.osc1.amp = val_norm; break;
        case 22: preset.osc1.shape = val_norm; break;
        case 23: preset.osc1.detune = (val_norm * 100.0f) - 50.0f; break; // Da -50 a +50 cents

        // ==========================================
        // OSCILLATOR 2 (CC 24 - 27)
        // ==========================================
        case 24: preset.osc2.waveform = osc_wave_sel; break;
        case 25: preset.osc2.amp = val_norm; break;
        case 26: preset.osc2.shape = val_norm; break;
        case 27: preset.osc2.detune = (val_norm * 100.0f) - 50.0f; break; // Da -50 a +50 cents

        // ==========================================
        // NOISE (CC 28 - 29)
        // ==========================================
        case 28: preset.noise.amp = val_norm; break;
        case 29: preset.noise.color = val_norm; break;

        // ==========================================
        // FILTER (CC 74, 71)
        // ==========================================
        case 74: preset.filter.cutoff = 20.0f + (val_norm * val_norm) * 18000.0f; break;
        case 71: preset.filter.resonance = val_norm * 0.95f; break; // Limite di sicurezza 0.95

        // ==========================================
        // AMP ENVELOPE (CC 73, 75, 79, 72, 80)
        // ==========================================
        case 73: preset.amp_env.attack  = 0.01f + (val_norm * 4.0f); break;
        case 75: preset.amp_env.decay   = 0.01f + (val_norm * 4.0f); break;
        case 79: preset.amp_env.sustain = val_norm; break;
        case 72: preset.amp_env.release = 0.01f + (val_norm * 5.0f); break;
        case 80: preset.amp_env.amp     = val_norm; break;

        // ==========================================
        // FILTER ENVELOPE (CC 81 - 85)
        // ==========================================
        case 81: preset.filt_env.attack  = 0.01f + (val_norm * 4.0f); break;
        case 82: preset.filt_env.decay   = 0.01f + (val_norm * 4.0f); break;
        case 83: preset.filt_env.sustain = val_norm; break;
        case 84: preset.filt_env.release = 0.01f + (val_norm * 5.0f); break;
        case 85: preset.filt_env.amp     = val_norm; break;

        // ==========================================
        // REVERB (CC 91, 93, 94)
        // ==========================================
        case 91: preset.reverb.dryWet   = val_norm; break;
        case 93: preset.reverb.feedback = val_norm * 0.99f; break; // Mai oltre 0.99
        case 94: preset.reverb.lpFreq   = 500.0f + (val_norm * 17500.0f); break;

        // ==========================================
        // SWITCH (CC 100, 101)
        // ==========================================
        case 100: cc_value < 64 ? preset.sync = false : preset.sync = true; break;
        case 101: cc_value < 64 ? preset.ring = false : preset.ring = true; break;

        // CC non mappato -> esci senza richiamare SetActivePreset
        default: return; 
    }

    // Applica immediatamente le modifiche al motore in esecuzione
    SetActivePreset(preset);
}

void AudioEngine::Process(float& out_l, float& out_r) {
    // 1. Elaborazione LFO e Inviluppi
    float amp_env_out = _amp_env.Process(_lastGate);
    float filt_env_out = _filt_env.Process(_lastGate);

    // 2. Aggiornamento Oscillatori
    _osc1.SetWaveform(_currentPreset.osc1.waveform);
    _osc2.SetWaveform(_currentPreset.osc2.waveform);

    _osc1.SetDetune(_currentPreset.osc1.detune);
    _osc2.SetDetune(_currentPreset.osc2.detune);
    _osc1.SetFreq(_currentFreq);
    _osc2.SetFreq(_currentFreq); 
    
    _osc1.SetAmp(_currentPreset.osc1.amp);
    _osc2.SetAmp(_currentPreset.osc2.amp);

    _osc1.SetShape(_currentPreset.osc1.shape);
    _osc2.SetShape(_currentPreset.osc2.shape);

    _lfo1.SetWaveform(_currentPreset.lfo1.waveform);
    _lfo2.SetWaveform(_currentPreset.lfo2.waveform);

    _lfo1.SetAmp(_currentPreset.lfo1.amp);
    _lfo2.SetAmp(_currentPreset.lfo2.amp);

    _lfo1.SetFreq(_currentPreset.lfo1.freq);
    _lfo2.SetFreq(_currentPreset.lfo2.freq);

    _dust.SetDensity(_currentPreset.noise.color);
    
    // Gestione della sincronizzazione degli oscillatori
    if (_currentPreset.sync) {
        if (_osc1.IsEOC()) _osc2.Reset();
    }
    
    float s_osc1 = _osc1.Process() * _currentPreset.osc1.amp;
    float s_osc2 = _osc2.Process() * _currentPreset.osc2.amp;

    float sNoise = _dust.Process() * _currentPreset.noise.amp;
    // Gestione della modulazione ad anello
    float sMix = s_osc1 + s_osc2 + sNoise;
    if (_currentPreset.ring) {
        sMix = s_osc1 * s_osc2 + sNoise; 
    }         
    

    // 3. Calcolo del Cutoff modulato e applicazione al filtro
    float target_cutoff = _currentPreset.filter.cutoff + (filt_env_out * _currentPreset.filt_env.amp * 10000.0f);
    //TODO : LFO
    target_cutoff = daisysp::fclamp(target_cutoff, 20.0f, 20000.0f);
    
    _filt.SetFreq(target_cutoff);
    _filt.SetRes(_currentPreset.filter.resonance);

    float sFilt = _filt.Process(sMix);

    // 5. Applicazione VCA (Inviluppo di ampiezza, segnale mono)
    float sDry = sFilt * amp_env_out * _currentPreset.amp_env.amp ;
    //TODO : LFO

    // 6. Riverbero Stereo
    float revL = 0.0f;
    float revR = 0.0f;
    
    // Invia il segnale mono a entrambi gli ingressi del riverbero
    _reverb.Process(sDry, sDry, &revL, &revR);
    
    float revDryWet = _currentPreset.reverb.dryWet;
    
    // 7. Calcolo del mix stereo
    // Scrive direttamente le variabili passate per riferimento
    out_l = (sDry * (1.0f - revDryWet)) + (revL * revDryWet);
    out_r = (sDry * (1.0f - revDryWet)) + (revR * revDryWet);
}