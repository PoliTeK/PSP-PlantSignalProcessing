
// I
class AudioEffect {
public:
    virtual float Process(float input) = 0;
    virtual void SetParam(int paramId, float value) = 0;
};

// Template adapter 
template<typename T>
class MCEffectAdapter : public AudioEffect {
private:
    T& wrappedEffect;
    
public:
    MCEffectAdapter(T& effect) : wrappedEffect(effect) {}
    
    float Process(float input) override {
        return wrappedEffect.Process(input);
    }
    
    void SetParam(int paramId, float value) override {
        wrappedEffect.SetParam(paramId, value);
    }
};