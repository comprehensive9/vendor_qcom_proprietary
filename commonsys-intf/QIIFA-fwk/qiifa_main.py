#!/usr/bin/python
# -*- coding: utf-8 -*-
#Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

'''
Import standard python modules
'''
import time,sys,os,errno,subprocess,argparse,tarfile,importlib

'''
Import plugin_interface base module
'''
import plugin_interface

'''
Import local utilities, constans, logger, arguement modules
'''
from qiifa_util.util import UtilFunctions, Variables, Constants, Logger, Arguments

def load_plugins():
    '''
    Description: dl_load defination dynamically loads modules from "/plugins/" folder
                 plugin filename MUST be _plugin.py
    Return     : List of modules
    '''
    pl_list = []
    root = os.path.dirname(os.path.realpath(__file__)) + "/plugins/"
    for dummy, dir, files in os.walk(root):
        head_tail = os.path.split(dummy)
        for ffile in files:
            if ffile.endswith('_plugin.py'):
                modulename = ffile.strip('.py')
                Logger.logInternal.info("calling for " + modulename)
                pl = None
                try:
                    pl = importlib.import_module('plugins.' + head_tail[1] + '.' + modulename)
                except (NameError, SyntaxError) as ex:
                    Logger.logInternal.info('Module %s cannot be loaded!', modulename)
                    Logger.logInternal.info(ex)
                    pl = None
                    sys.exit()
                pl_list.append(pl)
    return pl_list

def zip_artifacts():
    Logger.logInternal.info ("Creating tar file")
    UtilFunctions.makeTarFile(Constants.qiifa_cmd_tarfile, Constants.qiifa_current_cmd_dir)
    if Constants.target_product == "qssi":
        UtilFunctions.zip_dir(Constants.qiifa_code_path,Constants.qiifa_out_dir+"/QIIFA-fwk.zip")
    Logger.logInternal.info ("Done tar file")

def refresh_artifacts():
    if not UtilFunctions.dirExists(Constants.qiifa_out_dir):
        UtilFunctions.create_dir(Constants.qiifa_out_dir)
    UtilFunctions.rmdir(Constants.qiifa_current_cmd_dir)
    UtilFunctions.create_dir(Constants.qiifa_current_cmd_dir)

def main():
    '''
    Description: main function
    '''
    start = time.time()

    argsObj = Arguments()
    args = argsObj.parser.parse_args()

    pl_list = load_plugins()

    '''
    For argument type: --type
    '''
    arg_type = args.type
    arg_create = args.create
    arg_target = args.target
    arg_qssi = args.qssi
    arg_enforced = args.enforced

    if not UtilFunctions.validateArgs(arg_type, arg_create, arg_qssi, arg_target,pl_list):
        Logger.logStdout.info ("python qiifa_main.py -h\n")
        sys.exit(-1)

    if Constants.qiifa_out_dir != "" and arg_type != None:
        refresh_artifacts()

    if arg_enforced == "1":
        Variables.is_enforced = True
    else:
        Logger.logInternal.info("Not Enforced")
        Variables.is_enforced = False
    Variables.is_enforced = True

    Logger.logInternal.info("Checking for argument --type ")
    if arg_type == "all":
        for plugin in pl_list:
            if plugin != None:
                plugin.MyPlugin().register()
                plugin.MyPlugin().config()
                plugin.MyPlugin().IIC()
    elif arg_type != None:
        for plugin in pl_list:
            if plugin != None and arg_type != None:
                if arg_type == plugin.MyPlugin().register()[0]:
                    plugin.MyPlugin().config()
                    plugin.MyPlugin().IIC()
    if arg_type != None:
        zip_artifacts()

    '''
    For argument type: --create
    '''
    Logger.logInternal.info("Checking for argument --create")
    if arg_create != None and arg_create[0] == "all":
        for plugin in pl_list:
            if plugin != None:
                plugin.MyPlugin().register()
                plugin.MyPlugin().config()
                plugin.MyPlugin().generateGoldenCMD()
    elif arg_create != None:
        created = False
        for plugin in pl_list:
            if plugin != None and arg_create != None:
                ''' only validate the first in arg_create,
                    send the rest down the pipe for further validation'''
                if arg_create[0] in plugin.MyPlugin().register():
                    plugin.MyPlugin().config()
                    kwargs = {"create_args_lst":arg_create}
                    plugin.MyPlugin().generateGoldenCMD(**kwargs)
                    created = True
        if not created:
            Logger.logStdout.info ("python qiifa_main.py -h\n")
            sys.exit(-1)

    '''
    For argument type: --target
    '''
    if arg_qssi is not None and arg_target is not None:
        if not UtilFunctions.dirExists(arg_target):
            Logger.logStdout.error("target path " + arg_target + " doesn't exist")
            sys.exit(-1)
        elif not UtilFunctions.dirExists(arg_qssi):
            Logger.logStdout.error("qssi path " + arg_qssi + " doesn't exist")
            sys.exit(-1)
        status, t_file_name = UtilFunctions.searchAndUntarGzFile(arg_target, Constants.target_gz_file_name)
        if not status:
            Logger.logStdout.error("No.gz file found in the given target path")
            sys.exit(-1)
        status, q_file_name = UtilFunctions.searchAndUntarGzFile(arg_qssi, Constants.qssi_gz_file_name)
        if not status:
            Logger.logStdout.error("No .gz file found in the given qssi path")
            sys.exit(-1)
        for plugin in pl_list:
            if plugin != None:
                if "hidl" in plugin.MyPlugin().register():
                    kwargs = {"qssi_path":arg_qssi, "target_path":arg_target, "q_file_name":q_file_name, "t_file_name":t_file_name}
                    plugin.MyPlugin().IIC(**kwargs)
                    break

    '''
    end of main, exit with sucess
    '''
    if (Variables.is_violator == True):
        Logger.logStdout.error("\tFailure: There is/are interface integrity QIIFA violaters\n")
        Logger.logStdout.error("\tplease look at qiifa_violaters.txt file in out folder\n")
        Logger.logStdout.error("\tfor QSSI only, out/target/product/qssi/QIIFA_QSSI/qiifa_violaters.txt\n")
        Logger.logStdout.error("\tfor target," + Constants.logfileViolators.replace(Constants.croot + "/",'') + "\n")
    else:
        Logger.logStdout.info("\tThere are no QIIFA violators\n")
        Logger.logStdout.info("\tQIIFA: Succeess\n")

    if (Variables.is_enforced == True and Variables.is_violator == True):
        Logger.logStdout.error("\tQIIFA is in enforcing mode, returning failure to caller\n")
        sys.exit(-1)
    elif (Variables.is_enforced == False and Variables.is_violator == True):
        Logger.logStdout.info("\tThere are QIIFA violaters but QIIFA is not in enforcing mode, returning success to caller\n")

    sys.exit(0)

if __name__ == '__main__':
    main()
