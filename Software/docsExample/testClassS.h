///@file testClassS.h
///@brief Header file for testClass son 
#ifndef TESTCLASSS_H
#define TESTCLASSS_H
 

#include "testClassP.h" 

/// @class TestClassS
/// @brief prototype of class Testclass Son
class TestClassS : public TestClassP {
public:

    /// @brief default constructor
    TestClassS();
    /// @brief destructor
    ~TestClassS();

    /// @brief generic setter
    /// @param a generic parameter
    void SetGenericParam(int a);
    
private:
    int _c;
};

#endif //TESTCLASS_H