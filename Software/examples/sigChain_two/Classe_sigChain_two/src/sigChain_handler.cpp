// TODO:
//      - divide implementation and declaration

#include "EffectInterface.cpp"
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
        return partial;
    }

    /// @brief Run the signal chain without passing a sample (use at own risk).
    /// useful if at start of chain there is an oscillator, or something that generates
    /// the sample for the rest of the chain
    /// @return processed sample
    float Process (){
        float dummySample = 0;
        for (unsigned int i = 0; i < _effectsCounter; i++){
            dummySample = _effects[i]->Process(dummySample);
        }
        return dummySample;
    }

    /// @brief get name of effect in a position 
    /// @param numOfEffect index of effect to get name
    /// @return pointer to name (read until \0) OR 0 if too high index
    const char* getEffectName (unsigned int numOfEffect) {
        if (numOfEffect < _effectsCounter){
            return _effects[numOfEffect]->getName();
        }
        else {
            return 0;
        } 
    }
};

