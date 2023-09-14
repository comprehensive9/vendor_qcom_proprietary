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
#  vendor/qcom/proprietary/qcril-hal/test/utf/buildCodeCoverage.sh


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
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/EXECUTABLES/qcril_hal_utf_test_intermediates/
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/STATIC_LIBRARIES/libqcril-hal_intermediates/
rm -rf $ANDROID_BUILD_TOP/out/host/linux-x86/obj/STATIC_LIBRARIES/libqcril-utf-fwk_intermediates/
# Remove the previous intermediate files
rm -rf $ANDROID_BUILD_TOP/out/qcril-hal_coverage*.info
# Remove the previous coverage report directory
rm -rf $ANDROID_BUILD_TOP/out/qcril-hal_coverage/
# Remove the previous log directory
rm -rf $ANDROID_BUILD_TOP/log/

# Build RIL UTF
mmma -j16 UTF_TEST_COVERAGE=true vendor/qcom/proprietary/qcril-hal/test/utf/
#make -j16 UTF_TEST_COVERAGE=true qcril_hal_utf_test

if [ $? -ne 0 ]; then
    err_exit "    FAILED: make"
fi

# Create log directory
mkdir $ANDROID_BUILD_TOP/log/

# Run test cases
qcril_hal_utf_test

if [ $? -ne 0 ]; then
    err_exit "    FAILED: qcril_hal_utf_test"
fi

cd $ANDROID_BUILD_TOP/out/

# Generate the coverage info
lcov --directory . \
  --capture \
  --gcov-tool $ANDROID_BUILD_TOP/vendor/qcom/proprietary/qcril-hal/test/utf/llvm_gcov_wrapper.sh \
  --test-name "QCRIL_UTF_FullCoverage" \
  --output-file qcril-hal_coverage_i1.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [1]"
fi

# Include only qcril-hal related files in the coverage
lcov --extract qcril-hal_coverage_i1.info "*qcril-hal*" \
     --output-file qcril-hal_coverage_i2.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [2]"
fi

# Remove files which we do not want to include in the report (UIM, sample etc)
lcov --remove qcril-hal_coverage_i2.info  "*test*" \
     --remove qcril-hal_coverage_i2.info  "*modules/sample*" \
     --remove qcril-hal_coverage_i2.info  "*modules/uim*" \
     --remove qcril-hal_coverage_i2.info  "*modules/gstk*" \
     --remove qcril-hal_coverage_i2.info  "*modules/lpa*" \
     --remove qcril-hal_coverage_i2.info  "*modules/ccid*" \
     --remove qcril-hal_coverage_i2.info  "*interfaces/gstk*" \
     --remove qcril-hal_coverage_i2.info  "*interfaces/uim*" \
     --output-file qcril-hal_coverage.info

if [ $? -ne 0 ]; then
    err_exit "    FAILED: lcov [3]"
fi

# Generate html report
genhtml --demangle-cpp \
        --output-directory qcril-hal_coverage \
        --title "QCRIL Code Coverage" \
        --num-spaces 4 \
        --precision 2 \
        qcril-hal_coverage.info 2>&1 | tee qcril-hal_coverage_genhtml.log

if [ $? -ne 0 ]; then
    err_exit "    FAILED: genhtml"
fi
