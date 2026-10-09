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
    _filt.SetFilterMode(daisysp::LadderFilter::FilterMode::LP24);
    _reverb.Init(sample_rate);
    _xorMod.init();
    _xorMod.setMask(0.0f); 

    _lastGate = false;
    _currentFreq = 440.0f;

    SetActivePreset(_defaultPreset);
    _smoothedPreset = _currentPreset;
}

void AudioEngine::SetMasterVolume(float volume) {
    _masterVolume = daisysp::fclamp(volume, 0.0f, 1.0f);
}

void AudioEngine::SetActivePreset(const Preset_s& preset) {
    _currentPreset = preset;
    
    _osc1.SetWaveform(_currentPreset.osc1.waveform);
    _osc1.SetAmp(_currentPreset.osc1.amp);
    _osc1.SetShape(_currentPreset.osc1.shape);
    _osc1.SetDetune(_currentPreset.osc1.detune);
    _osc1.SetOctave(_currentPreset.osc1.octave);


    _osc2.SetWaveform(_currentPreset.osc2.waveform);
    _osc2.SetAmp(_currentPreset.osc2.amp);
    _osc2.SetShape(_currentPreset.osc2.shape);
    _osc2.SetDetune(_currentPreset.osc2.detune);
    _osc2.SetOctave(_currentPreset.osc2.octave);


    _lfo1.SetWaveform(_currentPreset.lfo1.waveform);
    _lfo1.SetAmp(1.0f); // Fisso a 1, l'ampiezza la gestisci nel Process()
    _lfo1.SetFreq(_currentPreset.lfo1.freq);

    // LFO 2
    _lfo2.SetWaveform(_currentPreset.lfo2.waveform);
    _lfo2.SetAmp(1.0f); // Fisso a 1
    _lfo2.SetFreq(_currentPreset.lfo2.freq);

    _dust.SetDensity(_currentPreset.noise.color);
    
    _amp_env.SetAttackTime(_currentPreset.amp_env.attack, 0.7f);
    _amp_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.amp_env.decay);
    _amp_env.SetSustainLevel(_currentPreset.amp_env.sustain);
    _amp_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.amp_env.release);

    _filt_env.SetAttackTime(_currentPreset.filt_env.attack, 0.7f);
    _filt_env.SetTime(daisysp::ADSR_SEG_DECAY, _currentPreset.filt_env.decay);
    _filt_env.SetSustainLevel(_currentPreset.filt_env.sustain);
    _filt_env.SetTime(daisysp::ADSR_SEG_RELEASE, _currentPreset.filt_env.release);

    _filt.SetRes(_currentPreset.filter.resonance);
    _filt.SetFreq(_currentPreset.filter.cutoff);

    _xorMod.setMask(_currentPreset.xor_m.amount);

    _reverb.SetLpFreq(_currentPreset.reverb.lpFreq);
    _reverb.SetFeedback(_currentPreset.reverb.feedback);
}


void AudioEngine::UpdateControls(const Control_s& controls) {
    _currentFreq = controls.freq;
    
    if(controls.gate && !_lastGate) {
        _amp_env.Retrigger(false);
        _filt_env.Retrigger(false);
        _osc1.SetFreq(_currentFreq);
        _osc2.SetFreq(_currentFreq);
    }
    _lastGate = controls.gate;
}

void AudioEngine::ProcessMidiCC(uint8_t cc_number, uint8_t cc_value, Preset_s& preset) {
    // Normalizzazione (0.0 - 1.0)
    float val_norm = static_cast<float>(cc_value) / 127.0f; 

    // Helper per le forme d'onda (mappa 0-127 su 8 valori interi: 0-7)
    uint8_t lfo_wave_sel = static_cast<uint8_t>(val_norm * 3.999f); 
    uint8_t osc_wave_sel = static_cast<uint8_t>(val_norm * 2.999f) + 1; 
    switch (cc_number) {
        
        // ==========================================
        // LFO 1 (CC 14 - 16)
        // ==========================================
        case 14: preset.lfo1.waveform = lfo_wave_sel; break;
        case 15: preset.lfo1.amp = val_norm; break;
        case 16: preset.lfo1.freq = 0.01f + (val_norm * 15.0f); break;

        // ==========================================
        // LFO 2 (CC 17 - 19)
        // ==========================================
        case 17: preset.lfo2.waveform = lfo_wave_sel; break;
        case 18: preset.lfo2.amp = val_norm; break;
        case 19: preset.lfo2.freq = 0.01f + (val_norm * 15.0f); break;

        // ==========================================
        // OSCILLATOR 1 (CC 20 - 23)
        // ==========================================
        case 20: preset.osc1.waveform = osc_wave_sel; break;
        case 21: preset.osc1.amp = val_norm; break;
        case 22: preset.osc1.shape = val_norm; break;
        case 23: preset.osc1.detune = (val_norm * 100.0f) - 50.0f; break; // Da -50 a +50 cents
        case 30: preset.osc1.octave = static_cast<uint8_t>(val_norm * 4.0f); break; // 0-4

        // ==========================================
        // OSCILLATOR 2 (CC 24 - 27)
        // ==========================================
        case 24: preset.osc2.waveform = osc_wave_sel; break;
        case 25: preset.osc2.amp = val_norm; break;
        case 26: preset.osc2.shape = val_norm; break;
        case 27: preset.osc2.detune = (val_norm * 100.0f) - 50.0f; break; // Da -50 a +50 cents
        case 31: preset.osc2.octave = static_cast<uint8_t>(val_norm * 4.0f); break; // 0-4

        // ==========================================
        // NOISE (CC 28 - 29)
        // ==========================================
        case 28: preset.noise.amp = val_norm; break;
        case 29: preset.noise.color = val_norm; break;

        // ==========================================
        // FILTER (CC 74, 71)
        // ==========================================
        case 74: preset.filter.cutoff = 20.0f + (val_norm * val_norm) * 20000.0f; break;
        case 71: preset.filter.resonance = val_norm * 1.8f; break; // Limite di sicurezza 0.95

        // ==========================================
        // AMP ENVELOPE (CC 73, 75, 79, 72, 80)
        // ==========================================
        case 73: preset.amp_env.attack  = 0.01f + (val_norm * 4.0f); break;
        case 75: preset.amp_env.decay   = 0.01f + (val_norm * 2.0f); break;
        case 79: preset.amp_env.sustain = val_norm; break;
        case 72: preset.amp_env.release = 0.01f + (val_norm * 3.0f); break;
        //case 80: preset.amp_env.amp     = val_norm; break;

        // ==========================================
        // FILTER ENVELOPE (CC 81 - 85)
        // ==========================================
        case 81: preset.filt_env.attack  = 0.01f + (val_norm * 4.0f); break;
        case 82: preset.filt_env.decay   = 0.01f + (val_norm * 2.0f); break;
        case 83: preset.filt_env.sustain = val_norm; break;
        case 84: preset.filt_env.release = 0.01f + (val_norm * 3.0f); break;
        case 85: preset.filt_env.amp     = val_norm; break;

        // ==========================================
        // REVERB (CC 91, 93, 94)
        // ==========================================
        case 91: preset.reverb.dryWet   = val_norm; break;
        case 93: preset.reverb.feedback = val_norm * 0.99f; break; // Mai oltre 0.99
        case 94: preset.reverb.lpFreq   = 500.0f + (val_norm * 17500.0f); break;

        //===========================================
        // XOR_MOD (CC 95)
        //===========================================
        case 95: preset.xor_m.amount = val_norm;

        // ==========================================
        // SWITCH (CC 100, 101)
        // ==========================================
        case 100: cc_value < 64 ? preset.sync = false : preset.sync = true; break;
        case 101: cc_value < 64 ? preset.ring = false : preset.ring = true; break;
        case 102: cc_value <64  ? preset.xor_m.isActive  = false : preset.xor_m.isActive  = true; break;
        
        // ==========================================
        // LFO DIRECTION (CC 102, 103)
        // ==========================================
        case 110: preset.lfo1.direction = static_cast<Direction_e>(cc_value / 21); break;
        case 111: preset.lfo2.direction = static_cast<Direction_e>(cc_value / 21); break;

        // CC non mappato -> esci senza richiamare SetActivePreset
        default: return; 
    }

    // Applica immediatamente le modifiche al motore in esecuzione
    SetActivePreset(preset);
}

void AudioEngine::Process(float& out_l, float& out_r) {
    const float s = 0.001f;

    Smooth(_smoothedPreset.filter.cutoff, _currentPreset.filter.cutoff, s);
    Smooth(_smoothedPreset.filter.resonance, _currentPreset.filter.resonance, s);
    Smooth(_smoothedPreset.osc1.amp, _currentPreset.osc1.amp, s);
    Smooth(_smoothedPreset.osc2.amp, _currentPreset.osc2.amp, s);
    Smooth(_smoothedPreset.noise.amp, _currentPreset.noise.amp, s);
    Smooth(_smoothedPreset.reverb.dryWet, _currentPreset.reverb.dryWet, s);

    // 1. Elaborazione LFO e Inviluppi
    float amp_env_out = _amp_env.Process(_lastGate);
    float filt_env_out = _filt_env.Process(_lastGate);

    // Salva sia il valore grezzo (-1 a 1) che quello scalato dall'ampiezza
    float lfo1_raw = _lfo1.Process();
    float lfo2_raw = _lfo2.Process();
    float lfo1_out = lfo1_raw * _currentPreset.lfo1.amp;
    float lfo2_out = lfo2_raw * _currentPreset.lfo2.amp;

    if (_currentPreset.sync) {
        if (_osc1.IsEOC()) _osc2.Reset();
    }

    // 2. Calcolo dei target per gli oscillatori (Valore Base + Modulazione)
    float target_shape1 = _currentPreset.osc1.shape;
    float target_shape2 = _currentPreset.osc2.shape;
    float target_detune1 = _currentPreset.osc1.detune;
    float target_detune2 = _currentPreset.osc2.detune;

    // LFO 1 Routing
    if (_currentPreset.lfo1.direction == Direction_e::SHAPE) target_shape1 += lfo1_out;
    if (_currentPreset.lfo1.direction == Direction_e::DETUNE) target_detune1 += lfo1_out * 50.0f;
    if (_currentPreset.lfo1.direction == XOR)  _xorMod.setMask(_currentPreset.xor_m.amount + lfo1_out);
    

    // LFO 2 Routing
    if (_currentPreset.lfo2.direction == Direction_e::SHAPE) target_shape2 += lfo2_out;
    if (_currentPreset.lfo2.direction == Direction_e::DETUNE) target_detune2 += lfo2_out * 50.0f;
    if (_currentPreset.lfo2.direction == XOR)  _xorMod.setMask(_currentPreset.xor_m.amount + lfo2_out);
    
    // Applica i limiti (Clamp) per evitare instabilità DSP e assegna
    _osc1.SetShape(daisysp::fclamp(target_shape1, 0.0f, 1.0f));
    _osc2.SetShape(daisysp::fclamp(target_shape2, 0.0f, 1.0f));
    _osc1.SetDetune(daisysp::fclamp(target_detune1, -50.0f, 50.0f));
    _osc2.SetDetune(daisysp::fclamp(target_detune2, -50.0f, 50.0f));
    _dust.SetDensity(daisysp::fclamp(_currentPreset.noise.color, 0.0f, 1.0f));

    // Generazione del suono
    float s_osc1 = _osc1.Process() * _smoothedPreset.osc1.amp;
    float s_osc2 = _osc2.Process() * _smoothedPreset.osc2.amp;
    float sNoise = _dust.Process() * _smoothedPreset.noise.amp;

    // Gestione delle modulazioni
    float sMix;
    float sXor;
    if (_currentPreset.ring) {
        sMix = s_osc1 * s_osc2;
        sXor =            0.0f;
    } 
    else if (_currentPreset.xor_m.isActive) {
        sMix = s_osc1;
        sXor = s_osc2;
    } 
    else {
        sMix = s_osc1 + s_osc2;
        sXor =            0.0f;
    }
    sMix  = _xorMod.process(sMix, sXor);
    sMix +=                      sNoise;
    
    // 3. Calcolo del Cutoff modulato e applicazione al filtro
    float target_cutoff = _smoothedPreset.filter.cutoff + (filt_env_out * _currentPreset.filt_env.amp * 10000.0f);
    if (_currentPreset.lfo1.direction == Direction_e::VCF) target_cutoff += lfo1_out * 10000.0f;
    if (_currentPreset.lfo2.direction == Direction_e::VCF) target_cutoff += lfo2_out * 10000.0f;
    
    _filt.SetFreq(daisysp::fclamp(target_cutoff, 20.0f, 20000.0f));

    float sFilt = _filt.Process(sMix);

    // 5. Applicazione VCA (Inviluppo di ampiezza + Tremolo corretto)
    float sDry = sFilt * amp_env_out * _currentPreset.amp_env.amp;
    
    // Correzione LFO su VCA: converte da bipolare (-1 a +1) a unipolare (0 a 1)
    if (_currentPreset.lfo1.direction == Direction_e::VCA) {
        float unipolar_lfo = (lfo1_raw + 1.0f) * 0.5f;
        sDry *= (1.0f - _currentPreset.lfo1.amp) + (unipolar_lfo * _currentPreset.lfo1.amp);
    }
    if (_currentPreset.lfo2.direction == Direction_e::VCA) {
        float unipolar_lfo = (lfo2_raw + 1.0f) * 0.5f;
        sDry *= (1.0f - _currentPreset.lfo2.amp) + (unipolar_lfo * _currentPreset.lfo2.amp);
    }

    // 6. Riverbero Stereo
    float revL = 0.0f;
    float revR = 0.0f;
    
    _reverb.Process(sDry, sDry, &revL, &revR);
    
    float revDryWet = _currentPreset.reverb.dryWet;
    
    // 7. Calcolo del mix stereo
    out_l = ((sDry * (1.0f - revDryWet)) + (revL * revDryWet)) * _masterVolume;
    out_r = ((sDry * (1.0f - revDryWet)) + (revR * revDryWet)) * _masterVolume;
}