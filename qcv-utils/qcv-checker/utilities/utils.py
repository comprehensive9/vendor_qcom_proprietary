# !/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

import argparse
import errno
import os
import logging
import shutil
import sys
import textwrap

class HelperFunctions:
    def __init__():
        pass

    @staticmethod
    def dir_exists(dir_name=None):
        if dir_name == None:
            return False
        if os.path.isdir(dir_name):
            return True
        else:
            return False

    @staticmethod
    def file_exists(file_name=None):
        if file_name == None:
            return False
        if os.path.isfile(file_name):
            return True
        else:
            return False

    @staticmethod
    def file_pattern_exists(dir_path=None, file_name=None):
        if dir_path == None:
            return False
        if file_name == None:
            return False
        for file in os.listdir(dir_path):
            if file_name in file:
                return True
        return False

    @staticmethod
    def path_exists(path=None):
        if path == None:
            return False
        if os.path.exists(path):
            return True
        else:
            return False

    @staticmethod
    def remove_if_file_exists(file_name=None):
        if file_name == None:
            return
        if os.path.isfile(file_name):
            os.remove(file_name)
            return

    @staticmethod
    def create_dir(path=None):
        if HelperFunctions.dir_exists(path) or path == None:
            return
        try:
            os.mkdir(path)
        except OSError as error:
            if error.errno != errno.EEXIST:
                raise

    @staticmethod
    def rm_dir(path):
        try:
            shutil.rmtree(path)
        except:
            HelperFunctions.print_info_on_internal_logger("rm_dir",
                "Failed deleting dir: " + path)

    @staticmethod
    def valid_value(default_value, base_value, actual_value):
        if base_value == None or base_value == "":
            return default_value
        else:
            return base_value + actual_value

    @staticmethod
    def get_qcv_chipsets(file_name=None):
        vendor_skus = None
        if file_name == None:
            return vendor_skus
        if os.path.isfile(file_name):
            with open(file_name) as f:
                for line in f.readlines():
                    if "vintf_vendor_manifest_skus" in line:
                        vintf_vendor_manifest_skus, vendor_skus = line.split(
                            "=", 1)
                        vendor_skus = list(vendor_skus.replace(
                            ' ', ',').rstrip('\n').split(","))
            return vendor_skus

    @staticmethod
    def validate_args(arg_target, arg_enforce):
        if arg_target == None or arg_enforce == None:
            print("validate_args:", "Target name and enforcement mode need to"
                " be specified to execute qcv_checker.py")
            return False
        elif arg_target != None:
            if Constants.qcv_chipsets != None:
                if arg_target not in Constants.qcv_chipsets:
                    print("validate_args:",
                        "Exiting qcv_checker. Target " + str(arg_target) +
                        " is not a QCV chipset")
                    return False
                elif arg_enforce != None:
                    if arg_enforce not in Constants.valid_enforcement_args:
                        print("validate_args:",
                            "Enforcement mode " + str(arg_enforce) +
                            " is not valid. Must be 1 or 0")
                        return False
            else:
                print("validate_args:",
                    "Exiting qcv_checker. Unable to retrieve the list of"
                    " qcv chipsets")
                return False
        return True

    @staticmethod
    def print_info_on_internal_logger(function_name=None, reason=None):
        Logger.logInternal.info(function_name + " " + reason)

    @staticmethod
    def print_info_on_stdout(function_name=None, reason=None):
        Logger.logStdout.info(function_name + " " + reason)

    @staticmethod
    def print_violations_on_stdout(function_name=None, reason=None):
        Variables.is_violator = True
        Logger.logStdout.error("\t============================================"
            "======================================================")
        Logger.logStdout.error("\tFunction Name        : " + function_name)
        Logger.logStdout.error("\tReason               : " + reason)
        Logger.logStdout.error("\tGuidelines to fix    : Refer to"
            " documentation in qcv_checker.py and go/qcv")
        Logger.logStdout.error("\t============================================"
            "======================================================\n")

    @staticmethod
    def print_error_on_stdout(function_name=None, reason=None):
        Logger.logStdout.error("\t============================================"
            "======================================================")
        Logger.logStdout.error("\tFunction Name        : " + function_name)
        Logger.logStdout.error("\tReason               : " + reason)
        Logger.logStdout.error("\tGuidelines to fix    : Refer to"
            " documentation in qcv_checker.py and go/qcv")
        Logger.logStdout.error("\t============================================"
            "======================================================\n")

    @staticmethod
    def print_error(function_name=None, reason=None):
        Logger.logViolators.error("\t========================================="
            "======================================================")
        Logger.logViolators.error("\tFunction Name        : " + function_name)
        Logger.logViolators.error("\tReason               : " + reason)
        Logger.logViolators.error("\tGuidelines to fix    : Refer to"
            " documentation in qcv_checker.py and go/qcv")
        Logger.logViolators.error("\t========================================="
            "======================================================\n")

    @staticmethod
    def print_warning(function_name=None, reason=None):
        Logger.logViolators.warning("\t======================================="
            "======================================================")
        Logger.logViolators.warning("\tFunction Name        : " + function_name)
        Logger.logViolators.warning("\tReason               : " + reason)
        Logger.logViolators.warning("\tGuidelines to fix    : Refer to"
            " documentation in qcv_checker.py and go/qcv")
        Logger.logViolators.warning("\t======================================="
            "======================================================\n")

    @staticmethod
    def print_violators(function_name=None, reason=None):
        Variables.is_violator = True
        HelperFunctions.print_error_on_stdout(function_name, reason)
        if (Variables.is_enforced == True):
            HelperFunctions.print_error(function_name, reason)
        else:
            HelperFunctions.print_warning(function_name, reason)

class Arguments:
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=textwrap.dedent(
                '''
                QCV Checker:
                python qcv_checker.py --target <target_name> --enforce <enforcement_mode> --product_out <product_out>
                '''))

        self.parser.add_argument("--target",
          help="Target name",
          required=True, type=str)

        self.parser.add_argument("--enforce",
          help="Enforcement mode e.g 1(enforce) or 0(do not enforce)",
          required=True, type=str)

        self.parser.add_argument("--product_out",
          help="PRODUCT_OUT e.g. out/target/product/{target}",
          required=True, type=str)

class Constants:
    def __init__():
        pass

    argsObj = Arguments()
    args = argsObj.parser.parse_args()
    product_out = args.product_out

    #Macros
    valid_enforcement_args = ['1', '0']
    pwd = os.environ['PWD']
    if "vendor/qcom/proprietary/qcv-utils/qcv-checker" in pwd:
        out_path = pwd + "/../../../../../" + product_out
        croot = pwd + "/../../../../../"
    else:
        out_path = pwd + "/" + product_out
        croot = pwd + "/"

    qcv_chipsets = HelperFunctions.get_qcv_chipsets(
        HelperFunctions.valid_value("",out_path,"/misc_info.txt"))

    if qcv_chipsets == None:
        print("validate_args:", "Exiting qcv_checker. Unable to retrieve the"
            " list of qcv chipsets. Check input argument 'product_out'")
        print("\nRefer to help in python qcv_checker.py -h\n")
        sys.exit(0)

    qcv_checker_dir = HelperFunctions.valid_value("",out_path,"/qcv_checker")
    logfileInternal = HelperFunctions.valid_value(
        "./qcv_internal.txt",qcv_checker_dir,"/qcv_internal.txt")
    logfileViolators = HelperFunctions.valid_value(
        "./qcv_violators.txt",qcv_checker_dir,"/qcv_violators.txt")
    device_path_prefix = "device/qcom/"
    vintf_manifest_prefix = "manifest_"
    xml_suffix = ".xml"
    qssi_suffix = "_qssi/"
    vendor_etc_vintf_path_prefix = "/vendor/etc/vintf/"
    vendor_etc_permissions_sku_path_prefix = "/vendor/etc/permissions/sku_"
    vendor_etc_audio_sku_path_prefix = "/vendor/etc/audio/sku_"
    vendor_etc_path_prefix = "/vendor/etc/"
    audio_policy_config = "audio_policy_configuration.xml"
    media_codecs_prefix = "media_codecs_"
    media_codecs_performance_prefix = "media_codecs_performance_"
    media_profiles_prefix = "media_profiles_"
    media_xmls = [media_codecs_prefix, media_codecs_performance_prefix,
        media_profiles_prefix]

class Variables:
    def __init__():
        pass

    is_enforced = False;
    is_violator = False;

class Logger:
    def __init__(self):
        pass

    def loggerSetup(name, log_file, level):
        if not (
            HelperFunctions.dir_exists(Constants.qcv_checker_dir)
            and Constants.qcv_checker_dir != ""
        ):
            HelperFunctions.create_dir(Constants.qcv_checker_dir)

        HelperFunctions.remove_if_file_exists(log_file)

        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        logger = logging.getLogger(name)
        logger.setLevel(level)
        if(log_file == None):
            ch = logging.StreamHandler(sys.stdout)
            ch.setFormatter(formatter)
            logger.addHandler(ch)
        else:
            fh = logging.FileHandler(log_file)
            fh.setFormatter(formatter)
            logger.addHandler(fh)
        return logger

    logInternal  = loggerSetup('Internal', Constants.logfileInternal,
        logging.DEBUG)
    logViolators = loggerSetup('Violators' , Constants.logfileViolators,
        logging.WARNING)
    logStdout    = loggerSetup('logStdout' , None, logging.DEBUG)
