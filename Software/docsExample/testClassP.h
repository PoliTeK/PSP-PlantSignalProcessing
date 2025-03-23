// all the comments with 3 forward slash are interpreted by doxygen as specific commands and will appear
// in the docs. This comment will not appear in the docs

/// @file testClassP.h
/// @brief Header file for testClass Parent

#ifndef TESTCLASSP_H
#define TESTCLASSP_H

/// @brief generic macro for testing doxy
#define GENERIC_MACRO 5 


/// @class testClassP
/// @brief prototype for the testClassP
class TestClassP {
public:
    /// @brief  default constructor
    TestClassP();    

    /// @brief parameter constructor
    /// @param a first parameter, SPECIFY HERE if it must be within a range
    /// @param b second parameter yadda yadda...
    TestClassP(int a, int b);

    /// @brief destructor (virtual because of parent class)
    virtual ~TestClassP();

private:
    // here doxygen comments will be unneessary: the private part of a class is not intended 
    // to be seen by a user so it will not appear in the docs.
    int _a;
    int _b;
};

#endif //TESTCLASSP_H