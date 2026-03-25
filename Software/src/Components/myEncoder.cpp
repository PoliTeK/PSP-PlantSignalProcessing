#include "myEncoder.h"

using namespace daisy;

/* Quadrature transition table:
    index = (prev_ab << 2) | curr_ab, result = -1,0,+1 quarter-step. */
static const int8_t ENC_TABLE[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

void myEncoder::Init(Pin a,
                     Pin b,
                     Pin click,
                     float update_rate)
{
    hw_a_.Init(a, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    hw_b_.Init(b, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    sw_.Init(click,
             update_rate,
             Switch::TYPE_MOMENTARY,
             Switch::POLARITY_INVERTED,
             GPIO::Pull::PULLUP);

    const uint8_t a0 = hw_a_.Read();
    const uint8_t b0 = hw_b_.Read();
    last_ab_         = (a0 << 1) | b0;
    quarter_accum_   = 0;
    inc_             = 0;
    pending_inc_     = 0;
}

int32_t myEncoder::Increment()
{
    const int32_t out = pending_inc_;
    pending_inc_      = 0;
    return out;
}

void myEncoder::Debounce()
{
    const uint8_t a       = hw_a_.Read();
    const uint8_t b       = hw_b_.Read();
    const uint8_t curr_ab = (a << 1) | b;

    const uint8_t idx     = (last_ab_ << 2) | curr_ab;
    const int8_t  delta   = ENC_TABLE[idx];
    last_ab_              = curr_ab;

    // EC11 with 18 pulses / 36 detents usually produces two valid
    // quadrature transitions per mechanical click.
    inc_ = 0;
    if(delta != 0)
    {
        quarter_accum_ += delta;
        if(quarter_accum_ >= 2)
        {
            inc_ = 1;
            quarter_accum_ = 0;
        }
        else if(quarter_accum_ <= -2)
        {
            inc_ = -1;
            quarter_accum_ = 0;
        }
    }

    pending_inc_ += inc_;

    sw_.Debounce();
}