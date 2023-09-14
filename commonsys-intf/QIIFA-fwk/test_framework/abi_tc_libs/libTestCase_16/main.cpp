////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "test_case.h"
#include <iostream>


int main(){
    // Instantiating Class objects
    Test t;
    Test3 t3;
    Test1 t1;

    // Instantiating union
    t1.value.i = 10;
    t1.value.j = 10;

    // Running functions from Class Test
    int b = 1;
    int c = 0;
    int a = t.add(b,2);
    int d = t.enumMult(Test::v2);
    std::cout << d;
    t.s = t.s + 1; 
    return 0;
}
