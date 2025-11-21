/// @file effect.hpp


// TODO implement variadic functionality
//      improve code readibilty with typedefs
//      divide implementation and definition

/// defines maximum number of param setter for a single block
#define MAX_N_OF_PARAM_SETTERS  4

/// @brief  template for signal chain block. Use it after instantiating the real effect
/// @tparam t_param1 type of first param (default = float)
/// @tparam t_param2 type of second param (default = float)
/// @tparam t_process type of process argument
template <typename t_param1 = float, typename t_param2 = float, typename t_process = float> 
class Effect {
private:
    
    // array of function pointers for setter of effect with single param
    int (*_SetParamFunction_ptr [MAX_N_OF_PARAM_SETTERS]) (t_param1 param1) = nullptr;
    
    // counter to keep track of latest addition to array
    unsigned int _setParamCounter = 0;
    
    // array of pointers to string containing param name
    const char* _paramNames[MAX_N_OF_PARAM_SETTERS] ;
    
    // array of function pointers for setter of effect with 2 params
    int (*_SetParamsFunction_ptr[MAX_N_OF_PARAM_SETTERS]) (t_param1 param1, t_param2 param2) = nullptr;
    
    // counter to keep track of latest addition to array
    unsigned int _setParamsCounter = 0;
    
    // array of string pointers containing the name of 2 param for the setters
    const char* _paramsNames [MAX_N_OF_PARAM_SETTERS][2];      
    
    // function pointer for process
    float (*_Process_ptr) (t_process) = nullptr;
public:

    Effect();

    /// @brief Parameter constructor for instantiating process function pointer
    /// @param Process_ptr process function pointer of original effect
    Effect(float (*Process_ptr)(t_process)){ addProcess(Process_ptr);}

    /// @brief Parameter constructor for instantiating parameter and process functions pointer
    /// @param Process_ptr process function pointer of original effect
    /// @param SetParamFunction_ptr function pointer of the setter of original effect
    Effect(float (*Process_ptr)(t_process), int (*SetParamFunction_ptr)(t_param1)){
        SetProcess(Process_ptr);
        AddSetParam(SetParamFunction_ptr);
    };
    
    /// @brief parameter constructor that sets process function, setter function and parameter name
    /// @param Process_ptr Function pointer to process 
    /// @param SetParamFunction_ptr Function pointer of setter
    /// @param paramName Pointer to string containing param name
    Effect( float (*Process_ptr)(t_process),
            int (*SetParamFunction_ptr)(t_param1), 
            const char* paramName)
    {
        SetProcess(Process_ptr);
        AddSetParam(SetParamFunction_ptr, paramName);
    }


    ~Effect();


    /// @brief adds the function pointer for a parameter setting
    /// @param SetParamFunction_ptr function pointer of the setter of original effect
    int AddSetParam (int (*SetParamFunction_ptr)(t_param1)) {
        if (_setParamCounter < MAX_N_OF_PARAM_SETTERS ){
            _SetParamFunction_ptr [_setParamCounter] = SetParamFunction_ptr;
            _setParamCounter++;
        }
        else {
            return -1;
        }         
    }

    /// @brief adds set param function to array and pointer to name of parameter
    /// @param SetParamFunction_ptr setParam function of original block
    /// @param paramName pointer to name of parameter
    /// @return -1 if no more functions can be added
    int AddSetParam (int (*SetParamFunction_ptr)(t_param1), const char* paramName) {
        if (_setParamCounter < MAX_N_OF_PARAM_SETTERS ){
            _SetParamFunction_ptr [_setParamCounter] = SetParamFunction_ptr;
            _paramNames[_setParamCounter] = paramName; 
            _setParamCounter++;
        }
        else {
            return -1;
        }         
    }

    /// @brief add function that sets two parameters
    /// @param SetParamsFunction_ptr pointer to function that sets two parameters
    /// @return -1 if full
    int AddSetParams(int(*SetParamsFunction_ptr)(t_param1,t_param2)){
        if(_setParamsCounter < MAX_N_OF_PARAM_SETTERS){
            _SetParamsFunction_ptr = SetParamsFunction_ptr;
            _setParamsCounter++;    
        }
        else {
            return -1;
        }
        
    }

    /// @brief  add function that sets two parameters, with given name
    /// @param SetParamsFunction_ptr pointer to function that sets parameters
    /// @param paramName1 name of parameter 1
    /// @param paramName2 name of parameter 2  
    /// @return -1 if full
    int AddSetParams(int(*SetParamsFunction_ptr)(t_param1,t_param2),
                     const char* paramName1,
                     const char* paramName2)
    {

        if(_setParamsCounter < MAX_N_OF_PARAM_SETTERS){
            _SetParamsFunction_ptr = SetParamsFunction_ptr;
            _paramsNames[_setParamsCounter][0] = paramName1; // position of first parameter
            _paramsNames[_setParamsCounter][1] = paramName2; // position of second parameter
            _setParamsCounter++;    
        }
        else {
            return -1;
        }        
    }

    /// @brief sets the function pointer for Process
    /// @param Process_ptr function pointer of Process of original effect
    inline void SetProcess (float (*Process_ptr)(t_process)) {_Process_ptr = Process_ptr;}
   
    
    /// @brief Process function
    /// @param sample sample to process
    /// @return processed sample
    inline float Process (t_process sample) {return _Process_ptr (sample);}
    
    /// @brief call set param function of original effect
    /// @param param1 new value of parameter
    /// @return errVal
    inline int SetParam (t_param1 param1) {return _SetParamFunction_ptr (param1);};
    
    /// @brief call set params function of original
    /// @param param1 new value of param1
    /// @param param2 new value of param2
    /// @return errVal
    inline int SetParams (t_param1 param1, t_param2 param2) {return _SetParamsFunction_ptr(param1, param2);}
};