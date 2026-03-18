#include "pentaPianta.h"


//rifare non costruttore ma pentapiantaBegin per implementare il controllo degli input
    pentaPianta::pentaPianta(int* soglieI,int f0I) { //constructor of the class 
        soglie=soglieI;                              //soglie is the vector of thresholds and has 7 int elements
                                                     //the first two ones (soglie[0] and soglie[1]) should be respectively the
                                                     //release and touch thresholds. For any plant the thresholds should be calibrated
        f0=f0I;                                      //Minimum output frequency: the first tone of the scale
    }



    int pentaPianta::initSoglie(int* soglieI) {     //Pay attention: pass a pointer!!!
        soglie = soglieI;
    }

    uint16_t pentaPianta::discFreq(uint16_t value) { //Value should be the sensor data (look at altroFile:riga)
        uint16_t frequenza=-1;                       //Declaration of the output variable
                                                     /*The function compares the read value against the thresholds 
                                                     and consequently assigns a tone in the pentatonic scale we're using
                                                      */
      if (soglie[0] <= value && value < soglie[2]) // soglie[0] otherwise we can have glitch on small value
{
    frequenza = f0;
}
else if (soglie[2] <= value && value < soglie[3])
{
    frequenza = f0 * 1.12;
}
else if (soglie[3] <= value && value < soglie[4])
{
    frequenza = f0 * 1.19;
}
else if (soglie[4] <= value && value < soglie[5])
{
    frequenza = f0 * 1.49;
}
else if (soglie[5] <= value && value < soglie[6])
{
    frequenza = f0 * 1.58;
}
else if (soglie[6] <= value && value < soglie[7])
{
    frequenza = f0 * 1.88;
}
else if (soglie[7] <= value)
{
    frequenza = f0 * 2.0;
}

return frequenza;

    }

    uint16_t pentaPianta::analogFreq(uint16_t value) {
        uint16_t frequenza=-1;

        frequenza = value*(f0*9/5-f0)/(soglie[6]-soglie[0]); //Scaling analog value in linear way 
        return frequenza;
    }
