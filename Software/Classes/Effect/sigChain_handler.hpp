// TODO:
//      - divide implementation and declaration

#include "EffectInterface.hpp"
#define MAX_NUM_OF_BLOCKS 8 

/// @brief Handler of the signal chain
class sigChain_handler {

private:
    EffectInterface* _effects [MAX_NUM_OF_BLOCKS];
    unsigned int _effectsCounter = 0;

public:
    
    /// @brief add effect to the signal chain
    /// @param effect pointer to effect 
    /// @return one if full
    int addEffect (EffectInterface* effect){
        int oneIfFull = 0;
        if (_effectsCounter < MAX_NUM_OF_BLOCKS){
             _effects[_effectsCounter] = effect;
             _effectsCounter++;
         } 
         else {
            oneIfFull = 1;
         }
        return oneIfFull;
    }


    /// @brief Process the sample going through the signal chain
    /// @param sample sample to process through signal chain
    /// @return processed sample
    float Process (float sample){
        float partial = sample;
        for (unsigned int i = 0; i < _effectsCounter; i++){
            partial = _effects[i]->Process(partial);
        }
    }
};

