////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DUMMY_h
#define DUMMY_h
#include <vector>

class Test1
{
  protected:
    char u;
  public:
    Test1();
    virtual ~Test1();
    virtual int smallest(int s1, int s2);
    union sample
    {
      char ch;
      int i;
      long j;
    } value;
};

class Test2
{
  public:
    Test2();
    struct random
    {
     int st1;
     int st2;
    } st;
};

class Test3
{
  public:
    Test3();
};

class Test : public Test1, public Test2
{
  public:
    int x;
  public:
    Test();
    static int s;
    char z;
    enum A { v1, v2, v3 };
    int add(int n1, int n2);
    int multiply(int n1, int n2);
    int enumMult(enum A);
};

#endif
