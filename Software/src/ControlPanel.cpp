#include "ControlPanel.h"
//#define DEBUG_MODE
void ControlPanel::Init(daisy::DaisySeed* hw) {
    _hw = hw;

    _pc.Init(IIR::Butterworth, _hw); 
    _pc.setCurve(_delta_max, _curve_type);
    _pc.setScale(PlantConditioner::C, PlantConditioner::Major);
    _pc.setOctave(4);


    _encoder.Init(_hw->GetPin(13), _hw->GetPin(14), _hw->GetPin(10));

}

void ControlPanel::HandleEncoder() {
    int32_t inc = _encoder.Increment();
    if (_encoder.RisingEdge()) _tarature_menu_index = (_tarature_menu_index + 1) % 3; // 3 modalità: DELTA, CURVE, IDLE
    if (inc != 0) {
        switch (_tarature_menu_index) {
            case MODE_DELTA:
                _delta_max += (inc * 5.0f);
                if (_delta_max < 1.0f)   _delta_max = 1.0f;
                if (_delta_max > 100.0f) _delta_max = 100.0f; 
                break;
            case MODE_CURVE:
                _curve_type += (inc * 0.1f);
                if (_curve_type < 0.1f) _curve_type = 0.1f; 
                if (_curve_type > 2.0f) _curve_type = 2.0f; 
                break;
            case MODE_IDLE:
                break;
        }
    }
    
}

void ControlPanel::UserInterface() {
    switch (_tarature_menu_index) {
        case MODE_DELTA:
            _hw->Print("Delta Max: %.1f\n", _delta_max);
            break;
        case MODE_CURVE:
            _hw->Print("Curve Type: %.1f\n", _curve_type);
            break;
        case MODE_IDLE:
            _hw->Print("Play Mode\n");
            if (_is_touched) {
                _hw->Print("Touched! Freq: %.1f Hz\n", _target_freq);
            } else {
                _hw->Print("Not Touched\n");
            }
            break;
    }
}

void ControlPanel::Update() {

    //PLANT CONDITIONER
    float calculated_freq = _pc.Process();

    if (_pc._cap.Touched() & _BV(0))
    {
        if (!_is_touched) _is_touched = true; 
        _target_freq = calculated_freq; 
        
    }else{
        if (_is_touched) _is_touched = false; 
    }

    //ENCODER
     _encoder.Debounce();
    HandleEncoder();

    //UI
    //UserInterface();
}




