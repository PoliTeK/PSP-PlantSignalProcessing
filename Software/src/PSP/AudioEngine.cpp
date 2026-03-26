#include "AudioEngine.h"
#include "AudioEngineConfig.h"

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}

void AudioEngine::Init(float sample_rate) {
    // Inizializzazione moduli
    _osc1.Init(sample_rate);
    _osc2.Init(sample_rate);
    _lfo1.Init(sample_rate);
    _lfo2.Init(sample_rate);
    _Amp_env.Init(sample_rate);
    _Filt_env.Init(sample_rate);
    _Filt.Init(sample_rate);

    _lastGate = false;
    _currentFreq = 440.0f;
    
    // Configura un preset di default
    SetPreset(PRESET_LEAD);
}

void AudioEngine::SetPreset(SynthPreset preset) {
    _currentPreset = preset;

    // 1. Popolamento delle struct in base al preset
    switch (preset) {
        case PRESET_PAD:
            _osc1Param.Waveform = pad_waveform1;
            _osc1Param.Amp      = pad_amp1;
            _osc1Param.Shape    = pad_shape1;
            
            _osc2Param.Waveform = pad_waveform2;
            _osc2Param.Amp      = pad_amp2;
            _osc2Param.Shape    = pad_shape2; 

            _lfo1Param.Waveform = lfo_pad_waveform1;
            _lfo1Param.Amp      = lfo_pad_Amp1;
            _lfo1Param.Shape    = lfo_pad_Shape1;

            _lfo2Param.Waveform = lfo_pad_waveform2;
            _lfo2Param.Amp      = lfo_pad_Amp2;
            _lfo2Param.Shape    = lfo_pad_Shape2;

            _Amp_envParam.Attack  = pad_attack;
            _Amp_envParam.Decay   = pad_decay;
            _Amp_envParam.Release = pad_release;
            _Amp_envParam.Sustain = pad_sustain;

            _Filt_envParam.Attack  = pad_fattack;
            _Filt_envParam.Decay   = pad_fdecay;
            _Filt_envParam.Release = pad_frelease;
            _Filt_envParam.Sustain = pad_fsustain;

            _FiltParam.Cutoff    = pad_cutoff_freq;
            _FiltParam.Resonance = pad_res;
            break;

        case PRESET_PLUCK:
            _osc1Param.Waveform = pluck_waveform1;
            _osc1Param.Amp      = pluck_amp1;
            _osc1Param.Shape    = pluck_shape1;

            _osc2Param.Waveform = pluck_waveform2;
            _osc2Param.Amp      = pluck_amp2;
            _osc2Param.Shape    = pluck_shape2;

            _lfo1Param.Waveform = lfo_pluck_waveform1;
            _lfo1Param.Amp      = lfo_pluck_Amp1;
            _lfo1Param.Shape    = lfo_pluck_Shape1;

            _lfo2Param.Waveform = lfo_pluck_waveform2;
            _lfo2Param.Amp      = lfo_pluck_Amp2;
            _lfo2Param.Shape    = lfo_pluck_Shape2;

            _Amp_envParam.Attack  = pluck_attack;
            _Amp_envParam.Decay   = pluck_decay;
            _Amp_envParam.Release = pluck_release;
            _Amp_envParam.Sustain = pluck_sustain;

            _Filt_envParam.Attack  = pluck_fattack;
            _Filt_envParam.Decay   = pluck_fdecay;
            _Filt_envParam.Release = pluck_frelease;
            _Filt_envParam.Sustain = pluck_fsustain;

            _FiltParam.Cutoff    = pluck_cutoff_freq;
            _FiltParam.Resonance = pluck_res;
            break;

        case PRESET_LEAD:
            _osc1Param.Waveform = lead_waveform1;
            _osc1Param.Amp      = lead_amp1;
            _osc1Param.Shape    = lead_shape1;

            _osc2Param.Waveform = lead_waveform2;
            _osc2Param.Amp      = lead_amp2;
            _osc2Param.Shape    = lead_shape2;

            _lfo1Param.Waveform = lfo_lead_waveform1;
            _lfo1Param.Amp      = lfo_lead_Amp1;
            _lfo1Param.Shape    = lfo_lead_Shape1;

            _lfo2Param.Waveform = lfo_lead_waveform2;
            _lfo2Param.Amp      = lfo_lead_Amp2;
            _lfo2Param.Shape    = lfo_lead_Shape2;

            _Amp_envParam.Attack  = lead_attack;
            _Amp_envParam.Decay   = lead_decay;
            _Amp_envParam.Release = lead_release;
            _Amp_envParam.Sustain = lead_sustain;

            _Filt_envParam.Attack  = lead_fattack;
            _Filt_envParam.Decay   = lead_fdecay;
            _Filt_envParam.Release = lead_frelease;
            _Filt_envParam.Sustain = lead_fsustain;
            _Filt_envParam.Amp = lead_fAmp;

            _FiltParam.Cutoff    = lead_cutoff_freq;
            _FiltParam.Resonance = lead_res;
            break;

        default:
            break;
    }

    // 2. Applicazione dei parametri estratti agli oggetti DSP
    _osc1.SetWaveform(_osc1Param.Waveform);
    _osc1.SetAmp(_osc1Param.Amp);
    _osc1.SetPw(_osc1Param.Shape);
    
    _osc2.SetWaveform(_osc2Param.Waveform);
    _osc2.SetAmp(_osc2Param.Amp);
    _osc2.SetPw(_osc2Param.Shape);

    _lfo1.SetWaveform(_lfo1Param.Waveform);
    _lfo1.SetAmp(_lfo1Param.Amp);
    _lfo1.SetPw(_lfo1Param.Shape);
    _lfo1.SetFreq(_lfo1Param.Freq);
    
    _lfo2.SetWaveform(_lfo2Param.Waveform);
    _lfo2.SetAmp(_lfo2Param.Amp);
    _lfo2.SetPw(_lfo2Param.Shape);
    _lfo2.SetFreq(_lfo2Param.Freq);

    _Amp_env.SetTime(daisysp::ADSR_SEG_ATTACK,  _Amp_envParam.Attack);
    _Amp_env.SetTime(daisysp::ADSR_SEG_DECAY,   _Amp_envParam.Decay);
    _Amp_env.SetTime(daisysp::ADSR_SEG_RELEASE, _Amp_envParam.Release);
    _Amp_env.SetSustainLevel(_Amp_envParam.Sustain);

    _Filt_env.SetTime(daisysp::ADSR_SEG_ATTACK,  _Filt_envParam.Attack);
    _Filt_env.SetTime(daisysp::ADSR_SEG_DECAY,   _Filt_envParam.Decay);
    _Filt_env.SetTime(daisysp::ADSR_SEG_RELEASE, _Filt_envParam.Release);
    _Filt_env.SetSustainLevel(_Filt_envParam.Sustain);
    
    _Filt.SetFreq(_FiltParam.Cutoff);
    _Filt.SetRes(_FiltParam.Resonance);
}

void AudioEngine::Update(ControlsStruct Controls) {

    //Lead
    _osc1Param.Freq = Controls.freq;
    _osc2Param.Freq = Controls.freq *2.0f;

    if (Controls.gate && !_lastGate) {
        _Amp_env.Retrigger(false);
        _Filt_env.Retrigger(false);
    }
    _lastGate = Controls.gate;
}

float AudioEngine::Process() {
    float Amp_env_out;
    float Fenv_out;
    float sig;
    float filtered_sig;

    switch (_currentPreset)
    {
    case PRESET_PAD://TODO
        _osc1.SetFreq(_osc1Param.Freq * (1 +  _lfo1.Process()));
        _osc2.SetFreq(_osc2Param.Freq * (1 +  _lfo2.Process()));
        Amp_env_out = _Amp_env.Process(_lastGate);
        Fenv_out = _Filt_env.Process(_lastGate);
        _Filt.SetFreq(lead_cutoff_freq * Fenv_out * _Filt_envParam.Amp);
        break;
    case PRESET_PLUCK://TODO
        _osc1.SetFreq(_osc1Param.Freq * (1 +  _lfo1.Process()));
        _osc2.SetFreq(_osc2Param.Freq * (1 +  _lfo2.Process()));
        Amp_env_out = _Amp_env.Process(_lastGate);
        Fenv_out = _Filt_env.Process(_lastGate);
        _Filt.SetFreq(lead_cutoff_freq * Fenv_out * _Filt_envParam.Amp);
        break;
    case PRESET_LEAD:
        _osc1.SetFreq(_osc1Param.Freq * (1 +  _lfo1.Process()));
        _osc2.SetFreq(_osc2Param.Freq * (1 +  _lfo2.Process()));
        Amp_env_out = _Amp_env.Process(_lastGate);
        Fenv_out = _Filt_env.Process(_lastGate);
        _Filt.SetFreq(lead_cutoff_freq * Fenv_out * _Filt_envParam.Amp);
        break;
    
    default:
        break;
    }

    sig = (_osc1.Process() + _osc2.Process())/2.0f;
    sig *= Amp_env_out;
    filtered_sig = _Filt.Process(sig);
    return  filtered_sig;
}