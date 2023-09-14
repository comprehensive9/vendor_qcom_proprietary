#!/bin/sh

# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

QTI_LIMITS_HAL_COMPONENTS=(
    "vendor.qti.hardware.limits@1.0"
)

QTI_LIMITS_HAL_SRC_FOLDER=vendor/qcom/proprietary/interfaces

for i in "${QTI_LIMITS_HAL_COMPONENTS[@]}"
do
    echo "Update $i Android.bp"
    hidl-gen -Landroidbp -r vendor.qti:$QTI_IMS_HAL_SRC_FOLDER \
        -randroid.hidl:system/libhidl/transport $i
done

#hidl-gen -o vendor/qcom/proprietary/limits -Lc++-impl \
#	-r vendor.qti.hardware.limits:vendor/qcom/proprietary/interfaces/limits \
#	-randroid.hidl:system/libhidl/transport vendor.qti.hardware.limits@1.0
