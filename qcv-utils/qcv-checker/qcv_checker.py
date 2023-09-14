# !/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

# Import standard python modules
import argparse
import importlib
import os
import sys
import time

# Import QCV utility modules
from utilities.utils import Arguments
from utilities.utils import Constants
from utilities.utils import HelperFunctions
from utilities.utils import Logger
from utilities.utils import Variables

# QCV checker script (qcv_checker.py) ensures that chipset specific vendor
# features are differentiated only at run-time by enabling below checks.
# All the violations would be captured in qcv_violators.txt file located in
# out/target/product/<target>/qcv_checker/ folder.
#
# 1) Common lunch option:
#
#    This is to ensure that the build configurations are the same across
#    QCV chipsets. e.g. If chipsetA and chipsetB belong to the QCV family,
#    'device/qcom/chipsetA' should be used to specify the build
#    configurations for both chipsetA and chipsetB.
#    'check_lunch_option' function in qcv_checker.py would flag an error if
#    a separate lunch option is created when a new chipset is onboarded.
#    List of chipsets which belong to the QCV family are derived from
#    DEVICE_MANIFEST_SKUS in device/qcom/chipsetA/chipsetA.mk
#
# 2) Chipset specific VINTF device manifest:
#
#    Chipset specific vendor interface (VINTF) manifest helps to advertise
#    different HAL's based on chipset capabilities although the interfaces
#    exported have to be the same for a HAL that's enabled across chipsets.
#    'check_vintf_device_manifest' function in qcv_checker.py would flag an
#    error if chipset specific VINTF manifests are missing in
#    '/vendor/etc/vintf/'
#
#    e.g. If chipsetA and chipsetB belong to the QCV family,
#    DEVICE_MANIFEST_SKUS would specify the list of chipsets which are
#    enabled using the same vendor and DEVICE_MANIFEST_{target}_FILES
#    specifies the path to VINTF manifest. These xml's get copied to
#    '/vendor/etc/vintf' during compilation
#
#    DEVICE_MANIFEST_SKUS := chipsetA chipsetB
#    DEVICE_MANIFEST_CHIPSETA_FILES := device/qcom/chipsetA/manifest_chipsetA.xml
#    DEVICE_MANIFEST_CHIPSETB_FILES := device/qcom/chipsetA/manifest_chipsetB.xml
#
# 3) Chipset specific Android permissions/capabilities:
#
#    Android permissions can differ per chipset based on hardware capability.
#    In order to allow run-time differentiation, these permission xml's are
#    stored and loaded from chipset specific sku based folder.
#    'check_android_capabilities' function in qcv_checker.py would flag an
#    error if these sku based folders are missing in '/vendor/etc/permissions/'
#
#    e.g. sensor permission/capability xml's are stored and loaded from
#    sku_chipsetA and sku_chipsetB folders for chipsetA and chipsetB
#    respectively.
#
#    chipsetA permissions : /vendor/etc/permissions/sku_chipsetA
#    chipsetB permissions : /vendor/etc/permissions/sku_chipsetB
#
# 4) Chipset specific subsystem capabilities:
#
#    Subsystem (e.g. audio) capabilities can differ per chipset based
#    on hardware capabilities. In order to allow run-time differentiation,
#    these capability xml's are stored and loaded from chipset specific sku
#    based folder. 'check_audio_capabilities' function in qcv_checker.py
#    would flag an error if these xml's are mising in
#    '/vendor/etc/audio/sku_${target}_qssi'
#
#    e.g. audio_policy_configuration.xml should be preesnt in below folders.
#        chipsetA audio : /vendor/etc/audio/sku_chipsetA_qssi
#        chipsetB audio : /vendor/etc/audio/sku_chipsetB_qssi
#
#    Similarly, media/video capability xml's are appended with chipset name
#    to run-time differentiate. 'check_media_capabilities' function in
#    qcv_checker.py would flag an error if media xml's are missing in
#    '/vendor/etc/'
#
#    e.g. chipsetA video : /vendor/etc/media_codecs_chipsetA.xml
#         chipsetB video : /vendor/etc/media_codecs_chipsetB.xml


# 'validate_args' function helps to check if target name and enforcement mode
# specified as input arguments are right. e.g. qcv_checker would execute only
# if the target is a QCV chipset. Enforcement mode can be either 1 (throw an
# error and stop compilation) or 0 (continue compilation even though violators
# are present)
def validate_args(arg_target, arg_enforce):
    print("qcv_checker: Checking input arguments --target and --enforce")

    if not HelperFunctions.validate_args(arg_target, arg_enforce):
        print("\nRefer to help in python qcv_checker.py -h\n")
        sys.exit(0)

    if arg_enforce == "1":
        print("qcv_checker is executing in enforcement mode")
        Variables.is_enforced = True
    else:
        print("qcv_checker is not executing in enforcement mode")
        Variables.is_enforced = False

# Ensure that chipsets in the QCV family use the same lunch option. Flag an
# error if QCV chipsets use separate lunch option
def check_lunch_option(arg_target):
    for chipset in Constants.qcv_chipsets:
        if chipset != arg_target and HelperFunctions.dir_exists(
            Constants.croot + Constants.device_path_prefix + chipset):
            HelperFunctions.print_violators("check_lunch_option:",
                "Cannot have separate lunch option for " + chipset)
        else:
            HelperFunctions.print_info_on_internal_logger("check_lunch_option:",
                "Common lunch option is used for " + chipset)

# Check for chipset specific VINTF device manifest in /vendor/etc/vintf/. Flag
# an error if VINTF manifest is missing for any of the QCV chipsets
def check_vintf_device_manifest():
    for chipset in Constants.qcv_chipsets:
        if not HelperFunctions.file_exists(Constants.out_path +
            Constants.vendor_etc_vintf_path_prefix +
            Constants.vintf_manifest_prefix + chipset + Constants.xml_suffix):
            HelperFunctions.print_violators("check_vintf_device_manifest:",
                "VINTF device manifest " + Constants.vintf_manifest_prefix +
                chipset + Constants.xml_suffix + " is missing for " + chipset)
        else:
            HelperFunctions.print_info_on_internal_logger(
                "check_vintf_device_manifest:", "VINTF device manifest " +
                Constants.vintf_manifest_prefix + chipset + Constants.xml_suffix
                + " is available for " + chipset)

# Check android and subsystem capabilities for chipsets in the QCV family
def check_subsytem_capabilities():
    for chipset in Constants.qcv_chipsets:
        check_android_capabilities(chipset)
        check_audio_capabilities(chipset)
        check_media_capabilities(chipset)

# Check for chipset specific sku based folder in /vendor/etc/permissions/
# Flag an error if sku based permission folder is missing for any of the
# QCV chipsets
def check_android_capabilities(chipset):
    if not HelperFunctions.dir_exists(Constants.out_path +
        Constants.vendor_etc_permissions_sku_path_prefix + chipset):
        HelperFunctions.print_violators("check_android_capabilities:",
            "/vendor/etc/permissions/sku_" + str(chipset) +
            " folder is missing for " + str(chipset))
    else:
        HelperFunctions.print_info_on_internal_logger(
            "check_android_capabilities:", "/vendor/etc/permissions/sku_" +
            str(chipset) + " folder is available for " + str(chipset))

# Check for chipset specific audio policy configuration.xml in
# /vendor/etc/audio/sku_${target}_qssi. Flag an error if APM config
# is missing for any of the QCV chipsets
def check_audio_capabilities(chipset):
    if not HelperFunctions.file_exists(Constants.out_path +
        Constants.vendor_etc_audio_sku_path_prefix + chipset +
        Constants.qssi_suffix + Constants.audio_policy_config):
        HelperFunctions.print_violators("check_audio_capabilities:",
            "/vendor/etc/audio/sku_" + str(chipset) +
            str(Constants.qssi_suffix) + str(Constants.audio_policy_config) +
            " is missing for " + str(chipset))
    else:
        HelperFunctions.print_info_on_internal_logger(
            "check_audio_capabilities:", "/vendor/etc/audio/sku_" +
            str(chipset) + str(Constants.qssi_suffix) +
            str(Constants.audio_policy_config) + " is available for " +
            str(chipset))

# Check for chipset specific media_codecs, media_codecs_performance and
# media_profiles xml's in /vendor/etc/. Flag an error if media xml's
# are missing for any of the QCV chipsets
def check_media_capabilities(chipset):
    dir_path = Constants.out_path + Constants.vendor_etc_path_prefix
    for media_xml in Constants.media_xmls:
        file_name = media_xml + chipset
        if not HelperFunctions.file_pattern_exists(dir_path, file_name):
            HelperFunctions.print_violators("check_media_capabilities:",
                str(Constants.vendor_etc_path_prefix) + str(media_xml) +
                str(chipset) + str(Constants.xml_suffix) + " is missing for " +
                str(chipset))
        else:
            HelperFunctions.print_info_on_internal_logger(
                "check_media_capabilities:",
                str(Constants.vendor_etc_path_prefix) + str(media_xml) +
                str(chipset) + str(Constants.xml_suffix) +
                " is available for " + str(chipset))

# main function
def main():
    start = time.time()

    argsObj = Arguments()
    args = argsObj.parser.parse_args()
    arg_target = args.target
    arg_enforce = args.enforce

    validate_args(arg_target, arg_enforce)

    check_lunch_option(arg_target)

    check_vintf_device_manifest()

    check_subsytem_capabilities()

    if (Variables.is_violator == True):
        Logger.logStdout.error("\tFailure: Found QCV violators")
        Logger.logStdout.error("\tPlease take a look at qcv_violators.txt file"
            " in " + Constants.qcv_checker_dir + " folder")
    else:
        Logger.logStdout.info("\tSuccess: No QCV violators!\n")

    if (Variables.is_enforced == True and Variables.is_violator == True):
        Logger.logStdout.error("\tQCV checker is executing in enforcement mode,"
            " returning failure to caller\n")
        sys.exit(-1)
    elif (Variables.is_enforced == False and Variables.is_violator == True):
        Logger.logStdout.info("\tQCV checker is not executing in enforcement"
            " mode, returning success to caller\n")

    sys.exit(0)

if __name__ == '__main__':
    main()
