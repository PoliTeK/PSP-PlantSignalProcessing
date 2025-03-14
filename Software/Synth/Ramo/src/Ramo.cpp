#include "dsp.h"
#include "Ramo.h"

Ramo::Ramo()
{
}

Ramo::~Ramo()
{
    // Cleanup if needed
}

void Ramo::Init(float sample_rate) // initialization
{
    _sr = sample_rate;
    _sr_recip = 1.0f / sample_rate;
    //__addgsbyte = 100.0f;
    _detune = 1.0f;
    _freq = 100.0f;
    _phase_inc = CalcPhaseInc(_freq);
    

    for (int i = 0; i < 2; i++) {
        _amp[i] = 0.5f;
        _pw[i] = 0.5f;
        _shape[i] = 0.0f;
        _mshape[i] = 0.5f - _shape[i];
        _phase[i] = 0.0f;
        _waveform[i] = WAVE_SIN;
        _eoc[i] = true;
        _eor[i] = true;
        _correctorGain[i] = 1 / (1 - 2 * _shape[i]);
    }

}

float Ramo::Process()
{
    float out[n], t[n];
    for (int i = 0; i < n; i++)
    {
        switch (_waveform[i])
        {
        case WAVE_SIN:
            out[i] = sinf(_phase[i] * TWOPI_F);
            break;
        case WAVE_TRI:
            t[i] = -1.0f + (2.0f * _phase[i]);
            if (abs(_phase[i] - 0.5) > _mshape[i] || abs(_phase[i] - 0.5) < _shape[i])
            {
                t[i] -= 4 * t[i];
            }                                                 // sottraggo nei casi dentro il DC di fold
            out[i] = _correctorGain[i] * (fabsf(t[i]) - 0.5f); // moltiplico per il gain corr

            break;
        case WAVE_SAW:
            out[i] = -1.0f * (((_phase[i] * 2.0f)) - 1.0f);
            break;
        case WAVE_SQUARE:
            out[i] = _phase[i] < _pw[i] ? (1.0f) : -1.0f;
            break;
        default:
            out[i] = 0.0f;
            break;
        }
        _phase[i] += _phase_inc;

        if (_phase[i] > 1.0f)
        {
            _phase[i] -= 1.0f;
            _eoc[i] = true;
        }
        else
        {
            _eoc[i] = false;
        }
        _eor[i] = (_phase[i] - _phase_inc < 0.5f && _phase[i] >= 0.5f);
    }

    return out[1] * _amp[1] + out[2] * _amp[2];
}

float Ramo::CalcPhaseInc(float f)
{
    return f * _sr_recip;
}

void Ramo::SetWaveforms(uint8_t wf1, uint8_t wf2)
{
    _waveform[0] = wf1 < WAVE_LAST ? wf1 : WAVE_SIN;
    _waveform[1] = wf2 < WAVE_LAST ? wf2 : WAVE_SIN;
}

void Ramo::SetFreq( const float f)
{
    _freq = f;
    for (int i = 0; i < n; i++)
    {
        _phase_inc = CalcPhaseInc(f);
    }
}

void Ramo::SetDetune(float ef)
{
    _detune = ef;
    _phase_inc = _phase_inc * ef;
}

void Ramo::SetAmp(float g1, float g2)
{
    _amp[0] = g1;
    _amp[1] = g2;
}

void Ramo::SetShape(float s1, float s2)
{
    _shape[0] = s1 * DC_MAX;
    _shape[1] = s2 * DC_MAX;
    _mshape[0] = 0.5 - _shape[0];
    _mshape[1] = 0.5 - _shape[1];

    _correctorGain[0] = 1 / (1 - 2 * _shape[0]);
    _correctorGain[1] = 1 / (1 - 2 * _shape[1]);

    _pw[0] = s1;
    _pw[1] = s2;
}
