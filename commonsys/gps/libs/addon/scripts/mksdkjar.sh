#!/bin/bash
#==========================================================================
# Copyright (c) 2013, 2015, 2017, 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# 2013 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#
#==========================================================================
#
#==========================================================================
#
# Usage:
#     mksdkjar.sh [output-path]
#
# Note, this script requires the existence of the build binary -
# out/target/common/obj/JAVA_LIBRARIES/framework-minus-apex_intermediates/classes.jar
# out/soong/.intermediates/vendor/qcom/proprietary/commonsys/gps/libs/glue/izat.lib.glue/android_common/javac/classes
# out/soong/.intermediates/vendor/qcom/proprietary/commonsys/gps/libs/addon/com.qti.location.sdk/android_common/javac/classes
# This script can be called from any directory
# It's also called automatically from com.qti.location.sdk make file
#==========================================================================

DIR="$( cd "$( dirname "$0" )" && pwd )"
CMD=${0##*/}
OUT="/tmp/izatsdk"

# Common input folders
OUT_JAVA_LIBRARIES="$DIR/../../../../../../../../out/target/common/obj/JAVA_LIBRARIES"
echo $OUT_JAVA_LIBRARIES
GLUE_JAVA="$DIR/../../glue/java"
COMPILED_AIDL_CLASSES="$DIR/../../../../../../../../out/soong/.intermediates/vendor/qcom/proprietary/commonsys/gps/libs/glue/izat.lib.glue/android_common/javac/classes"
COMPILED_SDK_CLASSES="$DIR/../../../../../../../../out/soong/.intermediates/vendor/qcom/proprietary/commonsys/gps/libs/addon/com.qti.location.sdk/android_common/javac/classes"

# Show help message if requested, otherwise create output folder
if [ -n "$1" ]
then
    if [ "$1" == "-h" ] || [ "$1" == "--help" ]
    then
        echo "$0 [output-path]"
        echo "  e.g. $0"
        echo "       $0 /tmp/sdk"
        exit
    else
        OUT=$1
        if [ ! -d $OUT ]
        then
            mkdir -p $OUT
        fi
    fi
fi

if [ ! -e $COMPILED_AIDL_CLASSES ]
then
    COMPILED_AIDL_CLASSES="$OUT_JAVA_LIBRARIES/izat.lib.glue_intermediates/aidl/java"
fi

# Check dependencies
if [[ ! -e $OUT_JAVA_LIBRARIES/framework-minus-apex_intermediates/classes.jar ||
      ! -e $COMPILED_AIDL_CLASSES  || ! -e $COMPILED_SDK_CLASSES ]]
then
    echo "This script requires com.qualcomm.location frameworks to have been built."
    exit 1
fi

# Build class files
mkdir -p $OUT/classes

cp -r $COMPILED_SDK_CLASSES $OUT
cp -r $COMPILED_AIDL_CLASSES $OUT

if [ ! -e $OUT/classes/com/qti/location/sdk/IZatManager.class ]
then
    echo "Error building SDK class files."
    exit 2
fi
jar cvf $OUT/izatsdk.jar -C $OUT/classes .
# Build javadoc files
rm -rf $OUT/docs
mkdir -p $OUT/docs
javadoc -d $OUT/docs -sourcepath $DIR/../java/ -classpath $OUT_JAVA_LIBRARIES/frameworks-core-util-lib_intermediates/classes.jar:$OUT_JAVA_LIBRARIES/framework-minus-apex_intermediates/classes.jar:$OUT/izatsdk.jar com.qti.location.sdk $GLUE_JAVA/com/qti/debugreport/*.java $GLUE_JAVA/com/qti/wifidbreceiver/*.java $GLUE_JAVA/com/qti/wwandbreceiver/*.java



if [ ! -e $OUT/docs/com/qti/location/sdk/IZatManager.html ]
then
    echo "Error building SDK javadoc files."
    exit 3
fi


# Remove built class files
rm -rf $OUT/classes

tar cjvf $OUT/docs.tbz -C $OUT/ docs

echo
echo "SDK library built at: $OUT/izatsdk.jar"
echo "Java docs for sdk available at: $OUT/docs"
