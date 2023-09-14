#
# Copyright (c) 2019 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
cmake_minimum_required(VERSION 3.15)
PROJECT(qcRilMarshalling CXX)

add_library(qcRilMarshalling SHARED
    ../../src/Dial.cpp
    )

##target_include_directories(qcRilMarshalling PRIVATE
#    ../../../interfaces/inc
#    ../../inc
#    inc
#    )
target_compile_options(qcRilMarshalling PRIVATE /std:c++17)
