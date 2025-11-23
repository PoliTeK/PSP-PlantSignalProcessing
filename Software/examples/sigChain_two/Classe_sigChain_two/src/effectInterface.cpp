#ifndef EFFECT_INTERFACE
#define EFFECT_INTERFACE
class EffectInterface {
public:
    virtual float Process (float sample);
    virtual void SetParam (float param);
};

#endif //EFFECT_INTERFACE