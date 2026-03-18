#include "pentaPianta.h"

pentaPianta::pentaPianta() {};
//rifare non costruttore ma pentapiantaBegin per implementare il controllo degli input
    pentaPianta::pentaPianta(int* soglieI,int f0I) { //constructor of the class 
        _soglie=soglieI;                              //soglie is the vector of thresholds and has 7 int elements
                                                     //the first two ones (soglie[0] and soglie[1]) should be respectively the
                                                     //release and touch thresholds. For any plant the thresholds should be calibrated
        _f0=f0I;                                      //Minimum output frequency: the first tone of the scale
        _count=0;
    }



    int pentaPianta::begin(int* soglieI,int f0I) {     //Pay attention: pass a pointer!!!
        _soglie = soglieI;
        _f0=f0I;
        _count=0;                              //The gate is closed at the beginning
        _raw_value=0;                               //The raw value is set to 0 at the beginning
    }

    uint16_t pentaPianta::discFreq(uint16_t value) { //Value should be the sensor data (look at altroFile:riga)
        uint16_t frequenza=-1;                       //Declaration of the output variable
                                                     /*The function compares the read value against the thresholds 
                                                     and consequently assigns a tone in the pentatonic scale we're using
                                                      */
        if(_soglie[0]<=value && value <_soglie[2])     //soglie[0] otherwise we can have glith on small value
        {
            frequenza= _f0;
        }else if(_soglie[2]<=value && value<_soglie[3])
        {
            frequenza= _f0*6/5;
        }else if(_soglie[3]<=value && value<_soglie[4])
        {
            frequenza= _f0*4/3;
        }else if(_soglie[4]<=value && value<_soglie[5])
        {
            frequenza= _f0*3/2;
        }else if(_soglie[5]<=value )
        {
            frequenza= _f0*9/5;
        }//mandare nello switch
        return frequenza;
    }

    uint16_t pentaPianta::analogFreq(uint16_t value) {
        uint16_t frequenza=-1;

        frequenza = value*(_f0*9/5-_f0)/(_soglie[6]-_soglie[0]); //Scaling analog value in linear way 
        return frequenza;
    }
    int pentaPianta::getFreq() {
        int frequenza=-1;
        if(_count<20)     //soglie[0] otherwise we can have glith on small value
        {
            frequenza = discFreq(_raw_value);
        }else
        {
            frequenza= analogFreq(_raw_value);
        }
        return frequenza;
    }
