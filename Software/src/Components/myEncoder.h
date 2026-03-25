#pragma once
#ifndef DSY_ENCODER_CUSTOM_H
#define DSY_ENCODER_CUSTOM_H

#include "daisy_core.h"
#include "per/gpio.h"
#include "hid/switch.h"

namespace daisy
{

class myEncoder
{
  public:
    myEncoder() {}
    ~myEncoder() {}

    void Init(Pin a,
          Pin b,
          Pin click,
              float update_rate = 0.f);

    void Debounce();

    int32_t Increment();

    inline bool RisingEdge() const { return sw_.RisingEdge(); }
    inline bool FallingEdge() const { return sw_.FallingEdge(); }
    inline bool Pressed() const { return sw_.Pressed(); }
    inline float TimeHeldMs() const { return sw_.TimeHeldMs(); }

  private:
    GPIO hw_a_;
    GPIO hw_b_;

    uint8_t last_ab_;
    int8_t  quarter_accum_;
    int32_t inc_;
    int32_t pending_inc_;

    Switch sw_;
};

} // namespace daisy

#endif