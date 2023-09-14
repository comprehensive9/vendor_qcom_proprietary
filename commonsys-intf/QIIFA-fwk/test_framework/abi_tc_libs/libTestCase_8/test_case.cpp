////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "test_case.h"
#include <iostream>

Test1::Test1()
{
    std::cout << "Test object created" << std::endl;
}

Test1::~Test1()
{
    std::cout << "Test object destroyed" << std::endl;
}

int Test1::smallest(int s1, int s2)
{
    if (s1 < s2) { return s1; }
    else { return s2; }
}

Test2::Test2()
{
    std::cout << "Test object created" << std::endl;
}

Test3::Test3()
{
    std::cout << "Test object created" << std::endl;
}

Test::Test()
{
    std::cout << "Test object created" << std::endl;
}

int Test::add(int n1, int n2){
    return n1 + n2;
}

int Test::multiply(int n1, int n2){
    return n1 * n2;
}

int Test::enumMult(Test::A a){
    return a;
}

int Test::s = 10;
