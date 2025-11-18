// TODO implement variadic functionality

/// @brief  template for signal chain block. Use it after instantiating the real effect
/// @tparam t_param1 type of first param (default = float)
/// @tparam t_param2 type of second param (default = float)
/// @tparam t_process type of process argument
template <typename t_param1 = float, typename t_param2 = float, typename t_process = float> 
class Effect {
private:

    // function pointer for setter of effect
    int (*_SetParamFunction_ptr) (dataType1 param1) = nullptr;
    
    int (*_SetParamsFunction_ptr) (dataType1 param1, dataType2 param2) = nullptr;

    // function pointer for process
    float (*_Process_ptr) (t_process) = nullptr;
public:

    Effect();

    /// @brief Parameter constructor for instantiating process function pointer
    /// @param Process_ptr process function pointer of original effect
    Effect(float (*Process_ptr)(t_process)){ addProcess(Process_ptr)};

    /// @brief Parameter constructor for instantiating parameter and process functions pointer
    /// @param Process_ptr process function pointer of original effect
    /// @param SetParamFunction_ptr function pointer of the setter of original effect
    Effect(float (*Process_ptr)(t_process), int (*SetParamFunction_ptr)(t_param1)){
        addProcess(Process_ptr);
        addSetParam(SetParamFunction_ptr);
    };
    
    ~Effect();

    /// @brief adds the function pointer for a parameter setting
    /// @param SetParamFunction_ptr function pointer of the setter of original effect
    void addSetParam (int (*SetParamFunction_ptr)(t_param1)) {_SetParamFunction_ptr = SetParamFunction_ptr};

    void addSetParams(int(*SetParamsFunction_ptr)(t_param1,t_param2)){_SetParamsFunction_ptr = SetParamsFunction_ptr};

    /// @brief adds the function pointer for Process
    /// @param Process_ptr function pointer of Process of original effect
    void addProcess (float (*Process_ptr)(t_process)) {_Process_ptr = Process_ptr};
   
    /// @brief Process function
    /// @param sample sample to process
    /// @return processed sample
    float Process (t_process sample) {return _Process_ptr (sample)};
    
    int SetParam (t_param1 param1) {return _SetParamFunction_ptr (param1)};
    int SetParams (t_param1 param1, t_param2 param2) {return _SetParamsFunction_ptr(param1, param2)}
};