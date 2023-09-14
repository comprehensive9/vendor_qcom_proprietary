#/bin/bash

##################################################################################################
## slim_pb_generate_script.sh
## Description:
## Util script to compile the proto files using protoc to generate the header and C file.
## This is intended to be run once you have made a full build and if any changes are made to proto
## files or if any new proto files are added. This relieves the burden of developer in finding the
## path to protobuf work folder and running those commands.
##
## This script is also used in pre compile stage to compile the proto files so that the generated
## C and header file are based on the protoc compiler for that workspace (LV or LE).
##
## For more details, pls refer the below link
##          - https://developers.google.com/protocol-buffers/docs/reference/cpp-generated
##
## Note:
## 1. Add any new .proto files to "src_proto_files" array
## 2. If protobuf package version is updated, update the values in this script too (variable -
##    protobuf_package_ver).
##
##  Copyright (c) 2020 Qualcomm Technologies, Inc.
##  All Rights Reserved.
##  Confidential and Proprietary - Qualcomm Technologies, Inc.
##################################################################################################

echo -e "\slim_pb_generate_script.sh"
echo      "***************************"
# find workspace path
wsp_path=`repo info platform/hardware/qcom/gps 2>&1|grep -o "Mount path:.*"|sed 's/Mount path: //'|sed 's/hardware\/qcom\/gps//'`
echo "Workspace path is: $wsp_path"
# find manifest
manifest_info=`repo info . 2>/dev/null|grep "Manifest merge branch"|sed 's/Manifest merge branch: refs\/[a-z]*\///'`
echo "Manifest branch/AU is: $manifest_info"

if [ "$wsp_path" == "" ]
then
    ## this happens when this script runs as part of bitbake compilation in do_compile_prepend
    echo "Wsp path is NULL"
    if [ "$WORKSPACE" != "" ];then
        # LE targets has root path in $WORKSPACE
        wsp_path=$WORKSPACE"/"
    elif [ "$SRC_DIR_ROOT" != "" ];then
        # LV targets has root path in $SRC_DIR_ROOT
        wsp_path=$SRC_DIR_ROOT"/"
    else
        ## Compiling in <ROOT>/poky/build/tmp-glibc/work/sa8155qdrive-agl-linux/slim-daemon/git-r1/gps/slim/
        ## Compiling in <ROOT>/poky/build/tmp-glibc/work/sa415m-oe-linux-gnueabi/eng-hub-msg-proto/git-r1/gps-noship/eng-hub-msg-proto/
        ## 9 level up - <ROOT> of workspace
        wsp_path="../../../../../../../../../"
    fi
    echo "do_compile_prepend: Wsp path is $wsp_path"
fi

## LV target
## - <ROOT>/meta-openembedded/meta-oe/recipes-devtools/protobuf/protobuf_3.6.1.bb
## SRCREV = "48cb18e5c419ddd23d9badcfe4e9df7bde1979b2"
## PV .= "+git${SRCPV}"
## eg: protobuf_package_ver="3.6.1+gitAUTOINC+48cb18e5c4-r0"
## Output path - <WSP>/poky/build/tmp-glibc/work/x86_64-linux/protobuf-native/

## LE target
## - <ROOT>/poky/meta-qti-bsp/recipes-devtools/protobuf/protobuf_3.3.0.bb
## SRCREV = "a6189acd18b00611c1dc7042299ad75486f08a1a"
## PV = "3.3.0+git${SRCPV}"
## eg: protobuf_package_ver="3.3.0+gitAUTOINC+a6189acd18-r0"
## Output path - <WSP>/poky/build/tmp-glibc/work/x86_64-linux/protobuf-native/

## EAP target
## - <ROOT>/sources/meta-openembedded/meta-oe/recipes-devtools/protobuf/protobuf_3.5.1.bb
## SRCREV = "106ffc04be1abf3ff3399f54ccf149815b287dd9"
## PV = "3.5.1+git${SRCPV}"
## eg: protobuf_package_ver="3.5.1+gitAUTOINC+106ffc04be-r0"
## Output path is different - <WSP>/build/tmp/work/x86_64-linux/protobuf-native/

## LE-PDK target
## - <ROOT>/poky/meta-qti-bsp/recipes-devtools/protobuf/protobuf_3.3.0.bb
## SRCREV = "a6189acd18b00611c1dc7042299ad75486f08a1a"
## PV = "3.3.0+git${SRCPV}"
## eg: protobuf_package_ver="3.3.0+gitAUTOINC+a6189acd18-r0"
## <WSP> will be "<ROOT>/src"
## Output path - eg: <WSP>/../build-qti-distro-fullstack-perf/tmp-glibc/work/x86_64-linux/protobuf-native

# protobuf native out path default - for LE/LV
protobuf_protoc_path=$wsp_path"poky/build/tmp-glibc/sysroots-components/x86_64/protobuf-native/usr/bin/protoc"

# find protoc path based on target type
if [ -e $wsp_path"sources/meta-qti-eap/recipes-location/location/gps-utils_git.bb" ]
then
    manifest_info="LE-EAP"
    protobuf_protoc_path=$wsp_path"build/tmp/sysroots-components/x86_64/protobuf-native/usr/bin/protoc"
elif [ -e "$wsp_path/../poky/qti-conf/set_bb_env.sh" ]
then
    manifest_info="LE-PDK"
    # find build folder and select first one if multiple build folders
    build_folders=(`ls $wsp_path/../|grep build-`)
    if [ ${#build_folders[@]} -eq 0 ]; then
        echo "ERROR!! No build folders found..."
        exit 1
    fi
    echo "LE PDK - Using build folder ${build_folders[0]}"
    protobuf_protoc_path=$wsp_path"../"${build_folders[0]}"/tmp-glibc/sysroots-components/x86_64/protobuf-native/usr/bin/protoc"
elif [ -e $wsp_path"meta-qti-bsp/meta-qti-base/recipes-location/location/gps-utils_git.bb" ]
then
    manifest_info="LV"
else
    manifest_info="LE"
fi

echo "Target is $manifest_info"
echo "Binary protoc path is : $protobuf_protoc_path"
if ! [ -e $protobuf_protoc_path ]
then
    echo "ERROR!! Protoc - $protobuf_protoc_path - is not found..."
    exit 1
fi

## array of .proto files. Add any new .proto files to this array.
src_proto_files=(SlimRemoteMsgs.proto)

for src_file in ${src_proto_files[@]}; do
    echo -e "\nProcessing $src_file"
    ## protoc --cpp_out=. EHMsg.proto
    echo " >> Running protoc"
    cmd=$protobuf_protoc_path' --proto_path=. --cpp_out=. '"provider/rp/proto/"$src_file
    eval $cmd
    if [ $? -eq 0 ]; then
        echo "SUCCESS !!! protoc compilation"
    else
        echo "ERROR!! protoc compilation failed"
        exit 1
    fi
done
