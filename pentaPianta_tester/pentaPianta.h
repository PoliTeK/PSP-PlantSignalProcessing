//
// Created by flipb on 06/03/2025.
//

#ifndef PENTAPIANTA_H
#define PENTAPIANTA_H
#include <stdint.h>


class pentaPianta {
private:
    int* soglie;
    int f0;

public:
    pentaPianta(int* soglieI,int f0I);



    int initSoglie(int* soglieI);

    uint16_t discFreq(uint16_t value);

    uint16_t analogFreq(uint16_t value);
};



#endif //PENTAPIANTA_H
