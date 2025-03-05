#include "utilities.h"



float utilities::SmoothAnalogRead(unsigned int Pin, unsigned int res_type){
    _res_type = res_type < LAST_RES ? res_type : LOW_RES; // se res type non è tra i scelti metti bassa res

    float sum, out;
    unsigned int k = 16*4^_res_type; // k = 16 se res_type = 0 , 64 se // = 1 , 256 se // = 2

    for (int i = 0; i < k; i++){
        sum += hw.adc.Get(Pin); // hw è l'oggetto che contiene tutte le periferiche, Come faccio si che possano utilizzare le letture??
    }
    out = sum/k;
    return out;
}
    