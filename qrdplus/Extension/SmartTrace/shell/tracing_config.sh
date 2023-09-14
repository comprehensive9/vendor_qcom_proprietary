#/bin/bash
#
# Copyright (c) 2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
if [ ! $1 ]; then
    exit -1
fi
tracing_on_path=/sys/kernel/tracing/tracing_on
prop_str="persist.vendor.sys.tracing_on"


traceOn=$(cat $tracing_on_path)
if [ "disable" = $1 ]; then
    setprop $prop_str $traceOn
    newValue=0
elif [ "restore" = $1 ]; then
    newValue=`getprop $prop_str`
else
    #invalid input
    exit -1
fi

if [ $traceOn != $newValue ]; then
   echo $newValue > $tracing_on_path
fi

