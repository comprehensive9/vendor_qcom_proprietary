#!/usr/bin/python
# -*- coding: utf-8 -*-
#Copyright (c) 2020 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

import os,logging,shutil,tarfile,sys,argparse,errno,textwrap
from zipfile import ZipFile

class UtilFunctions:
    def __init__():
        pass

    @staticmethod
    def dirExists(dirName=None):
        if dirName == None:
            return False
        if os.path.isdir(dirName):
            return True
        else:
            return False

    @staticmethod
    def fileExists(fileName=None):
        if fileName == None:
            return False
        if os.path.isfile(fileName):
            return True
        else:
            return False

    @staticmethod
    def pathExists(path=None):
        if path == None:
            return False
        if os.path.exists(path):
            return True
        else:
            return False


    @staticmethod
    def removeIfFileExists(fileName=None):
        if fileName == None:
            return
        if os.path.isfile(fileName):
            os.remove(fileName)
            return

    @staticmethod
    def print_info_on_stdout(LOG_TAG=None, info=None):
        Logger.logStdout.info(LOG_TAG + " " + info)

    @staticmethod
    def print_violations_on_stdout(LOG_TAG=None, sub_module=None, function_name=None, reason=None):
        Variables.is_violator = True
        Logger.logStdout.error("\t==================================================================================================")
        Logger.logStdout.error("\tFileName      : " + LOG_TAG)
        Logger.logStdout.error("\tSubModuleName : " + sub_module)
        Logger.logStdout.error("\tFunctionName   : " + function_name)
        Logger.logStdout.error("\tReason        : " + reason)
        Logger.logStdout.error("\tHow to fix    : on go/qiifa, look at the How to fix QIIFA violators")
        Logger.logStdout.error("\t==================================================================================================\n")

    @staticmethod
    def print_warning_on_stdout(reason=None):
        Logger.logStdout.warning(reason)

    @staticmethod
    def print_error_on_stdout(interface_name=None, reason=None, cmd_version=None, found_version=None):
        Logger.logStdout.error("\t==================================================================================================")
        Logger.logStdout.error("\tInterface Name: " + interface_name)
        Logger.logStdout.error("\tDescription   : " + reason)
        Logger.logStdout.error("\tReason        :")
        Logger.logStdout.error("\t\t\tqiifa_cmd version: " + cmd_version)
        Logger.logStdout.error("\t\t\tFound version    : " + found_version)
        Logger.logStdout.error("\tHow to fix    : on go/qiifa, look at the How to fix QIIFA violators")
        Logger.logStdout.error("\t==================================================================================================\n")

    @staticmethod
    def print_error(interface_name=None, reason=None, cmd_version=None, found_version=None):
        Logger.logViolators.error("\t==================================================================================================")
        Logger.logViolators.error("\tInterface Name: " + interface_name)
        Logger.logViolators.error("\tDescription   : " + reason)
        Logger.logViolators.error("\tReason        :")
        Logger.logViolators.error("\t\t\tqiifa_cmd version: " + cmd_version)
        Logger.logViolators.error("\t\t\tFound version    : " + found_version)
        Logger.logViolators.error("\tHow to fix    : on go/qiifa, look at the How to fix QIIFA violators")
        Logger.logViolators.error("\t==================================================================================================\n")

    @staticmethod
    def print_warning(interface_name=None, reason=None, cmd_version=None, found_version=None):
        Logger.logViolators.warning("\t==================================================================================================")
        Logger.logViolators.warning("\tInterface Name: " + interface_name)
        Logger.logViolators.warning("\tDescription   : " + reason)
        Logger.logViolators.warning("\tReason        :")
        Logger.logViolators.warning("\t\t\tqiifa_cmd version: " + cmd_version)
        Logger.logViolators.warning("\t\t\tFound version    : " + found_version)
        Logger.logViolators.warning("\tHow to fix    : on go/qiifa, look at the How to fix QIIFA violators")
        Logger.logViolators.warning("\t==================================================================================================\n")

    @staticmethod
    def print_violators(interface_name=None, reason=None, cmd_version=None, found_version=None):
        Variables.is_violator = True
        UtilFunctions.print_error_on_stdout(interface_name, reason, cmd_version, found_version)
        if (Variables.is_enforced == True):
            UtilFunctions.print_error(interface_name, reason, cmd_version, found_version)
        else:
            UtilFunctions.print_warning(interface_name, reason, cmd_version, found_version)

    @staticmethod
    def create_dir(path=None):
        if UtilFunctions.dirExists(path) or path == None:
            return
        try:
            os.mkdir(path)
        except OSError as error:
            if error.errno != errno.EEXIST:
                raise

    @staticmethod
    def rmdir(path):
        try:
            shutil.rmtree(path)
        except:
            Logger.logInternal.info("error: rmdir %s", path)
    @staticmethod
    def zip_dir(dir_name, zip_file_name):
        with ZipFile(zip_file_name, 'w') as zip_fd:
            for d_name, sub_d_name, file_names in os.walk(dir_name):
                if not d_name.endswith('.git'):
                    for f_name in file_names:
                       if not f_name.endswith(('.pyc','.bp')):
                           file_path = os.path.join(d_name, f_name)
                           zip_fd.write(file_path, os.path.relpath(os.path.join(d_name, f_name), os.path.join(dir_name, '..')))

    @staticmethod
    def makeTarFile(output_filename, source_dir):
        UtilFunctions.removeIfFileExists(output_filename)
        if(UtilFunctions.dirExists(source_dir)):
            with tarfile.open(output_filename, "w:gz") as tar:
                tar.add(source_dir, arcname=os.path.basename(source_dir))
        else:
            Logger.logInternal.info("%s dir does not exist")

    @staticmethod
    def searchAndUntarGzFile(path, file_name):
        target = True if "target" in file_name else False
        for root, dr, files in os.walk(path):
            for file in files:
                if not target:
                    if file == file_name:
                        tar = tarfile.open(os.path.join(root,file), 'r:gz')
                        tar.extractall(path)
                        tar.close()
                        return True, file
                else:
                    if file.startswith('qiifa_') and file.endswith('_current_cmd.gz') and "qssi" not in file:
                        tar = tarfile.open(os.path.join(root,file), 'r:gz')
                        tar.extractall(path)
                        tar.close()
                        return True, file
        return False, None

    @staticmethod
    def validValue(default_value, base_value, actual_value):
        return default_value if base_value == None or base_value == "" else base_value + actual_value

    @staticmethod
    def isValidArgs(arg, pl_list):
        if(arg == "all"):
            return True
        for plugin in pl_list:
            if (plugin != None and arg != None):
                arg_list = plugin.MyPlugin().register()
                for arg_list_ele in arg_list:
                    if(arg == arg_list_ele):
                        return True
        return False

    @staticmethod
    def validateArgs(arg_type, arg_create, arg_qssi, arg_target, pl_list):
        if arg_type == None and arg_create == None and arg_qssi == None and arg_target == None:
            return False
        elif arg_type != None:
            if arg_create != None or arg_qssi != None or arg_target != None:
                print("error: --type argument should not be mixed with any other arguments!")
                return False
            elif Constants.croot == None or Constants.qc_path == None or Constants.target_product == None:
                print("error: --type argument is only used when tool is run from Android build")
                print("       if Android build exists setup source and lunch and try again.")
                return False
            elif not UtilFunctions.isValidArgs(arg_type,pl_list):
                print("error: There is no such valid plugin(Invalid --type argument)")
                return False
        elif arg_create != None:
            if arg_type != None or arg_target != None or arg_qssi != None:
                print("error: --create argument should not be mixed with any other arguments!")
                return False
            elif Constants.croot == None or Constants.qc_path == None or Constants.target_product == None:
                print("error: --create argument is only used when tool is run from Android build")
                print("       if Android build exists setup source and lunch and try again.")
                return False
            elif not UtilFunctions.isValidArgs(arg_create[0],pl_list):
                print("error: There is no such valid plugin(Invalid --create argument)")
                return False
        elif arg_qssi == None and arg_target != None:
            print("error: --target and --qssi arguments must both be given!")
            return False
        elif arg_target == None and arg_qssi != None:
            print("error: --target and --qssi arguments must both be given!")
            return False
        return True

    @staticmethod
    def is_customer_variant(qc_path=None):
        if qc_path == None:
            return False
        root,dirs,files = next(os.walk(qc_path))
        for dir in dirs:
            if dir.startswith("prebuilt_"):
                return True
        return False

class Variables:
    def __init__():
        pass

    is_enforced = False;
    is_violator = False;

class Constants:
    def __init__():
        pass

    #Macros
    croot = os.getenv("ANDROID_BUILD_TOP")
    qc_path = os.getenv("QCPATH")
    target_product = os.getenv("TARGET_PRODUCT")
    out_path = os.getenv("OUT")
    qiifa_code_path = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-fwk/")
    input_db_path = qiifa_code_path + "qiifa_cmd"
    qiifa_cmd_prefix = "qiifa_hidl_cmd"
    json_file_suffix = ".json"

    if target_product == "qssi":
      qiifa_out_dir = UtilFunctions.validValue("",out_path,"/QIIFA_QSSI")
      qiifa_current_cmd_dir = out_path +"/qiifa_qssi_current_cmd"
      qiifa_cmd_tarfile = qiifa_out_dir + "/qiifa_qssi_current_cmd.gz"
    else:
      qiifa_out_dir = UtilFunctions.validValue("",out_path,"/QIIFA_TARGET")
      qiifa_current_cmd_dir = UtilFunctions.validValue("",out_path,"/qiifa_"+UtilFunctions.validValue("target",target_product,"")+"_current_cmd")
      qiifa_cmd_tarfile = UtilFunctions.validValue("",qiifa_out_dir,"/qiifa_"+UtilFunctions.validValue("target",target_product,"")+"_current_cmd.gz")

    qssi_gz_file_name = "qiifa_qssi_current_cmd.gz"
    target_gz_file_name = "qiifa_target_current_cmd.gz"

    sp_hal_list_path = UtilFunctions.validValue("",qiifa_code_path,"/qiifa_abi_checker/sp_hal_generated_file")

    logfileInternal = UtilFunctions.validValue("./qiifa_internal.txt",qiifa_out_dir,"/qiifa_internal.txt")
    logfileViolators = UtilFunctions.validValue("./qiifa_violators.txt",qiifa_out_dir,"/qiifa_violators.txt")

    #ABI related Macros
    qiifa_golden_directory_path_ship = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-cmd")
    qiifa_golden_directory_path_noship = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-cmd-noship")
    qiifa_golden_directory_path_abi_dumps_ship = os.path.join(qiifa_golden_directory_path_ship,"abi-dumps")
    qiifa_abi_checker_code_path = os.path.join(qiifa_code_path,"plugins/qiifa_abi_checker")
    module_info_file_path = UtilFunctions.validValue("",out_path,"/module-info.json")
    sdump_system_include_headers = ["bionic/libc/include",
                                    "bionic/libc/kernel/uapi",
                                    "bionic/libc/kernel/android/scsi",
                                    "bionic/libc/kernel/android/uapi",
                                    "prebuilts/clang-tools/linux-x86/clang-headers"]

    sdump_system_asm_32_includes = "bionic/libc/kernel/uapi/asm-arm"
    sdump_system_asm_64_includes = "bionic/libc/kernel/uapi/asm-arm64"
    std_sdump_cpp = " -std=gnu++17 "
    std_sdump_c = " -std=gnu99 "
    sdump_system_cflags = "-Werror=implicit-function-declaration \
                           -DANDROID \
                           -Wno-unused \
                           -Wno-reserved-id-macro \
                           -Wno-format-pedantic \
                           -Wno-unused-command-line-argument \
                           -Wno-zero-as-null-pointer-constant \
                           -Wno-sign-compare \
                           -Wno-defaulted-function-deleted \
                           -Wno-inconsistent-missing-override \
                           -Wno-unused-variable \
                           -Wno-error=deprecated-register \
                           -Wno-error=unknown-attributes \
                           -fno-exceptions"


    sdump_64_bit_target = "-march=armv8-a -target aarch64-linux-android10000"
    sdump_64_bit_gcc = UtilFunctions.validValue("",croot,"/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/aarch64-linux-android/bin")
    sdump_32_bit_target = "-march=armv7-a -mfpu=neon -target armv7a-linux-androideabi10000"
    sdump_32_bit_gcc = UtilFunctions.validValue("",croot,"/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/arm-linux-androideabi/bin")

    qiifa_golden_directory_path = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-cmd")
    qiifa_golden_abi_dump_folder = os.path.join(qiifa_golden_directory_path,"abi-dumps")
    header_abi_dumper = UtilFunctions.validValue("",croot,"/prebuilts/clang-tools/linux-x86/bin/header-abi-dumper")
    header_abi_linker = UtilFunctions.validValue("",croot,"/prebuilts/clang-tools/linux-x86/bin/header-abi-linker")
    header_abi_diff   = UtilFunctions.validValue("",croot,"/prebuilts/clang-tools/linux-x86/bin/header-abi-diff")
    selinux_file_path = UtilFunctions.validValue("",out_path,"/vendor/etc/selinux/vendor_file_contexts")
    selinux_context_sp_chars = ['?', '(', ')','\\','|']
    sp_hal_gen_file = "sp_hal_generated_file"
    arch64 = "arm64"
    arch32 = "arm32"
    arm64_armv8_a = "arm64_armv8-a"
    arm_armv7_a_neon = "arm_armv7-a-neon"
    sp_hal_string = "sp-hal"
    abidumps_string = "abi-dumps"
    abidiff_string = "abidiff"
    golden_db = "Golden db"
    current_db = "Current db"
    current_abidiff_path = os.path.join(qiifa_current_cmd_dir,abidiff_string)
    commonsys_intf_string = "commonsys-intf"
    techpackage_string = "techpackages"
    commonsys_intf_lib_file_path = os.path.join(qiifa_current_cmd_dir,"commonsys_intf_libraries")
    qiifa_current_abi_dump_folder = os.path.join(qiifa_current_cmd_dir,abidumps_string)
    sp_hal_golden_abi_dump_path = os.path.join(qiifa_golden_abi_dump_folder,sp_hal_string)
    sp_hal_current_abi_dump_path = os.path.join(qiifa_current_abi_dump_folder,sp_hal_string)
    sp_hal_gen_golden_path = UtilFunctions.validValue("",sp_hal_golden_abi_dump_path,"/sp_hal_generated_file")
    sp_hal_gen_current_path = UtilFunctions.validValue("",sp_hal_current_abi_dump_path,"/sp_hal_generated_file")
    commonsys_intf_golden_abi_dump_path = os.path.join(qiifa_golden_abi_dump_folder,commonsys_intf_string)
    commonsys_intf_current_abi_dump_path = os.path.join(qiifa_current_abi_dump_folder,commonsys_intf_string)
    sp_hal_metadata_object = {"name"         :sp_hal_string,\
                              "lib_file_path":sp_hal_gen_golden_path,\
                              "golden_path"  :sp_hal_golden_abi_dump_path, \
                              "current_path" :sp_hal_current_abi_dump_path }
    commonsys_intf_metadta_object = {"name":commonsys_intf_string, \
                                     "lib_file_path":commonsys_intf_lib_file_path, \
                                     "golden_path":commonsys_intf_golden_abi_dump_path, \
                                     "current_path":commonsys_intf_current_abi_dump_path}
    abidump_modules = {sp_hal_string : sp_hal_metadata_object, commonsys_intf_string: commonsys_intf_metadta_object}
    abidump_metadata = {"name":abidumps_string,
                        "golden_path":qiifa_golden_abi_dump_folder,
                        "current_path" : qiifa_current_abi_dump_folder,
                        "modules":abidump_modules}
    HOST_PATH = "out/host"
    recovery_path = "/recovery/root/"
    commonsys_intf_path = UtilFunctions.validValue("",qc_path,"/commonsys-intf")
    shared_libs = "SHARED_LIBRARIES"
    target_product_out_path = "out/target/product/"+UtilFunctions.validValue("",target_product,"")
    abi_config_file = os.path.join(qiifa_abi_checker_code_path,"abi_config.json")
    ##VNDK Macros
    vndk_version = "30"
    binder_bitness = "64"
    vndk_core = "VNDK-core"
    vndk_sp = "VNDK-SP"
    vndk_abi_string = "vndk-abidumps"
    vndk_abidiff_string = "vndk-abidiff"
    vndk_abi_golden_dumps = os.path.join(qiifa_golden_directory_path_noship,abidumps_string,vndk_abi_string)
    vndk_abi_current_dumps = os.path.join(qiifa_current_abi_dump_folder,vndk_abi_string)
    vndk_abi_diff_path = os.path.join(current_abidiff_path,vndk_abidiff_string)
    vndk_abi_compatiblity_report = os.path.join(qiifa_current_cmd_dir,"vndk_abi_compatiblity_report.txt")
    lsdump_paths_file_path = UtilFunctions.validValue("./lsdump_paths.txt", out_path, "/lsdump_paths.txt")
    python3_path = UtilFunctions.validValue("",croot,"/out/.path/python3")
    #HIDL related Macros
    qiifa_out_temp_cmd = UtilFunctions.validValue("",qiifa_current_cmd_dir,"/qiifa_hidl_cmd.json")
    qiifa_out_temp_cmd_2_0 = qiifa_current_cmd_dir + "/" + qiifa_cmd_prefix + "_2.0" + json_file_suffix
    qiifa_hidl_db_root = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-cmd/hidl/")
    qiifa_hidl_db_path = qiifa_hidl_db_root + "qiifa_hidl_cmd.json"
    paths_of_xml_files = [UtilFunctions.validValue("",croot,"/out/target/product/qssi/system/etc/vintf"),
                          UtilFunctions.validValue("",croot,"/out/target/product/"+UtilFunctions.validValue("",target_product,"")+"/vendor/etc/vintf")]
    hidl_skipped_intf = UtilFunctions.validValue("",qc_path,"/commonsys-intf/QIIFA-cmd/hidl/qiifa_hidl_skipped_interfaces.txt")
    qiifa_current_txt_file_paths =  [] if qc_path == None or croot == None else \
                                [qc_path + "/commonsys-intf",
                                croot + "/hardware/interfaces",
                                qc_path + "/interfaces",
                                qc_path + "/interfaces-noship",
                                croot + "/vendor/qcom/opensource/interfaces",
                                croot + "/vendor/nxp/opensource/interfaces",
                                croot + "/system/libhidl/transport",
                                croot + "/system/hardware/interfaces",
                                croot + "/frameworks/hardware/interfaces",
                                croot + "/frameworks/hardware/interfaces",
                                croot + "/hardware/google/interfaces",
                                croot + "/hardware/nxp",
                                croot + "/external/ant-wireless/hidl/interfaces",
                                croot + "/vendor/qcom/opensource/commonsys-intf/audio/"]

    legacy_xml_files = ["compatibility_matrix.1.xml",
                        "compatibility_matrix.2.xml",
                        "compatibility_matrix.3.xml",
                        "compatibility_matrix.4.xml",
                        "compatibility_matrix.legacy.xml"]
    skipped_intf =  [
                    "android.hardware.neuralnetworks",
                    "android.hardware.drm",
                    "android.hardware.radio.config",
                    "android.hardware.sensors",
                    "android.hardware.gnss",
                    "android.hardware.thermal",
                    "android.hardware.configstore",
                    "android.hardware.radio",
                    "vendor.qti.hardware.vpp"
                    ]
    unhashed_intfs = ['android.hardware.automotive.vehicle',
                      'vendor.qti.automotive.qcarcam',
                      'vendor.qti.diaghal',
                      'vendor.qti.hardware.improvetouch.blobmanager',
                      'vendor.qti.hardware.improvetouch.gesturemanager',
                      'vendor.qti.hardware.improvetouch.touchcompanion',
                      'vendor.qti.hardware.systemhelper',
                      'android.hardware.automotive.occupant_awareness',
                      'android.hardware.automotive.vehicle',
                      'android.hardware.identity',
                      'android.hardware.rebootescrow',
                      'android.hardware.automotive.vehicle'
                     ]
    QIIFA_1_0_WHITE_LIST = {'android.hardware.nfc':'v',
                            'vendor.qti.hardware.vpp':'all',
                            'vendor.qti.hardware.bluetooth_sar':['v','fq'],
                            'vendor.qti.memory.pasrmanager':'all'
                           }

    ERR_HIDL_NOT_SAME_NEED_VERSIONS = "The list of hidl versions in need section should be\n\t\t\t\t\t\tsame as in compatibility matrix entry (CMD) and current CMD!"
    ERR_HIDL_VERSIONS_MISMATCH = "There is version mismatch between compatibility matrix database(CMD)\n\t\t\t\t\t\tand current CMD!"
    ERR_HIDL_NOT_PRESENT_IN_GOLDEN = "A compatibility matrix entry must be present for"
    ERR_HIDL_MODIFIED_EXISTING_HASH = "Existing HASH is modified"
    HIDL_VERSION_NO_ERR = "pass"

    HIDL_HASH_NO_ERR = HIDL_VERSION_NO_ERR
    HIDL_HASH_MISMATCH_ERR = "A hash mismatch detected for the above hidl interface"
    HIDL_HASH_REMOVED_ERR = "A hash entry was removed from current.txt for the above hidl interface"
    HIDL_HASH_ADDED_ERR = "A new hash entry is added for the above interface, but not in qiifa cmd"

    HIDL_INTF_NO_ERR = HIDL_HASH_NO_ERR
    HIDL_INTF_MISMATCH_ERR = "An interface entry mismatch detected in the above interface"

    HIDL_SKIP_INTERFACE = ['android.hardware']

    '''Supported list of arguments per plugin (for generating golden cmd in particular)
    '''
    HIDL_SUPPORTED_CREATE_ARGS = ["hidl","hidl_one","hidl_new"]
    ABI_SUPPORTED_CREATE_ARGS = ["abi"]

    SUPPORTED_FCM_LEVELS = ['3','4','5','device']

    IS_CUSTOMER_VARIANT = UtilFunctions.is_customer_variant(qc_path)

class Logger:
    def __init__(self):
        pass

    def loggerSetup(name, log_file, level):
        if not UtilFunctions.dirExists(Constants.qiifa_out_dir) and Constants.qiifa_out_dir != "":
            UtilFunctions.create_dir(Constants.qiifa_out_dir)
        UtilFunctions.removeIfFileExists(log_file)
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
    logInternal  = loggerSetup('Internal', Constants.logfileInternal, logging.DEBUG)
    logViolators = loggerSetup('Violaters' , Constants.logfileViolators, logging.WARNING)
    logStdout    = loggerSetup('logStdout' , None, logging.DEBUG)

class Arguments:
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=textwrap.dedent(
                '''
                QIIFA: QTI Interface Integrity for Android

                please refer go/qiifa for info.

                examples:
                        1. For running interface integrity check for HIDL
                           python qiifa_main.py --type hidl

                        2. For running interface integrity check for ABI
                           python qiifa_main.py --type abi

                        3. For running interface integrity check for all
                           python qiifa_main.py --type all

                        4. For generating QIIFA CMD for HIDL
                           python qiifa_main.py --create <options> <intf-name>
                             options: hidl -> for generating full cmd
                                           -> don't give <intf-name> option here
                                           ex: python qiifa_main.py --create hidl

                                      hidl_one  -> for modifying the golden cmd entry for one interface
                                                -> make sure to give <intf-name> option
                                           ex: python qiifa_main.py --create hidl_one vendor.qti.some.intf

                                      hidl_new  -> for adding a new interface to the golden cmd
                                                -> make sure to give <intf-name> option
                                           ex: python qiifa_main.py --create hidl_new vendor.qti.some.intf

                        5. For generating QIIFA CMD for ABI
                           python qiifa_main.py --create abi

                        6. For interface integrity check for QSSI and vendor image
                           python qiifa_main.py --qssi < path for qiifa_cmd_qssi > --target < path for qiifa_cmd_qssi >
                '''))

        self.parser.add_argument("--type",
            help="If this argument is given, don't give any other arguments",
            required=False, type=str)

        self.parser.add_argument("--create",
            help="If this argument is given, don't give any other arguments",
            required=False, type=str, nargs='+')

        self.parser.add_argument("--target",
          help="Target CMD path",
          required=False, type=str)

        self.parser.add_argument("--qssi",
          help="qssi CMD path",
          required=False, type=str)

        self.parser.add_argument("--enforced",
          help="1 is enforced, 0 is NOT enforced",
          required=False, type=str)
