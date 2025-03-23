
/// @file testClassP.cpp
/// @brief implementation of the class TestClass Parent (just for example)
#include "testClassP.h"

TestClassP::TestClassP(){

}

TestClassP::TestClassP(int a, int b) : TestClassP(){
    this->_a = a;
    this->_b = b; 
}

TestClassP::~TestClassP(){

}

