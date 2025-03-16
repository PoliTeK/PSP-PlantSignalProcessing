
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
    _shape = 0.0f;
    _mshape = 0.5f - _shape;
    _correctorGain = 1 / (1 - 2 * _shape);
    

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
    float out[n], t[n];
    for (int i = 0; i < n; i++)
    {
        switch (_waveform[i])
        {
        case WAVE_SIN:
            out[i] = sinf(_phase[i] * TWOPI_F);
            break;
        case WAVE_TRI:
            t[i] = abs(-1.0f + (2.0f * _phase[i]));
            if (abs(_phase[i] - 0.5) > _mshape)
            {
                t[i] -= 4 * t[i]; // qui c'è il problema, non è 4t la funzione corretta da somamre/aggiungere
            }
            if (abs(_phase[i] - 0.5) < _shape)
            {
                t[i] += 4 * t[i];
            }
            out[i] = _correctorGain *  (t[i] - 0.5f); // moltiplico per il gain correttivo  

            break;
        case WAVE_SAW:
            out[i] = -1.0f * (((_phase[i] * 2.0f)) - 1.0f);
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

    return out[0] * _amp[0] + out[1] * _amp[1];
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




void Ramo::SetAmp(float g[])
{
    for (int i = 0; i < n; i++)
    {
        _amp[i] = g[i];
    }
}

void Ramo::SetShape(float s)
{
    _shape = s * DC_MAX;
    _mshape = 0.5 - _shape;
    _correctorGain = 2 / (1 - 4 * _shape);
    _pw =0.05 +  s*0.75;
}
