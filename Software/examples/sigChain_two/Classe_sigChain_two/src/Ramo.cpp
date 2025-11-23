
#include "Ramo.h"

using namespace daisysp;

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
    _freq = 100.0f;
    _phase_inc[0] = CalcPhaseInc(_freq);
    _phase_inc[1] = CalcPhaseInc(_freq);
    _pw = 0.5f;
    _tresh = 1.0f;
    
    

    for (int i = 0; i < 2; i++) {
        _amp[i] = 0.5f;
        _phase[i] = 0.0f;
        _waveform[i] = WAVE_SIN;
        _eoc[i] = true;
        _eor[i] = true;
    }

}

float Ramo::Process()
{
    float out[n], t[n], s[n];
    for (int i = 0; i < n; i++)
    {
        switch (_waveform[i])
        {
        case WAVE_SIN:
            out[i] = sinf(_phase[i] * TWOPI_F);
            break;
        case WAVE_TRI:
            t[i] = abs(-1.0f + (2.0f * _phase[i]));
            t[i] = 2 *  (t[i] - 0.5f); 
            if (t[i] > _tresh)
            {
                out[i] = (- t[i] + 2 * _tresh) * 1/_tresh;
            }
            else if (t[i] < -_tresh)
            {
                out[i] = (- t[i] - 2 * _tresh) * 1/_tresh;
            }
            else
            {
                out[i] = t[i] *  1/_tresh;
            }

            break;
        case WAVE_SAW:
        if (_phase[i] > _c-0.05 && _phase[i] < 0.95-_c || _c==0)
        {
            out[i] =- 1.0f+((_phase[i] * 2.0f));
        }else{
            out[i] = 1.0f-(_phase[i] * 2.0f);
        }
            
            break;
        case WAVE_SQUARE:
            out[i] = _phase[i] < _pw ? (1.0f) : -1.0f;
            break;
        default:
            out[i] = 0.0f;
            break;
        }
        _phase[i] += _phase_inc[i];

        if (_phase[i] > 1.0f)
        {
            _phase[i] -= 1.0f;
            _eoc[i] = true;
        }
        else
        {
            _eoc[i] = false;
        }
        _eor[i] = (_phase[i] - _phase_inc[i] < 0.5f && _phase[i] >= 0.5f);
    }

    return _gain * (out[0] * _amp[0] + out[1] * _amp[1]);
}

float Ramo::CalcPhaseInc(float f)
{
    return f * _sr_recip;
}

void Ramo::SetWaveforms(uint8_t wf[])
{   
    for(int i = 0; i < n; i++)
    {
        _waveform[i] = wf[i] < WAVE_LAST ? wf[i] : WAVE_SIN;
    }
    
}

void Ramo::SetFreq( const float f)
{
    _freq = f;
    for (int i = 0; i < n; i++)
    {
        _phase_inc[i] = CalcPhaseInc(f);
    }
}

void Ramo::SetDetune(float ef[])
{
    for (int i = 0; i < n; i++)
    {
        _phase_inc[i] = CalcPhaseInc(_freq) * ef[i];
    }
}




void Ramo::SetAmp(float a[])
{
    for (int i = 0; i < n; i++)
    {
        _amp[i] = a[i];
    }
}

void Ramo::SetGain(float g)
{
    _gain = g;
}

void Ramo::SetShape(float s)
{
    _tresh = Th_MAX + s *(1-Th_MAX); ;
    _pw =  0.5f + s * 0.35f;
    _c = s* 0.25;
}
