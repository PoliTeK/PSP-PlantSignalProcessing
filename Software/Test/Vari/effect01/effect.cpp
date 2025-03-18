#include "Effect.h"


    void Effect::EffectBegin(int* thresholdsI){
        if (sizeof(_thresholds) / sizeof(_thresholds[0]) != 7){         //controls that the size of the input array is right when calibrating the sensor 
            std::cout<<"Numero sbagliato di soglie"<<std::endl;
        } else {
            _thresholds = thresholdsI;
        }
        
    }





    uint16_t Effect::analogCoeffFine(uint16_t value) { /*
                                                        _coeff is a fixed values array which serve as coefficients for the normalization.
                                                        This way we don't have a single function which normalizes for all the values,
                                                        But a normalization with a different slope for every thresholds interval,
                                                        because the values read from the various spots may not change linearly
                                                        to have a slightly better modulation
                                                        */
        uint16_t output=-1;
        if(_thresholds[0]<=value && value <_thresholds[2])     //_thresholds[0] otherwise we can have glitch on small value
        {                                                      
            output= value*(_coeff[1]-_coeff[0])/(_thresholds[2]-_thresholds[0]);   
        }   else if(_thresholds[2]<=value && value<_thresholds[3])                 
        {                                                                  
            output= value*(_coeff[2]-_coeff[1])/(_thresholds[3]-_thresholds[2]);   
        }   else if(_thresholds[3]<=value && value<_thresholds[4])
        {
            output= value*(_coeff[3]-_coeff[2])/(_thresholds[4]-_thresholds[3]);
        }   else if(_thresholds[4]<=value && value<_thresholds[5])
        {
            output= value*(_coeff[4]-_coeff[3])/(_thresholds[5]-_thresholds[4]);
        }   else if(_thresholds[5]<=value )
        {
            output= value*(_coeff[5]-_coeff[4])/(_thresholds[6]-_thresholds[5]);
        }
        if (output>1)  //if the values are out of scale, it returns the max
            return 1;
        return output;
    }

    uint16_t Effect::analogCoeffIgnorant(uint16_t value){ //stupid normalization with only a scale
        uint16_t output=-1;
        output = value*1/(_thresholds[6]-_thresholds[0]);
        if (output>1) 
            return 1;
        return output;
    }