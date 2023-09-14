#!/bin/bash
#******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

# To run this script:
#
#  source build/envsetup.sh
#  lunch <target>-userdebug
#  vendor/qcom/proprietary/qcril-nr/test/utf/buildCodeCoverage.sh


err_exit() {
    if [ "$1" ]; then
        echo -e 1>&2 "\nERROR: $1"
    fi
    exit 1
}

cd $ANDROID_BUILD_TOP/  # croot

source build/envsetup.sh
lunch ${TARGET_PRODUCT}-${TARGET_BUILD_VARIANT}

#Remove existing coverage files from out
#cd $ANDROID_BUILD_TOP/out/   # croot
#find . -type f -name "*.gcno" -exec rm -rf {} \;
#find . -type f -name "*.gcda" -exec rm -rf {} \;
#cd $ANDROID_BUILD_TOP/  # croot

# Remove the previous RIL UTF intermediate files
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/EXECUTABLES/qcrilnr_utf_test.bin_intermediates/
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/STATIC_LIBRARIES/libqcrilNr_intermediates/
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/STATIC_LIBRARIES/libqcril-nr-utf-fwk_intermediates/
# Remove the previous intermediate files
rm -rf $ANDROID_BUILD_TOP/out/qcril_coverage*.info
# Remove the previous coverage report directory
rm -rf $ANDROID_BUILD_TOP/out/qcril_coverage/


mmma -j16 UTF_TEST_COVERAGE=true vendor/qcom/proprietary/qcril-nr/test/utf/
#make -j16 UTF_TEST_COVERAGE=true qcrilnr_utf_test

if [ $? -ne 0 ]; then
    err_exit "    FAILED: make"
fi

# Create log directory
#mkdir $ANDROID_BUILD_TOP/log/

# Run test cases
qcrilnr_utf_test -a ALL

if [ $? -ne 0 ]; then
    err_exit "    FAILED: qcrilnr_utf_test"
fi

cd $ANDROID_BUILD_TOP/out/

# Generate the coverage info
lcov --directory . \
  --capture \
  --gcov-tool $ANDROID_BUILD_TOP/vendor/qcom/proprietary/qcril-nr/test/utf/llvm_gcov_wrapper.sh \
  --test-name "QCRIL_UTF_FullCoverage" \
  --output-file qcril_coverage_i1.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [1]"
fi

# Include only qcril-nr related files in the coverage
lcov --extract qcril_coverage_i1.info "*qcril-nr*" \
     --output-file qcril_coverage_i2.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [2]"
fi

# Remove files which we do not want to include in the report (UIM, sample etc)
lcov --remove qcril_coverage_i2.info  "*test*" \
     --remove qcril_coverage_i2.info  "*modules/sample*" \
     --remove qcril_coverage_i2.info  "*modules/uim*" \
     --remove qcril_coverage_i2.info  "*modules/gstk*" \
     --remove qcril_coverage_i2.info  "*modules/lpa*" \
     --remove qcril_coverage_i2.info  "*modules/ccid*" \
     --remove qcril_coverage_i2.info  "*interfaces/gstk*" \
     --remove qcril_coverage_i2.info  "*interfaces/uim*" \
     --output-file qcril_coverage.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [3]"
fi

# Generate html report
genhtml --demangle-cpp \
        --title "QCRIL Code Coverage" \
        --num-spaces 4 \
        --precision 2 \
        --output-directory qcril_coverage \
        qcril_coverage.info 2>&1 | tee qcril_coverage_genhtml.log

if [ $? -ne 0 ]; then
    err_exit "    FAILED: genhtml"
fi
