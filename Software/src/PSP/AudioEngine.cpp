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

    // 1. Popolamento delle struct in base al preset
    switch (preset) {
        case PRESET_PAD:
            _osc1Param.Waveform = pad_waveform;
            _osc1Param.Amp      = pad_amp1;
            _osc1Param.Shape    = pad_shape1;
            
            _osc2Param.Waveform = pad_waveform2;
            _osc2Param.Amp      = pad_amp2;
            _osc2Param.Shape    = pad_shape1; // Usato shape1 come da tue dichiarazioni

            _lfo1Param.Waveform = lfo_pad_waveform1;
            _lfo1Param.Amp      = lfo_pad_Amp1;
            _lfo1Param.Shape    = lfo_pad_Shape1;

            _lfo2Param.Waveform = lfo_pad_waveform2;
            _lfo2Param.Amp      = lfo_pad_Amp2;
            _lfo2Param.Shape    = lfo_pad_Shape2;

            _AmpADSR.Attack  = pad_attack;
            _AmpADSR.Decay   = pad_decay;
            _AmpADSR.Release = pad_release;
            _AmpADSR.Sustain = pad_sustain;

            _FiltADSR.Attack  = pad_fattack;
            _FiltADSR.Decay   = pad_fdecay;
            _FiltADSR.Release = pad_frelease;
            _FiltADSR.Sustain = pad_fsustain;

            _Filt.Cutoff    = pad_cutoff_freq;
            _Filt.Resonance = pad_res;
            break;

        case PRESET_PLUCK:
            _osc1Param.Waveform = pluck_waveform;
            _osc1Param.Amp      = pluck_amp1;
            _osc1Param.Shape    = pluck_shape1;

            _osc2Param.Waveform = pluck_waveform2;
            _osc2Param.Amp      = pluck_amp2;
            _osc2Param.Shape    = pluck_shape1;

            _lfo1Param.Waveform = lfo_pluck_waveform1;
            _lfo1Param.Amp      = lfo_pluck_Amp1;
            _lfo1Param.Shape    = lfo_pluck_Shape1;

            _lfo2Param.Waveform = lfo_pluck_waveform2;
            _lfo2Param.Amp      = lfo_pluck_Amp2;
            _lfo2Param.Shape    = lfo_pluck_Shape2;

            _AmpADSR.Attack  = pluck_attack;
            _AmpADSR.Decay   = pluck_decay;
            _AmpADSR.Release = pluck_release;
            _AmpADSR.Sustain = pluck_sustain;

            _FiltADSR.Attack  = pluck_fattack;
            _FiltADSR.Decay   = pluck_fdecay;
            _FiltADSR.Release = pluck_frelease;
            _FiltADSR.Sustain = pluck_fsustain;

            _Filt.Cutoff    = pluck_cutoff_freq;
            _Filt.Resonance = pluck_res;
            break;

        case PRESET_LEAD:
            _osc1Param.Waveform = lead_waveform1;
            _osc1Param.Amp      = lead_amp1;
            _osc1Param.Shape    = lead_shape1;

            _osc2Param.Waveform = lead_waveform2;
            _osc2Param.Amp      = lead_amp2;
            _osc2Param.Shape    = lead_shape1;

            _lfo1Param.Waveform = lfo_lead_waveform1;
            _lfo1Param.Amp      = lfo_lead_Amp1;
            _lfo1Param.Shape    = lfo_lead_Shape1;

            _lfo2Param.Waveform = lfo_lead_waveform2;
            _lfo2Param.Amp      = lfo_lead_Amp2;
            _lfo2Param.Shape    = lfo_lead_Shape2;

            _AmpADSR.Attack  = lead_attack;
            _AmpADSR.Decay   = lead_decay;
            _AmpADSR.Release = lead_release;
            _AmpADSR.Sustain = lead_sustain;

            _FiltADSR.Attack  = lead_fattack;
            _FiltADSR.Decay   = lead_fdecay;
            _FiltADSR.Release = lead_frelease;
            _FiltADSR.Sustain = lead_fsustain;

            _Filt.Cutoff    = lead_cutoff_freq;
            _Filt.Resonance = lead_res;
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
    
    _lfo2.SetWaveform(_lfo2Param.Waveform);
    _lfo2.SetAmp(_lfo2Param.Amp);
    _lfo2.SetPw(_lfo2Param.Shape);

    _Aenv.SetTime(daisysp::ADSR_SEG_ATTACK,  _AmpADSR.Attack);
    _Aenv.SetTime(daisysp::ADSR_SEG_DECAY,   _AmpADSR.Decay);
    _Aenv.SetTime(daisysp::ADSR_SEG_RELEASE, _AmpADSR.Release);
    _Aenv.SetSustainLevel(_AmpADSR.Sustain);

    _Fenv.SetTime(daisysp::ADSR_SEG_ATTACK,  _FiltADSR.Attack);
    _Fenv.SetTime(daisysp::ADSR_SEG_DECAY,   _FiltADSR.Decay);
    _Fenv.SetTime(daisysp::ADSR_SEG_RELEASE, _FiltADSR.Release);
    _Fenv.SetSustainLevel(_FiltADSR.Sustain);
    
    _filter.SetFreq(_Filt.Cutoff);
    _filter.SetRes(_Filt.Resonance);
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
    float filtered_sig;

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
    filtered_sig = _filter.Process(sig);
    return  filered_sig;
}