#Copyright (c) 2019 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.
#!/bin/bash
TARGET=$1
tbname=`echo $TARGET | awk -F"_" '{print $1}'`
tbsuffix=`echo $TARGET | awk -F"$tbname" '{print $2}'`
VAR_DIR=$(echo $2 | sed 's/.*vendor\/qcom/vendor\/qcom/g')
TOP_LEVEL_MK="$VAR_DIR/Android.mk"
rm -rf $TOP_LEVEL_MK
echo 'LOCAL_PATH := $(call my-dir)' >> $TOP_LEVEL_MK
echo 'PREBUILT_DIR_PATH := $(LOCAL_PATH)' >> $TOP_LEVEL_MK
echo "-include \$(PREBUILT_DIR_PATH)/target/product/$TARGET/Android.mk" >> $TOP_LEVEL_MK
echo "-include \$(PREBUILT_DIR_PATH)/target/product/$TARGET/symlinks.mk" >> $TOP_LEVEL_MK

if [ -d "$VAR_DIR/target/product/qssi$tbsuffix" ];then
    echo "-include \$(PREBUILT_DIR_PATH)/target/product/qssi$tbsuffix/Android.mk" >> $TOP_LEVEL_MK
else
    echo "-include \$(PREBUILT_DIR_PATH)/target/product/qssi/Android.mk" >> $TOP_LEVEL_MK
fi
$(warning Succesfully created Dynamic Android.mk files.)
