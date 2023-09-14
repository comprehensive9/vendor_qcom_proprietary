#!/usr/bin/python
# -*- coding: utf-8 -*-
#Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

'''
Import standard python modules
'''
import os,json,sys,shutil

from . import xmltodict

from collections import OrderedDict

'''
Import local utilities
'''
from qiifa_util.util import UtilFunctions, Variables, Constants, Logger

'''
Import plugin_interface base module
'''
from plugin_interface import plugin_interface

'''
Global variables
'''
provide_data = {}
need_data = {}

def parse_xml_files():
    '''
    Description: parse_xml_files defination parses XML files
    Type       : Internal defination
    '''
    for xml_path in Constants.paths_of_xml_files:
        if not UtilFunctions.dirExists(xml_path):
            Logger.logStdout.info("Directory: " + xml_path + " Doesn't exist.")
            sys.exit(-1)
        common_parser(xml_path)
    #insert the need into the provider
    trim_need_data()
    trim_provide_data()
    insert_need_into_provide()

def skipintf_chk(intf_name):
    string_concatenate = ''
    for words in intf_name.split('.'):
        string_concatenate += words
        if string_concatenate in Constants.HIDL_SKIP_INTERFACE:
            return True
        string_concatenate += '.'
    return False

def parse_xml_files_for_create(xml_file_path, txt_file_data, arg_type, for_intf_name=None):
    if not UtilFunctions.dirExists(xml_file_path):
        Logger.logStdout.error("Directory: " + xml_file_path +" Doesn't exist.")
        sys.exit(-1)

    #assumption is that we are only concerned with FCM
    if arg_type == "all": # then no need to worry about for_intf
        full_create_parser(xml_file_path, txt_file_data)

    #again another safe assumption where if it isn't full cmd, then it must be
    #a single intf cmd generation
    else:
        single_create_parser(xml_file_path, txt_file_data, for_intf_name)

def parse_xml_files_for_check(txt_file_data, xml_files_path, target):
    if target == "qssi":
        handle_qssi_parse_and_check(txt_file_data, xml_files_path)
    else:
        parse_partial_xml_files(xml_files_path)

def handle_qssi_parse_and_check(txt_file_data, xml_files_path):
    root,dirs,files = next(os.walk(xml_files_path))
    for xml_file in files:
        if xml_file.startswith("compatibility_matrix."):
            #disect the name and find the level of the FCM
            split_name = xml_file.split('.')
            #get the level if it matches with known levels
            level_val = None if split_name[1] not in Constants.SUPPORTED_FCM_LEVELS else split_name[1]
            if level_val != None:
                parse_helper = DataParsingHelper(root, xml_file, level_val)
                xml_data = None
                xml_data = parse_helper.append_hashes(txt_file_data)
                cmd_data = parse_helper.get_cmd_data()
                #now that the parsing is done, let's hand it over to a check handler
                #routine
                handle_qssi_check(xml_data, cmd_data, level_val)
            else:
                if xml_file not in Constants.legacy_xml_files:
                    Logger.logStdout.warning("Unknown compatibility matrix file: " + os.path.join(root, xml_file))

def parse_partial_xml_files(xml_files_path):
    '''
    Description: This defination parses parial XML files when QSSI and target are separate SIs
    Type       : Internal defination
    '''
    if not UtilFunctions.dirExists(xml_files_path):
        Logger.logStdout.error("Directory: " + xml_files_path +" Doesn't exist.")
        sys.exit(-1)
    common_parser(xml_files_path)
    trim_provide_data()

def full_create_parser(xml_path, txt_file_data):
    for root, dirs, files in os.walk(xml_path):
        for x_file in files:
            #analyze x_file to see if it is one of the FCM we want
            if x_file.startswith("compatibility_matrix."):
                split_name = x_file.split('.')
                level_val = None if split_name[1] not in Constants.SUPPORTED_FCM_LEVELS else split_name[1]
                #at this point we can parse the files we are interested in
                if level_val != None:
                    #open and parse the FCM file
                    x_file_full_path = os.path.join(root, x_file)
                    fd = open(x_file_full_path)
                    dict_data = xmltodict.parse(fd.read())

                    #generate cmd file name and concatinate with db location
                    jsn_file_name = Constants.qiifa_cmd_prefix + '_' + level_val + Constants.json_file_suffix
                    jsn_file_path = Constants.qiifa_hidl_db_root + jsn_file_name

                    #trim the data so that only "hal" tags are present
                    trim_need_data(dict_data)

                    #before dumping into the cmd file, we must append the
                    #respective hashes to each hal entry.
                    append_hashes(txt_file_data, dict_data, jsn_file_name)

                    #write x_file_full_path into the cmd
                    Logger.logStdout.info("Writing to: " + jsn_file_name)
                    with open(jsn_file_path,"w") as jf:
                        json.dump(dict_data, jf, separators=(",", ": "), indent=4)
                else:
                    if x_file not in Constants.legacy_xml_files:
                        Logger.logStdout.warning("Unknown compatibility matrix file: " + os.path.join(root, x_file))

def single_create_parser(xml_file_path, txt_file_data, for_intf_name):
    found = False
    root,dirs,files = next(os.walk(xml_file_path))
    #since all the xml files will be at the top level directory
    #no need to have the outer for-loop
    for x_file in files:
        #analyze x_file to see if it is one of the FCM we want
        if x_file.startswith("compatibility_matrix."):
            split_name = x_file.split('.')
            level_val = None if split_name[1] not in Constants.SUPPORTED_FCM_LEVELS else split_name[1]
            #at this point we can parse the files we are interested in
            if level_val != None:
                #if for_intf_name.startswith("vendor."):
                #open and parse the FCM file
                x_file_full_path = os.path.join(root, x_file)
                fd = open(x_file_full_path)
                dict_data = xmltodict.parse(fd.read())

                #trim the data so that only "hal" tags are present
                trim_need_data(dict_data)

                #generate cmd file name and concatinate with db location
                jsn_file_name = Constants.qiifa_cmd_prefix + '_' + level_val + Constants.json_file_suffix
                jsn_file_path = Constants.qiifa_hidl_db_root + jsn_file_name
                if not UtilFunctions.fileExists(jsn_file_path):
                    Logger.logStdout.error("The cmd file " + jsn_file_name + " doesn't exist. Make sure it exists before proceeding.")
                    sys.exit()

                #look for the single intf in the parsed xml file
                intf_in_q = check_intf_name(dict_data, for_intf_name)
                if intf_in_q != None:
                    #we have found it from the xml file
                    found = True
                    #now we must load the coresponding cmd to which we can add it
                    #TODO: we must figure out exactly where we found it
                    golden_dict = json.load(open(jsn_file_path,"r"), object_pairs_hook=OrderedDict)
                    #is the update a modification of existing hal or a brand new one??
                    status, existing_intf  = is_intf_new(golden_dict, txt_file_data, intf_in_q)
                    if status:
                        #it must have been added to the cmd already since it is new
                        #we can just write the golden_dict back to the json file and be done
                        Logger.logStdout.info("Intf: " + for_intf_name + " not found in cmd. Adding it to the cmd..")
                        with open(jsn_file_path, "w") as jf:
                            json.dump(golden_dict, jf, separators=(",", ": "), indent=4)
                        #if it is a new interface, it is likely that this is the only cmd to which it needs to be added
                        break
                    else:
                        #at this point it must mean that the intf could be in two states
                        #  1. exists and not modiefied -> nothing to do here
                        #  2. exists and modified -> figure out what is modified and add to it
                        if not is_intf_modified(intf_in_q, existing_intf, txt_file_data):
                            if for_intf_name.startswith("vendor.") and level_val == "device":
                                #if the interface isn't modified, then we are done. display a message,
                                #set done to True, then break out of the first loop
                                Logger.logStdout.info("No new changes detected for intf: " + intf_in_q['name'] +  "\n")
                                break
                        else:
                            #we will assume that the two most important modifications are version and hash
                            #  1.make sure it doesn't remove existing versions and hashes
                            #  2.if it removes existing one, then makesure it exists in another cmd
                            vaa = validate_and_add(level_val, existing_intf, intf_in_q)
                            if vaa == None:
                                vaa = intf_in_q
                            golden_dict['compatibility-matrix']['hal'][golden_dict['compatibility-matrix']['hal'].index(existing_intf)] = vaa
                            with open(jsn_file_path, "w") as jf:
                                json.dump(golden_dict, jf, separators=(",", ": "), indent=4)
                else:
                    Logger.logStdout.info("Intf not found in: " + x_file + " moving on...")
            else:
                if x_file not in Constants.legacy_xml_files:
                    Logger.logStdout.warning("Unknown compatibility matrix file: " + os.path.join(root, x_file))
    if not found:
        Logger.logStdout.warning("Couldn't find the intf: " + for_intf_name + " in any of the xml files..")

def is_intf_new(golden_dict, txt_file_data, intf_in_q):
    #if the intf is brand new, this function should just
    #append it to the end of the hal list
    intf = check_intf_name(golden_dict, intf_in_q['name'])
    if intf == None:
        #this means we didn't find it and we can append it to the end
        #but before we add it, we must add hashes for it
        append_hash_for_single_create(intf_in_q, txt_file_data, True)
        golden_dict['compatibility-matrix']['hal'].append(intf_in_q)
        return True, intf
    return False, intf

def is_intf_modified(modified_intf, existing_intf, txt_file_data=None):
    #blindly insert the collected hash and do dict comparison
    #b/n the newly generated data and the existing data
    if txt_file_data != None:
        append_single_hash(modified_intf, txt_file_data, True)
    if not modified_intf == existing_intf:
        return True
    return False

def is_hash_modified(modified_intf_hash, existing_intf_hash):
    if not modified_intf_hash == existing_intf_hash:
        #hash has been somehow modified
        return True
    return False
def validate_and_add(level_val, existing_intf, modified_intf):
    #the modification hunting game begins
    #TODO: figure out what really has changed and update
    return None

def is_version_modified(existing_version, modified_version):
    return True if not existing_version == modified_version else False

def is_version_added_or_removed(existing_version, modified_version):
    if not isinstance(existing_version,list) and not isinstance(modified_version, list):
        if '-' not in existing_version:
            if is_valid_version(modified_version, existing_version):
                print("Not implemented yet")

def handle_qssi_check(fcm_data, cmd_data, level_val):
    local_skipped_intf_list = get_skipped_intf_list()
    for fcm_entry in fcm_data['compatibility-matrix']['hal']:
        #first, let's check if the interface is new
        if local_skipped_intf_list and fcm_entry['name'] in local_skipped_intf_list:
            Logger.logInternal.info("Skipping HIDL interface for:"+fcm_entry['name'])
            continue
        if fcm_entry['@format'] == "aidl":
            continue
        res = check_dup_intf_name(cmd_data, fcm_entry['name'], True)
        if res == None:
            UtilFunctions.print_violators(fcm_entry['name'], "Hidl interface doesn't exist in the coresponding QIIFA cmd. please add it to the cmd","","")
            continue
        #at this point, the fcm_entry must exist in the coresponding
        #cmd which means we can check for its validity and we know exactly
        #to which cmd entry it coresponds from the return of check_dup_intf_name
        elif not isinstance(res, list):
            #no duplicate for this intf. most of the cases will fall through
            #this case
            is_modified = is_intf_modified(fcm_entry, res)
            if not is_modified:
                continue
            else:
                block_by_block_check(fcm_entry, res, level_val)
        else:
            #duplicates found, we will have to go one by one and
            #find a coresponding match
            if fcm_entry not in res:
                #something has been modified
                #this will be the most difficult to figure out what is wrong
                found_match = False
                matching_hal = None
                for intf in res:
                    is_modified = check_intf(fcm_entry['interface'], intf['interface'], fcm_entry['name'])
                    if is_modified.err_code == Constants.HIDL_INTF_NO_ERR:
                        found_match = True
                        matching_hal = intf
                        break
                if not found_match:
                    UtilFunctions.print_violators(fcm_entry['name'], Constants.HIDL_INTF_MISMATCH_ERR, "","")
                else:
                    if is_intf_modified(fcm_entry, matching_hal):
                        block_by_block_check(fcm_entry, matching_hal, level_val)

def block_by_block_check(fcm_hal, cmd_hal, level_val):
    #if we have gotten here, it is because there was a definite mismatch
    #at a high level, lets first figure out which section is causing the mismatch
    is_c_variant = Constants.IS_CUSTOMER_VARIANT
    try:
        if fcm_hal['@format'] != cmd_hal['@format']:
            UtilFunctions.print_violators(cmd_hal['name'],"Format Mismatch between QIIFA CMD and compatibility matrix. Check go/qiifa for more information","","")
        elif fcm_hal['version'] != cmd_hal['version']:
            #call version block check to handle it
            version_block_check(fcm_hal['version'], cmd_hal['version'], cmd_hal['name'],level_val)
            #it is safe to return now
            return
        elif not is_c_variant and fcm_hal['hash_info'] != cmd_hal['hash_info']:
            #call hash block check to handle it
            hash_block_check(fcm_hal['hash_info'], cmd_hal['hash_info'], cmd_hal['name'])
            #safe to return here because whatever error found downstream will have been
            #already reported
            return
        elif fcm_hal['interface'] != cmd_hal['interface']:
            #call intf block check to handle it
            intf_block_check(fcm_hal['interface'], cmd_hal['interface'], cmd_hal['name'])
            #safe to return here because whatever error found downstream will have been
            #already reported
            return
        else:
            if not is_c_variant:
                UtilFunctions.print_violators(cmd_hal['name'], \
                            "Something minor has been modified for the above interface. Please double check",
                            "","")
    except KeyError as err:
        if str(err).strip("'") == "hash_info":
            if "hash_info" not in cmd_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The Hash_Info tag is not present in the QIIFA CMD. Please update before proceeding. Check go/qiifa for more information","","")
            elif "hash_info" not in fcm_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The Hash_Info tag is not present in the Compatibility matrix or current.txt. Please update before proceeding. Check go/qiifa for more information","","")
        elif str(err).strip("'") == "interface":
            if "interface" not in cmd_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The Interface tag is not present in the QIIFA CMD. Please update before proceeding. Check go/qiifa for more information","","")
            elif "interface" not in fcm_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The Interface tag is not present in the Compatibility matrix or current.txt. Please update before proceeding. Check go/qiifa for more information","","")
        elif str(err).strip("'") == "version":
            if "version" not in cmd_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The version tag is not present in the QIIFA CMD. Please update before proceeding. Check go/qiifa for more information","","")
            elif "version" not in fcm_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The version tag is not present in the Compatibility matrix or current.txt. Please update before proceeding. Check go/qiifa for more information","","")
        elif str(err).strip("'") == "@format":
            if "@format" not in cmd_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The format tag is not present in the QIIFA CMD. Please update before proceeding. Check go/qiifa for more information","","")
            elif "@format" not in fcm_hal.keys():
                UtilFunctions.print_violators(cmd_hal['name'],"The format tag is not present in the Compatibility matrix or current.txt. Please update before proceeding. Check go/qiifa for more information","","")
            return
        else:
            UtilFunctions.print_violators(cmd_hal['name'],"Some tag is not present in the QIIFA CMD or in current.txt file. Please update before proceeding. Check go/qiifa for more information","","")

def version_block_check(fcm_version, cmd_version, intf_name, level_val):
    res = version_check(fcm_version, cmd_version, intf_name)
    if res.err_code != Constants.HIDL_VERSION_NO_ERR:
        #does this qualify for false positive check?
        #assuming that *_device.xml entries don't qualify for
        #false positive check. perhaps extend disqualification to
        # intfs that start with vendor.
        if level_val == "device":
            #report version error
            UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)
            #we should probably not bother checking the hash if there is a version mismatch
            return
        else:
            #intfs here qualify for false positive check
            #lets load other cmds except the one we are analyzing and the *_device.* cmd
            extra_fcm_hal = []
            extra_cmd_hal = []
            for sfv in Constants.SUPPORTED_FCM_LEVELS:
                if sfv != level_val and sfv != "device":
                    #construct the xml file name using the sfv value
                    xml_file = "compatibility_matrix." + sfv + ".xml"
                    arb_data = DataParsingHelper(Constants.paths_of_xml_files[0], xml_file, sfv)

                    single_fcm_entry = check_intf_name(arb_data.get_raw_xml_data(), intf_name)
                    if single_fcm_entry != None:
                        extra_fcm_hal.append(single_fcm_entry)

                    single_cmd_entry = check_intf_name(arb_data.get_cmd_data(), intf_name)
                    if single_cmd_entry != None:
                        extra_cmd_hal.append(single_cmd_entry)
            #now we can call a subroutine to take it from here, but first we have to check
            #if either of the extra lists are empty.
            if len(extra_fcm_hal) == 0 or len(extra_cmd_hal) == 0:
                #in this case, we don't need to do a consolidated check.
                #just report an error and return
                UtilFunctions.print_violators(fcm_entry['name'],Constants.ERR_HIDL_VERSIONS_MISMATCH,str(cmd_version), str(fcm_version))
                return

            fpc_res = version_consolidated_check(fcm_version, cmd_version, extra_fcm_hal, extra_cmd_hal, intf_name)
            if fpc_res.err_code != Constants.HIDL_VERSION_NO_ERR:
                #report genuine errors
                UtilFunctions.print_violators(res.intf_name,res.err_code, str(cmd_version), str(fcm_version))
            else:
                #not an error but should still notify as a warning
                Logger.logStdout.warning("The versions in cmd and FCM seem to be out of sync for: " + intf_name + " Please update it.")

def version_consolidated_check(fcm_version, cmd_version, extra_fcm_hals, extra_cmd_hals, intf_name):
    cons_fcm_version, cons_cmd_version = consolidate_versions(fcm_version, cmd_version, extra_fcm_hals, extra_cmd_hals)
    return version_check(cons_fcm_version, cons_cmd_version, intf_name)

def consolidate_versions(fcm_v, cmd_v, extra_fcm_hs, extra_cmd_hs):
    c_fcm_v = []
    c_cmd_v = []
    c_fcm_v.append(fcm_v) if not isinstance(fcm_v,list) else c_fcm_v.extend(v for v in fcm_v if v not in c_fcm_v)
    for fcm_hal in extra_fcm_hs:
        if isinstance(fcm_hal['version'],list):
            c_fcm_v.extend(v for v in fcm_hal['version'] if v not in c_fcm_v)
        else:
            if fcm_hal['version'] not in c_fcm_v:
                c_fcm_v.append(fcm_hal['version'])

    c_cmd_v.append(cmd_v) if not isinstance(cmd_v,list) else c_cmd_v.extend(v for v in cmd_v if v not in c_cmd_v)
    for cmd_hal in extra_cmd_hs:
        if isinstance(cmd_hal['version'],list):
            c_cmd_v.extend(v for v in cmd_hal['version'] if v not in c_cmd_v)
        else:
            if cmd_hal['version'] not in c_cmd_v:
                c_cmd_v.append(cmd_hal['version'])

    return c_fcm_v,c_cmd_v

def hash_block_check(fcm_hash, cmd_hash, intf_name):
    #lets check hash now.also, no need to go through false positive process here because the hash is only
    #here if the version is here.
    hsh_res = check_hash(fcm_hash, cmd_hash, intf_name)
    if hsh_res.err_code != Constants.HIDL_HASH_NO_ERR:
        #report error and return, no need to proceed for this interface
        intf_skip_chk = skipintf_chk(intf_name)
        if not intf_skip_chk:
            UtilFunctions.print_violators(hsh_res.gldn_hash['name'],hsh_res.err_code,"","")
        else:
            Logger.logInternal.info("ABI PRESERVED INTF " + intf_name)
        return

def intf_block_check(fcm_intf, cmd_intf, intf_name):
    #if we have gotten this far, then interface info might have changed
    #so lets check that
    intf_res = check_intf(fcm_intf, cmd_intf, intf_name)
    if intf_res.err_code != Constants.HIDL_INTF_NO_ERR:
        UtilFunctions.print_violators(intf_name, intf_res.err_code,"","")
        return

def common_parser(xml_path):
    '''
    Description: This defination is common parsing that will be used by other definations
    Type       : Internal defination
    '''
    for root, dirs, files in os.walk(xml_path):
        for x_file in files:
            if not x_file in Constants.legacy_xml_files:
                file_path = os.path.join(root,x_file)
                with open(file_path) as fd:
                    new_dict = xmltodict.parse(fd.read())
                    if 'manifest' in new_dict and 'framework' == new_dict['manifest']['@type']: #framework provide
                        append_provide_info(new_dict, new_dict['manifest']['@type'])
                    elif 'manifest' in new_dict and 'device' == new_dict['manifest']['@type']:  #device provide
                        append_provide_info(new_dict, new_dict['manifest']['@type'])
                    elif 'compatibility-matrix' in new_dict and 'framework' == new_dict['compatibility-matrix']['@type']: #framework need
                        append_need_info(new_dict)
                    elif 'compatibility-matrix' in new_dict and 'device' == new_dict['compatibility-matrix']['@type']: #device need
                        append_need_info(new_dict)
            else:
                Logger.logInternal.info("Skiping " + x_file)

def get_skipped_intf_list():
    skipped_intf_list = []
    if(UtilFunctions.fileExists(Constants.hidl_skipped_intf)):
        skipped_intf_fh = open(Constants.hidl_skipped_intf ,"r")
        skipped_intf_list = []
        for lib_path in skipped_intf_fh:
            skipped_intf_list.append(lib_path.rstrip())
        skipped_intf_fh.close()
    else:
        Logger.logInternal.info("Hidl skip interface file does not exist:" + Constants.hidl_skipped_intf)
    return skipped_intf_list

def trim_need_data(data=None):
    '''
    Description: This defination trims need data from dictionary
    Type       : Internal defination
    '''
    if data == None:
        data = need_data #rarely the case
    for key,value in list(data['compatibility-matrix'].items()):
        if not key.startswith('@') and not key == 'hal':
            del data['compatibility-matrix'][key]

def trim_provide_data():
    '''
    Description: This defination trims provide data from dictionary
    Type       : Internal defination
    '''
    for key,value in list(provide_data['manifest'].items()):
        if not key.startswith('@') and not key == 'hal':
            del provide_data['manifest'][key]

    for item in provide_data['manifest']['hal']:
        if not isinstance(item, dict):
           provide_data['manifest']['hal'].remove(item)

def is_valid_version(single_cm_version, hash_version):
    if not '-' in single_cm_version:
        return True if single_cm_version == hash_version else False
    else:
        #requires breaking down the format major.minor-minor
        split_cm_v = single_cm_version.split('-')         # split to ['major.minor'] and ['minor']
        sub_split_cm_v = split_cm_v[0].split('.')  # split to ['major'] and ['minor']
        split_hash_v = hash_version.split('.')     # split to ['major'] and ['minor']
        #first check if the major versions are the same
        #if not don't bother proceeding
        if sub_split_cm_v[0] == split_hash_v[0]:
            #then see if minor version is within the range
            if int(split_hash_v[1]) >= int(sub_split_cm_v[1]) and int(split_hash_v[1]) <= int(split_cm_v[1]):
                return True
    return False

def is_single_v_in_list(cm_version, single_v):
    for v in cm_version:
        if is_valid_version(v, single_v):
            return True
    return False

def is_hal_version_in_cm(cm_version, hal_version, name=None):
    #assumption here is that hal_version will not have '-'
    #but it can be a list
    if not isinstance(cm_version, list):
        if not isinstance(hal_version, list):
            if not is_valid_version(cm_version, hal_version):
                return VersionErrorInfo(str(cm_version), str(hal_version), name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
    else:
        if not isinstance(hal_version, list):
            if not is_single_v_in_list(cm_version, hal_version):
                return VersionErrorInfo(str(cm_version), str(hal_version), name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
        else:
            for h_v in hal_version:
                if not is_single_v_in_list(cm_version, h_v):
                    return VersionErrorInfo(str(cm_version), str(hal_version), name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
    return VersionErrorInfo("", "", name, Constants.HIDL_VERSION_NO_ERR)

def is_valid_intfs(cm_intfs, hal_intfs, name=None):
    if not isinstance(cm_intfs, list):
        if not isinstance(hal_intfs, list):
            if hal_intfs['name'] != cm_intfs['name']:
                return IntfErrorInfo(cm_intfs, hal_intfs, name, Constants.HIDL_INTF_MISMATCH_ERR)
        else:
            for hal_i in hal_intfs:
                if hal_i['name'] != cm_intfs['name']:
                    return IntfErrorInfo(cm_intfs, hal_intfs, name, Constants.HIDL_INTF_MISMATCH_ERR)
    else:
        if not isinstance(hal_intfs, list):
            is_good = False
            for cm_i in cm_intfs:
                if hal_intfs['name'] == cm_i['name']:
                    is_good = True
                    break
            if not is_good:
                return IntfErrorInfo(cm_intfs, hal_intfs, name, Constants.HIDL_INTF_MISMATCH_ERR)
        else:
            for hal_i in hal_intfs:
                is_good = False
                for cm_i in cm_intfs:
                    if cm_i['name'] == hal_i['name']:
                        is_good = True
                        break
                if not is_good:
                    return IntfErrorInfo(cm_intfs, hal_intfs, name, Constants.HIDL_INTF_MISMATCH_ERR)
    return IntfErrorInfo("", "", name, Constants.HIDL_INTF_NO_ERR)

def is_hash_version_in_cm(cm_version, hash_version):
    #hash_version will be a single value
    #cm_version can have multiple forms
    #  1.single entry
    #  2.single entry with major.minor-minor
    #  3.list of entries with a mix of single value
    #    and values with major.minor-minor
    if not isinstance(cm_version,list):
        return is_valid_version(cm_version, hash_version)
    else:
        for cm_v in cm_version:
            if is_valid_version(cm_v, hash_version):
                return True
    return False

def append_hashes(txt_file_data, xml_file_data, cmd_name=None, is_for_check=False):
    #TODO: this function should eventually replace insert_hash() below
    for hal in xml_file_data['compatibility-matrix']['hal']:
        if hal['@format'] =="hidl":
            append_single_hash(hal, txt_file_data, is_for_check)
        elif hal['@format'] =="aidl":
            Logger.logStdout.warning("aidl interface found: " + hal['name'])
            continue
        else:
            UtilFunctions.print_violators(hal['name'],"No versions found in FCM entry. Check go/qiifa for more information","","")
            return

def append_single_hash(hal, txt_file_data, is_for_single, is_for_create=False):
    at_least_one_hash_found = False
    for hash_info in txt_file_data:
        if hash_info['name'] == hal['name']:
            if 'version' in hal:
                if is_hash_version_in_cm(hal['version'], hash_info['version']):
                    if 'hash_info' not in hal:
                        hal['hash_info'] = []
                    hal['hash_info'].append(hash_info)
                    at_least_one_hash_found = True
            elif hal['@format'] != "aidl":
                #TODO: highlight this in meetings
                UtilFunctions.print_violators(hal['name'],"No versions found in FCM entry. Check go/qiifa for more information","","")
                return
            else:
                Logger.logStdout.warning("aidl interface found: " + hal['name'])
                #since this is aidl and it has no version or hash
                #we will pretend as if we found a hash for it
                at_least_one_hash_found = True
                break

    if not at_least_one_hash_found and hal['name'] not in Constants.unhashed_intfs:
        if is_for_single and not Constants.IS_CUSTOMER_VARIANT:
            UtilFunctions.print_violators(hal['name'], "No hash found for intf above. Please hash the interface and try again.","","")
            if is_for_create:
                #if user running single cmd create, we should notify error
                #and immediately bail.
                sys.exit()
        else:
            Logger.logStdout.warning("No hash found for intf: " + hal['name'])

def append_hash_for_single_create(hal, txt_file_data, is_new):
    append_single_hash(hal, txt_file_data, True,is_for_create=True)

def insert_hash(name, hash_info, version):
    '''
    Description: This defination inserts HASH in to dictionary
    Type       : Internal defination
    '''
    for x in provide_data['manifest']['hal']:
        if isinstance(x,dict) and  x['name'] == name:
            if 'version' in x and x['version'] == version:
                if not 'hash_info' in x:
                    x['hash_info'] = []
                    x['hash_info'].append(hash_info)
                else:
                    x['hash_info'].append(hash_info)

def insert_provide(parsed_xml_data):
    '''
    Description: This defination trims need data from dictionary
    Type       : Internal defination
    '''
    global provide_data
    if not isinstance(parsed_xml_data['manifest']['hal'], list):
        provide_data['manifest']['hal'].append(parsed_xml_data['manifest']['hal'])
        return
    for provide in parsed_xml_data['manifest']['hal']:
        provide_data['manifest']['hal'].append(provide)

def insert_need(parsed_xml_data):
    '''
    Description: This defination insert need data in to dictionary
    Type       : Internal defination
    '''
    global need_data
    if not isinstance(parsed_xml_data['compatibility-matrix']['hal'], list):
        need_data['compatibility-matrix']['hal'].append(parsed_xml_data['compatibility-matrix']['hal'])
        return
    for need in parsed_xml_data['compatibility-matrix']['hal']:
        need_data['compatibility-matrix']['hal'].append(need)

def insert_need_into_provide():
    '''
    Description: This defination insert need data in to provide section
    Type       : Internal defination
    '''
    global provide_data
    global need_data
    for x in provide_data['manifest']['hal']:
        for y in need_data['compatibility-matrix']['hal']:
            if isinstance(x,dict) and isinstance(y,dict) and x['name'] == y['name']:
                if not 'need' in x:
                    x['need'] = []
                    x['need'].append(y)
                else:
                    x['need'].append(y)

def check_hash(fcm_hash, cmd_hash, intf_name):
    #TODO: this routine should eventually replace the
    #hash_check routine below
    if fcm_hash == cmd_hash:
        #just incase the problem is neither version nor hash
        #it is possible if the interface section is modified
        return HashErrorInfo("","",intf_name, Constants.HIDL_VERSION_NO_ERR)
    else:
        #should we consider cmd as a superset even for hash??
        for fcm_hsh_blck in fcm_hash:
            has_match = False
            for cmd_hsh_blck in cmd_hash:
                if cmd_hsh_blck['intf'] == fcm_hsh_blck['intf'] and \
                   cmd_hsh_blck['version'] == fcm_hsh_blck['version']:
                    if cmd_hsh_blck['hash'] != fcm_hsh_blck['hash']:
                        #at this point it means the hash was modified
                        return HashErrorInfo(cmd_hsh_blck,fcm_hsh_blck,intf_name,Constants.HIDL_HASH_MISMATCH_ERR)
                    else:
                        has_match = True
                        break
            if not has_match:
                return HashErrorInfo(cmd_hsh_blck,fcm_hsh_blck,intf_name,Constants.HIDL_HASH_ADDED_ERR)
    return HashErrorInfo("","","",Constants.HIDL_HASH_NO_ERR)

def check_intf(fcm_intf, cmd_intf, intf_name):
    if isinstance(fcm_intf,list) and isinstance(cmd_intf,list):
        for f_intf in fcm_intf:
            found = False
            for c_intf in cmd_intf:
                if c_intf['name'] == f_intf['name']:
                    found = True
                    break
            if not found:
                 return IntfErrorInfo(cmd_intf, fcm_intf, intf_name, Constants.HIDL_INTF_MISMATCH_ERR)
    elif isinstance(fcm_intf,list) and not isinstance(cmd_intf,list):
        return IntfErrorInfo(cmd_intf, fcm_intf, intf_name, Constants.HIDL_INTF_MISMATCH_ERR)
    elif isinstance(cmd_intf, list) and not isinstance(fcm_intf, list):
        found = False
        for c_intf in cmd_intf:
            if c_intf['name'] == fcm_intf['name']:
                found = True
                break
        if not found:
            return IntfErrorInfo(cmd_intf, fcm_intf, intf_name, Constants.HIDL_INTF_MISMATCH_ERR)
    else:
        if fcm_intf['name'] != cmd_intf['name']:
            return IntfErrorInfo(cmd_intf, fcm_intf, intf_name, Constants.HIDL_INTF_MISMATCH_ERR)
    return IntfErrorInfo("","","",Constants.HIDL_INTF_NO_ERR)

def hash_check(golden_dict):
    '''
    Description: This defination check HASH
    Type       : Internal defination
    '''
    for current_data in provide_data['manifest']['hal']:
        for golden_data in golden_dict['manifest']['hal']:
            if isinstance(golden_data, dict) and isinstance(current_data, dict) and golden_data['name'] == current_data['name'] and \
                 golden_data['name'] not in Constants.skipped_intf  and 'version' in golden_data and 'version' in current_data and \
                 golden_data['version'] == current_data['version']:
                if 'hash_info' in golden_data and 'hash_info' in current_data :
                    for hash_info in golden_data['hash_info']:
                        checker = False
                        local_hash_info = None
                        local_curr_hash = None
                        for curr_hash in current_data['hash_info']:
                            if hash_info['name'] == curr_hash['name']:
                                if hash_info['hash'] == curr_hash['hash']:
                                    checker = True
                                    break
                                else:
                                    local_hash_info = hash_info['hash']
                                    local_curr_hash = curr_hash['hash']
                        if checker == False:
                            UtilFunctions.print_violators(golden_data['name'], Constants.ERR_HIDL_MODIFIED_EXISTING_HASH,
                                local_hash_info, local_curr_hash)
                else:
                   pass

def version_check(current_version, golden_version, golden_name):
    '''
    Description: This defination is top level function to verify dictionaries
    Type       : Internal defination
    '''
    if isinstance(golden_version, list) and isinstance(current_version,list):
        if len(golden_version) >= len(current_version):
            for cv in current_version:
                cv_has_, checker, cv_split_by_ = False, False, []
                if '-' in cv:
                    cv_has_, cv_split_by_ = True, cv.split('-')
                for gv in golden_version:
                    if not cv_has_ and len(cv_split_by_) == 0 and '-' not in gv:
                        if cv == gv:
                            checker = True
                            break
                    elif cv_has_ and len(cv_split_by_) != 0 and '-' in gv:
                        if cv_split_by_[0] == gv.split('-')[0] and int(cv_split_by_[1]) <= int(gv.split('-')[1]):
                            checker = True
                            break
                    elif not cv_has_ and '-' in gv:
                        if cv == gv.split('-')[0]:
                            checker = True
                            break
                if checker == False:
                    return VersionErrorInfo(str(golden_version)[1:-1], str(current_version)[1:-1], golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
        else:
            return VersionErrorInfo(str(golden_version)[1:-1], str(current_version)[1:-1], golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)

    elif isinstance(golden_version, list) and not isinstance(current_version,list):
        cv_has_, gv_has_ = True if '-' in current_version else False, True if '-' in golden_version[0] else False

        if not cv_has_ and not gv_has_ and not current_version == golden_version[0]:
            return VersionErrorInfo(str(golden_version)[1:-1], current_version, golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
        elif not cv_has_ and gv_has_:
            if not current_version == golden_version[0].split('-')[0]:
                return VersionErrorInfo(str(golden_version)[1:-1], current_version, golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
        elif cv_has_ and not gv_has_:
            return VersionErrorInfo(str(golden_version)[1:-1], current_version, golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
        elif cv_has_ and gv_has_:
            if current_version.split('-')[0] == golden_version[0].split('-')[0]:
                if not int(current_version.split('-')[1]) <= int(golden_version[0].split('-')[1]):
                    return VersionErrorInfo(str(golden_version)[1:-1], current_version, golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
            else:
                return VersionErrorInfo(str(golden_version)[1:-1], current_version, golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
    elif not isinstance(golden_version, list) and isinstance(current_version,list):
        return VersionErrorInfo(golden_version, str(current_version)[1:-1], golden_name, Constants.ERR_HIDL_VERSIONS_MISMATCH)
    else:
        if '-' in golden_version and '-' in current_version:
            g_split = golden_version.split('-')
            c_split = current_version.split('-')
            if float(g_split[0]) == float(c_split[0]):
                if int(g_split[1]) < int(c_split[1]):
                    return VersionErrorInfo(golden_version, current_version, golden_name, Constants.ERR_HIDL_NOT_SAME_NEED_VERSIONS)
            else:
                return VersionErrorInfo(golden_version, current_version, golden_name, Constants.ERR_HIDL_NOT_SAME_NEED_VERSIONS)
        elif '-' in golden_version and '-' not in current_version:
            g_split = golden_version.split('-')
            if float(g_split[0]) != float(current_version):
                return VersionErrorInfo(golden_version, current_version, golden_name, Constants.ERR_HIDL_NOT_SAME_NEED_VERSIONS)
        elif '-' in current_version and '-' not in golden_version:
            return VersionErrorInfo(golden_version, current_version, golden_name, Constants.ERR_HIDL_NOT_SAME_NEED_VERSIONS)
        else:
            if golden_version != current_version:
                return VersionErrorInfo(golden_version, current_version, golden_name, Constants.ERR_HIDL_NOT_SAME_NEED_VERSIONS)
    return VersionErrorInfo("", "",golden_name, Constants.HIDL_VERSION_NO_ERR)

def full_version_check(golden_dict):
    '''
    Description: This defination checks version checks for full CMDs
    Type       : Internal defination
    '''
    for current_data in provide_data['manifest']['hal']:
        for golden_data in golden_dict['manifest']['hal']:
            if isinstance(golden_data, dict) and isinstance(current_data, dict) and golden_data['name'] == current_data['name'] and \
                golden_data['name'] not in Constants.skipped_intf:
                if 'need' in golden_data and not 'need' in current_data:
                    UtilFunctions.print_violators(golden_data['name'], Constants.ERR_HIDL_NOT_PRESENT_IN_GOLDEN, "", "")
                if not 'need' in golden_data and 'need' in current_data:
                    UtilFunctions.print_violators(golden_data['name'], Constants.ERR_HIDL_NOT_PRESENT_IN_GOLDEN, golden_data['version'], current_data['version'])

                if 'need' in golden_data and 'need' in current_data:
                    if len(golden_data['need']) > 1 and len(current_data['need']) > 1:
                        for golden_need in golden_data['need']:
                            checker = False
                            res = None
                            for current_need in current_data['need']:
                                if 'version' in current_need and 'version' in golden_need:
                                    res = version_check(current_need['version'],golden_need['version'],golden_data['name'])
                                    if Constants.HIDL_VERSION_NO_ERR == res.err_code:
                                        checker = True
                                        break
                            if checker == False and res != None:
                                UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)

                    elif len(golden_data['need']) == 1 and len(current_data['need']) > 1:
                        for current_need in current_data['need']:
                            if 'version' in current_need and 'version' in golden_data['need'][0]:
                                res = version_check(current_need['version'],golden_data['need'][0]['version'],golden_data['name'])
                                if Constants.HIDL_VERSION_NO_ERR == res.err_code:
                                    break
                                elif current_need == current_data['need'][len(current_data)-1]:
                                    UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)

                    elif len(golden_data['need']) > 1 and len(current_data['need']) == 1:
                        for golden_need in golden_data['need']:
                            if 'version' in golden_need and 'version' in current_data['need'][0]:
                                res = version_check(current_data['need'][0]['version'],golden_need['version'],golden_data['name'])
                                if Constants.HIDL_VERSION_NO_ERR == res.err_code:
                                    break
                                elif golden_need == golden_data['need'][len(golden_data['need'])-1]:
                                    UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)

                    elif len(golden_data['need']) == 1 and len(current_data['need']) ==1:
                        if 'version' in golden_data['need'][0] and 'version' in current_data['need'][0]:
                            res = version_check(current_data['need'][0]['version'],golden_data['need'][0]['version'],golden_data['name'])
                            if Constants.HIDL_VERSION_NO_ERR != res.err_code:
                                UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)

def partial_version_check(golden_dict):
    '''
    Description: This defination checks version for partion CMDs
    Type       : Internal defination
    '''
    for current_data in need_data['compatibility-matrix']['hal']:
        for golden_data in golden_dict['manifest']['hal']:
            if isinstance(golden_data, dict) and isinstance(current_data,dict) and golden_data['name'] == current_data['name'] \
                    and golden_data['name'] not in Constants.skipped_intf:
                if 'need' in golden_data and 'version' in current_data:
                    if len(golden_data['need']) > 1:
                        found = False
                        res = None
                        for golden_need in golden_data['need']:
                            if 'version' in golden_need:
                                res = version_check(current_data['version'], golden_need['version'], current_data['name'])
                                if res.err_code == Constants.HIDL_VERSION_NO_ERR:
                                    found = True
                                    break
                        if found == False and res != None:
                            UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)
                        break
                    elif len(golden_data['need'])== 1:
                        if 'version' in golden_data['need'][0]:
                            res = version_check(current_data['version'], golden_data['need'][0]['version'], current_data['name'])
                            if res.err_code != Constants.HIDL_VERSION_NO_ERR:
                                UtilFunctions.print_violators(res.intf_name,res.err_code,res.golden_ver, res.current_ver)
                            break

def check_intf_name(current_data, intf_name):
    for c_data in current_data['compatibility-matrix']['hal']:
        if isinstance(c_data,dict) and c_data['name'] == intf_name:
            return c_data
    return None

def check_dup_intf_name(current_data, intf_name, find_dup):
    intfs = []
    for c_data in current_data['compatibility-matrix']['hal']:
        if isinstance(c_data, dict) and c_data['name'] == intf_name:
            if find_dup:
                intfs.append(c_data)
            else:
                return c_data
    if len(intfs) == 0:
        return None
    elif len(intfs) == 1:
        return intfs[0]
    return intfs

def copy_qssi_artifacts():
    for f in os.listdir(Constants.qiifa_hidl_db_root):
        if f.endswith(".json"):
            src = Constants.qiifa_hidl_db_root + f
            dest = Constants.qiifa_current_cmd_dir + "/" + f
            try:
                shutil.copyfile(src, dest)
            except:
                Logger.logStdout.error("Couldn't copy qssi artifacts to out.")

def dump_to_json_file(arg_create_type,
                      arg_intf_name=None,
                      golden_db=None):
    '''
    Description: This defination dump to JSON file
    Type       : Internal defination
    '''

    global provide_data
    if arg_create_type == Constants.HIDL_SUPPORTED_CREATE_ARGS[0]:
        with open(Constants.qiifa_hidl_db_path,"w") as json_file:
            app_json = json.dump(provide_data, json_file, separators=(",", ": "), indent=4)
    else:
        intf_in_q = check_intf_name(provide_data, arg_intf_name)
        if intf_in_q is None:
            Logger.logStdout.error("Invalid intf name: " + arg_intf_name)
            sys.exit(-1)
        else:
            intf_found = False
            for g_data in golden_db['manifest']['hal']:
                if isinstance(g_data,dict) and g_data['name'] == intf_in_q['name']:
                    intf_found = True
                    golden_db['manifest']['hal'][golden_db['manifest']['hal'].index(g_data)] = intf_in_q
                    break
            if not intf_found:
                Logger.logStdout.info("Intf: " + arg_intf_name + " not found in cmd. Adding it to cmd..")
                golden_db['manifest']['hal'].append(intf_in_q)
        with open(Constants.qiifa_hidl_db_path,"w") as jf:
            json.dump(golden_db, jf, separators=(",", ": "), indent=4)

def dump_to_temp_json_file(cmd=None):
    '''
    Description: This defination dump to temparory JSON file
    Type       : Internal defination
    '''
    global provide_data
    if cmd == None:
        cmd = Constants.qiifa_out_temp_cmd
    if UtilFunctions.pathExists(cmd):
        os.remove(cmd)

    with open(cmd,"w") as json_file:
        app_json = json.dump(provide_data, json_file, indent=4)

def get_vndr_cmd_ver(path):
    for root, dr, files in os.walk(path):
        for file in files:
            if file.endswith(".json"):
                if "2.0" in file:
                    return "2.0"
                else:
                    break
    return ""

def get_target_level(vndr_cmd):
    if '@target-level' in vndr_cmd['manifest']:
        return vndr_cmd['manifest']['@target-level']
    return Constants.SUPPORTED_FCM_LEVELS[0]

def is_hal_exemptible(name, vndr_cmd_ver):
    v_ex,fq_ex,intf_ex = False, False, False
    if vndr_cmd_ver != "" or name not in Constants.QIIFA_1_0_WHITE_LIST:
        #No one is exempted unless 1.0
        return v_ex,fq_ex,intf_ex

    if 'all' in Constants.QIIFA_1_0_WHITE_LIST[name]:
        v_ex = True
        fq_ex = True
        intf_ex = True
    if 'v' in Constants.QIIFA_1_0_WHITE_LIST[name]:
        v_ex = True
    if 'fq' in Constants.QIIFA_1_0_WHITE_LIST[name]:
        fq_ex = True
    if 'intf' in Constants.QIIFA_1_0_WHITE_LIST[name]:
        intf_ex = True
    return v_ex,fq_ex,intf_ex

def load_qssi_cmds(path, target_level):
    qssi_cmds = []
    for l_val in Constants.SUPPORTED_FCM_LEVELS:
        #for the time being load FCMs >= target-level
        if l_val == "device":
            Logger.logStdout.info("loading target-level "+l_val+ " ......")
            qssi_cmds.append(DataParsingHelper(path, None, l_val, img_to_img=True))
        else:
            if l_val >= target_level:
                Logger.logStdout.info("loading target-level "+l_val+ " ......")
                qssi_cmds.append(DataParsingHelper(path, None, l_val, img_to_img=True))
    return qssi_cmds

def merge_qssi_cmds(qssi_cmds):
    m_qssi_cmds = None
    for qssi_cmd in qssi_cmds:
        if m_qssi_cmds == None:
            m_qssi_cmds = qssi_cmd.get_cmd_data()
        else:
            m_qssi_cmds['compatibility-matrix']['hal'].extend(qssi_cmd.get_cmd_data()['compatibility-matrix']['hal'])
    return m_qssi_cmds

def extract_ver_from_fq(hal):
    versions = []
    if not isinstance(hal['fqname'], list):
        return hal['fqname'][1:4]
    else:
        for fq in hal['fqname']:
            ver = fq[1:4]
            if ver not in versions:
                versions.append(ver)
        if len(versions) == 1:
            return versions[0]
    return versions

def get_intf_and_inst(fqname):
    half = fqname.split('::')
    intf_and_inst = half[1].split('/')
    return intf_and_inst

def extract_intf_from_fq(hal):
    intf_list = []
    interfaces = {}
    if not isinstance(hal['fqname'], list):
        intf_and_inst = get_intf_and_inst(hal['fqname'])
        interfaces['name'] = intf_and_inst[0]
        interfaces['instance'] = intf_and_inst[1]
        return interfaces
    else:
        for fq in hal['fqname']:
            intf_and_inst = get_intf_and_inst(fq)
            interfaces['name'] = intf_and_inst[0]
            interfaces['instance'] = intf_and_inst[1]
            intf_list.append(interfaces)
    return intf_list

def img_to_img_intf_check(qssi_cmd, vndr_hal, from_fqname=False):
    is_i_good,intf_res = False, None
    for fcm in qssi_cmd['compatibility-matrix']['hal']:
        if fcm['name'] == vndr_hal['name']:
            intf_res = is_valid_intfs(fcm['interface'], vndr_hal['interface'], vndr_hal['name'])
            if intf_res.err_code == Constants.HIDL_INTF_NO_ERR:
                is_i_good = True
                break
    if not is_i_good:
        if from_fqname:
            UtilFunctions.print_violators(intf_res.intf_name, "INTF(S): "+ str(vndr_hal['fqname'])+ "\nOne or more of the interfaces for the above hal have no client support","", "")
        else:
            UtilFunctions.print_violators(vndr_hal['name'], "INTF(S): "+ str(vndr_hal['interface'])+ "\nOne or more of the interfaces for the above hal have no client support","", "")

def img_to_img_version_check(qssi_cmd, vndr_hal, from_fqname=False, vndr_cmd_ver=""):
    #only called if version tag is in the hal or if version is
    #extracted from fqname tag
    is_v_good,ver_res = False,None
    for fcm in qssi_cmd['compatibility-matrix']['hal']:
        if fcm['name'] == vndr_hal['name'] and fcm['@format'] == "hidl":
            try:
                ver_res = is_hal_version_in_cm(fcm['version'], vndr_hal['version'], vndr_hal['name'])
            except KeyError as err:
                if "version" not in fcm.keys():
                    Logger.logStdout.error(fcm)
                    UtilFunctions.print_violators(vndr_hal['name'], "UNKNOWN HAL: The above hal has no information in compatibility matrix.","","")
                elif "version" not in vndr_hal.keys():
                    Logger.logStdout.error(vndr_hal)
                    UtilFunctions.print_violators(vndr_hal['name'], "UNKNOWN HAL: The above hal has no registered client.","","")
            if ver_res.err_code == Constants.HIDL_VERSION_NO_ERR:
                is_v_good = True
                break
    if not is_v_good:
        if ver_res == None:
            #this would mean that we never even found the interface within the client list
            if vndr_cmd_ver == "":
                Logger.logStdout.warning("UNKNOWN HAL: " + vndr_hal['name'])
            else:
                UtilFunctions.print_violators(vndr_hal['name'], "UNKNOWN HAL: The above hal has no registered client.","","")
            return None #this will let the caller know that the interface is uknown and don't bother with extra check
        else:
            if not from_fqname:
                UtilFunctions.print_violators(ver_res.intf_name, "The above hal has no client support for one or more of the listed version(s).","", str(vndr_hal['version']))
            else:
                UtilFunctions.print_violators(ver_res.intf_name, "No client support found for one or more of the listed version(s). Coming from "+ str(vndr_hal['fqname']),"", str(vndr_hal['version']))
    return is_v_good #let the caller know that the interface is known.

def img_to_img_fqname_check(qssi_cmd, vndr_hal):
    hal = vndr_hal.copy() #we might need to modify it
    #extract all the versions of fqname
    fq_versions = extract_ver_from_fq(hal)
    hal['version'] = fq_versions
    res = img_to_img_version_check(qssi_cmd, hal, from_fqname=True)
    if not res:
        return False #let the caller know to stop any further check on this hal

    fq_intfs = extract_intf_from_fq(hal)
    hal['interface'] = fq_intfs
    img_to_img_intf_check(qssi_cmd, hal, True)

def img_to_img_check(qssi_cmd, vndr_cmd, vndr_cmd_ver=""):
    for hal in vndr_cmd['manifest']['hal']:
        #the hals could have a mix of info we can use
        # 1. version, interface fqname info
        # 2. just version and interface, no fqname
        # 3. just fqname and no version..extract version from fqname
        # if none are present, it is an issue
        if hal['provider'] == 'framework':
            continue

        legacy_v_ex, legacy_fq_ex, legacy_intf_ex = is_hal_exemptible(hal['name'], vndr_cmd_ver)

        if '@format' in hal and hal['@format'] == "aidl":
            Logger.logStdout.warning("aidl interface found: " + hal['name'])
            continue

        something_was_checked = False
        if 'version' in hal and not legacy_v_ex:
            something_was_checked = True
            #time to do version check for hals that have version tag
            known_hal = img_to_img_version_check(qssi_cmd, hal,vndr_cmd_ver=vndr_cmd_ver)
            if known_hal == None:
                #if this is the case, then the violation is already reported in
                #img_to_img_version_check routine so lets move on to other hals
                continue
        #lets do fqname check next, so that if the hal is unkown we don't waste time
        #with the interface tag check
        if 'fqname' in hal and not legacy_fq_ex:
            something_was_checked = True
            res = img_to_img_fqname_check(qssi_cmd, hal)

        if 'interface' in hal and not legacy_intf_ex:
            something_was_checked = True
            img_to_img_intf_check(qssi_cmd, hal)

        if not something_was_checked:
            if not legacy_v_ex and not legacy_fq_ex and not legacy_intf_ex:
                UtilFunctions.print_violators(hal['name'], "The above hal has no usable information for running compatibility check.","","")

def parse_current_txt_files():
    '''
    Description: This defination is parsing text files
    Type       : Internal defination
    '''
    #TODO: eventually replace parse_txt_files() with this function
    parsed_txt_file_data = []
    for current_txt_path in Constants.qiifa_current_txt_file_paths:
        for root, dirs, files in os.walk(current_txt_path):
            for file in files:
                if file == "current.txt":
                    with open(os.path.join(root,file)) as fb:
                        line = fb.readline()
                        while line:
                            #if it isn't a new line or commented line
                            #it is considered a valid hash entry. subject to
                            #further validation
                            excluded_chars = '{}[]|();'
                            char_found=False
                            for excl_char in excluded_chars:
                                if line.find(excl_char) >= 0:
                                    char_found = True
                                    break
                            if not line.startswith('#') and not line.startswith('//') and not line == '\n' and not char_found:
                                hash_dict = {}
                                #break down the string into hash, version, name, intf
                                #so that there is no processing needed in the later stages
                                #hash format before parsed: "hash name@version::intf"

                                #split to ['hash'] and ['name@version::intf']
                                split_by_space = line.split()

                                #store the ['hash'] into the hash_dict indexed by 'hash'
                                hash_dict['hash'] = split_by_space[0]

                                #split the remainer by '@' to ['name'] and ['version::intf'] then
                                #store the first part in hash_dict indexed by 'name'
                                split_at = split_by_space[1].split('@')
                                hash_dict['name'] = split_at[0]

                                #make sure the ['hash'] is 64 chars long
                                if len(split_by_space[0]) != 64:
                                    print(line)
                                    UtilFunctions.print_violations_on_stdout("HIDL Plugin",split_at[0],parse_current_txt_files.__name__,"Invalid Hash Length")
                                    return []

                                #split the version and intf by '::' to ['version'] and ['intf'] then
                                #store the ['version'] in hash_dict indexed by 'version'

                                split_double_cln = split_at[1].split('::')
                                hash_dict['version'] = split_double_cln[0]

                                if hash_dict['version'].count("-") > 0 or hash_dict['version'].count(".") > 1:
                                    print(line)
                                    UtilFunctions.print_violations_on_stdout("HIDL Plugin",split_at[0],parse_current_txt_files.__name__,"Invalid version Signature")
                                    return []

                                #store the remaineder ['intf'] into hash_dict indexed by 'intf'
                                hash_dict['intf'] = split_double_cln[1].splitlines()[0]

                                #then finally  append hash_dict into the list
                                parsed_txt_file_data.append(hash_dict)
                            line = fb.readline()
                        fb.close()
    return parsed_txt_file_data

def parse_txt_files():
    '''
    Description: This defination is parsing text files
    Type       : Internal defination
    '''
    for current_txt_path in Constants.qiifa_current_txt_file_paths:
        for root, dirs, files in os.walk(current_txt_path):
            for file in files:
                if file == "current.txt":
                    with open(os.path.join(root,file)) as fb:
                        line = fb.readline()
                        while line:
                            if not line.startswith('#') and not line == '\n':
                                line_split = line.split()
                                if not len(line_split[0]) == 64:
                                    break
                                name = line_split[1].split('@')
                                intf_name = name[1].split('::')
                                intf_name_is = intf_name[1].splitlines()
                                new_dict = {}
                                new_dict['name'] = intf_name_is[0]
                                new_dict['hash'] = line_split[0]
                                new_dict['version'] = intf_name[0]
                                #make sure the ['hash'] is 64 chars long
                                if not len(line_split[0]) == 64:
                                    UtilFunctions.print_violations_on_stdout("HIDL Plugin",intf_name[0],parse_txt_files.__name__,"Invalid Hash Length",False)
                                    return
                                insert_hash(name[0], new_dict, intf_name[0])
                            line = fb.readline()
                        fb.close()

def append_need_info(parsed_xml_data):
    '''
    Description: This defination is appending need info in to dictionary
    Type       : Internal defination
    '''
    global need_data
    if len(need_data) == 0:
        need_data = parsed_xml_data
        return

    insert_need(parsed_xml_data)

def append_provide_info(parsed_xml_data, provider):
    '''
    Description: This defination appends provide info in to dictionary
    Type       : Internal defination
    '''
    global provide_data
    if len(provide_data) == 0:
        provide_data = insert_provider_attr(parsed_xml_data, provider)
        return
    insert_provide(insert_provider_attr(parsed_xml_data, provider))

def insert_provider_attr(parsed_xml_data, provider):
    '''
    Description: This defination inserts provide attributes
    Type       : Internal defination
    '''
    if not isinstance(parsed_xml_data['manifest']['hal'], list):
        if isinstance(parsed_xml_data['manifest']['hal'], dict):
            parsed_xml_data['manifest']['hal']['provider'] = provider
        return parsed_xml_data
    for x in parsed_xml_data['manifest']['hal']:
        if isinstance(x, dict) and not 'provider' in x:
            x['provider'] = provider
    return parsed_xml_data

def run_qssi_only_checker(golden_dict):
    '''
    Description: This defination runs qssi ONLY HIDL checker
    Type       : Internal defination
    '''
    Logger.logInternal.info("Running a QSSI only hidl compatibility check")
    #pr-parse txt files to avoid parsing at every stage
    txt_file_data = parse_current_txt_files()
    if txt_file_data == None:
        return
    if len(txt_file_data) == 0:
        Logger.logStdout.error("error while parsing txt files")
    parse_xml_files_for_check(txt_file_data, Constants.paths_of_xml_files[0], "qssi")

def run_target_only_checker(golden_dict):
    '''
    Description: This defination runs target only checker
    Type       : Internal defination
    '''
    Logger.logInternal.info("Running a Target only hidl compatibility check")
    parse_xml_files_for_check(None, Constants.paths_of_xml_files[1], "target")

def run_full_blown_checker(golden_dict):
    '''
    Description: This defination runs full blow (qssi + target) checker
    Type       : Internal defination
    '''
    Logger.logInternal.info("Running a full blown hidl compatibility check")
    parse_xml_files()
    parse_txt_files()
    dump_to_temp_json_file()
    hash_check(golden_dict)
    full_version_check(golden_dict)

def run_img_to_img_checker(qssi_path,
                           target_path,
                           q_file_name,
                           t_file_name):
    '''
    Description: This defination runs image to image checker
    Type       : Internal defination
    '''
    Logger.logInternal.info("Running image to image compatibility check")
    qssi_json_cmd_path =''
    target_json_cmd_path = ''

    if qssi_path.endswith('/'):
        qssi_json_cmd_path = qssi_path + q_file_name[:-3]
    else:
        qssi_json_cmd_path = qssi_path +"/"+q_file_name[:-3]

    if target_path.endswith('/'):
        target_json_cmd_path = target_path + t_file_name[:-3]
    else:
        target_json_cmd_path = target_path + "/" + t_file_name[:-3]

    if UtilFunctions.dirExists(qssi_json_cmd_path) and UtilFunctions.dirExists(target_json_cmd_path):
        #load the cmd that was exported from the vendor half
        vndr_cmd_ver = get_vndr_cmd_ver(target_json_cmd_path)
        vndr_cmd = DataParsingHelper(target_json_cmd_path, None, vndr_cmd_ver, img_to_img=True)
        #load all the cmds that were exported form the qssi half
        target_level = get_target_level(vndr_cmd.get_cmd_data())
        qssi_cmds = load_qssi_cmds(qssi_json_cmd_path, target_level)

        if qssi_cmds == None or vndr_cmd == None:
            Logger.logStdout.error("Either target-CMD or QSSI-CMD is empty")
            sys.exit(-1)
        else:
            #let the check begin
            m_qssi_cmds = merge_qssi_cmds(qssi_cmds)
            img_to_img_check(m_qssi_cmds, vndr_cmd.get_cmd_data(), vndr_cmd_ver)
    else:
        Logger.logStdout.error(qssi_json_cmd_path + " or " + target_json_cmd_path + " doesn't exist.")
        sys.exit()

def main(flag):
    '''
    Description: Main defination
    Type       : Internal defination
    '''
    append_f_provider()
    merge_dev_fw_need()
    insert_fw_dev_need()
    parse_txt_file()
    if flag == "check":
        Logger.logInternal.info("Running qiifa hidl compatibility check...")
        hash_check()
        version_check()
        Logger.logInternal.info ("Success")
    elif flag == "golden":
        Logger.logInternal.info("Running qiifa golden db generator...")
        dump_to_json_file()
        Logger.logInternal.info ("Success")

def hidl_checker_main(self,
                      flag,
                      qssi_path=None,
                      target_path=None,
                      q_file_name=None,
                      t_file_name=None):
    '''
    Description: This defination is main function for hidl checker
    Type       : External defination
    '''
    if flag == "check":
        if qssi_path != None and target_path != None:
            run_img_to_img_checker(qssi_path, target_path, q_file_name, t_file_name)
        else:
            if (UtilFunctions.pathExists(Constants.qiifa_hidl_db_path) == False):
                Logger.logStdout.error("JSON file is not found at %s", Constants.qiifa_hidl_db_path)
                Logger.logInternal.error("JSON file is not found at %s", Constants.qiifa_hidl_db_path)
                sys.exit()
            golden_dict = json.load(open(Constants.qiifa_hidl_db_path,"r"))
            if not Constants.target_product == "qssi":
                #if UtilFunctions.dirExists(Constants.paths_of_xml_files[0]):
                #    Logger.logInternal.info("Skipping a full blown hidl compatibility check..")
                #    #run_full_blown_checker(golden_dict)
                #else:
                run_target_only_checker(golden_dict)
                dump_to_temp_json_file(Constants.qiifa_out_temp_cmd_2_0)
            else:
                run_qssi_only_checker(golden_dict)
                copy_qssi_artifacts()
    else:
        Logger.logStdout.info("Unexpected hidl checker flag!")
        sys.exit(1)

def hidl_checker_main_create(self,
                             flag,
                             arg_create_type,
                             arg_intf_name=None):
    Logger.logInternal.info("Running qiifa golden db generator...")
    ''' rarely the case'''
    if not flag == "golden":
        Logger.logStdout.info("Unexpected create flag!")
        sys.exit(1)
    #pre-parse the txt files to avoid parsing them for every
    #xml file that is parsed in the subsequent methods
    txt_file_data = parse_current_txt_files()
    if len(txt_file_data) == 0:
        Logger.logStdout.error("error while parsing txt files")

    #the case where user wants to generate the full cmd
    #not recommended :)
    if arg_intf_name == None and arg_create_type == Constants.HIDL_SUPPORTED_CREATE_ARGS[0]:
        Logger.logStdout.info("Running full hidl cmd generation..")
        Logger.logStdout.warning("Not a recommended operation..\n")
        parse_xml_files_for_create(Constants.paths_of_xml_files[0], txt_file_data, "all")
    #at this point it must be a single intf cmd generation
    else:
        Logger.logStdout.info("Running hidl cmd generation with option: " + arg_create_type +", intf name: " + arg_intf_name)
        parse_xml_files_for_create(Constants.paths_of_xml_files[0], txt_file_data, "single", arg_intf_name)
    Logger.logInternal.info ("Success")

'''
plugin class implementation
plugin class is derived from plugin_interface class
Name of plugin class MUST be MyPlugin
'''
class MyPlugin(plugin_interface):
    def __init__(self):
        pass

    def register(self):
        return Constants.HIDL_SUPPORTED_CREATE_ARGS

    def config(self, QIIFA_type=None, libsPath=None, CMDPath=None):
        pass

    def generateGoldenCMD(self, libsPath=None, storagePath=None, create_args_lst=None):
        '''
        the assumption here is that if create_args_lst is empty, then this was called under the
        circumstance where --create arg was called with "all" option; so it should behave as if
        --create was called with "hidl" option.
        '''
        if create_args_lst is None:
            hidl_checker_main_create(self, "golden", Constants.HIDL_SUPPORTED_CREATE_ARGS[0])
        #Same behavior as above. Ignore everything in create_args_lst but the first one.
        elif create_args_lst[0] == Constants.HIDL_SUPPORTED_CREATE_ARGS[0]:
            hidl_checker_main_create(self, "golden", Constants.HIDL_SUPPORTED_CREATE_ARGS[0])
        #In this case create_args_lst[1] will have the particular intf name
        elif create_args_lst[0] in Constants.HIDL_SUPPORTED_CREATE_ARGS and len(create_args_lst)==2:
            hidl_checker_main_create(self, "golden", create_args_lst[0], create_args_lst[1])
        else:
            Logger.logStdout.info("Invalid --create argument options")
            Logger.logStdout.info("python qiifa_main.py -h")
            sys.exit()

    def IIC(self, **kwargs):
        if kwargs:
            kwargs.update({"flag":"check"})
            hidl_checker_main(self, **kwargs)
        else:
            hidl_checker_main(self, "check")

class ErrorInfo:
    def __init__(self, intf_name, err_code):
        self.intf_name = intf_name
        self.err_code = err_code

class VersionErrorInfo(ErrorInfo):
    def __init__(self, golden_ver, current_ver, intf_name, err_code):
        ErrorInfo.__init__(self,intf_name, err_code)
        self.golden_ver = golden_ver
        self.current_ver = current_ver

class HashErrorInfo(ErrorInfo):
    def __init__(self, gldn_hash, crrnt_hash, intf_name, err_code):
        ErrorInfo.__init__(self, intf_name, err_code)
        self.gldn_hash = gldn_hash
        self.crrnt_hash = crrnt_hash

class IntfErrorInfo(ErrorInfo):
    def __init__(self, gldn_intf, crrnt_intf, hal_name, err_code):
        ErrorInfo.__init__(self, hal_name, err_code)
        self.gldn_intf = gldn_intf
        self.crrnt_intf = crrnt_intf

class DataParsingHelper:
    def __init__(self, root, xml_file, level_val, hash_data=None,img_to_img=False):
        self.__root = root
        self.__xml_file = xml_file
        self.__level_val = level_val
        self.__hash_data = hash_data
        self.__img_to_img = img_to_img

    def get_level_val(self):
        return self.__level_val

    def get_cmd_name(self):
        if self.__level_val != "":
            return Constants.qiifa_cmd_prefix + '_' + self.__level_val + Constants.json_file_suffix
        return Constants.qiifa_cmd_prefix + self.__level_val + Constants.json_file_suffix

    def get_cmd_path(self):
        if self.__img_to_img:
            return os.path.join(self.__root, self.get_cmd_name())
        return Constants.qiifa_hidl_db_root + self.get_cmd_name()

    def get_xml_file_name(self):
        if self.__img_to_img:
            return None
        return self.__xml_file

    def get_xml_full_path(self):
        if self.__img_to_img:
            return None
        return os.path.join(self.__root, self.get_xml_file_name())

    def get_raw_xml_data(self):
        if self.__img_to_img:
            return None
        fd = open(self.get_xml_full_path())
        fcm_data = xmltodict.parse(fd.read())
        trim_need_data(fcm_data)
        return fcm_data

    def append_hashes(self, txt_file_data=None):
        if self.__img_to_img:
            return None
        raw_xml_data = self.get_raw_xml_data()
        append_hashes(txt_file_data, raw_xml_data, is_for_check=True)
        return raw_xml_data

    def get_cmd_data(self):
        cmd_file_path = self.get_cmd_path()
        #highly unlikely scenario, but just in case
        if not UtilFunctions.fileExists(cmd_file_path):
            Logger.logStdout.error("The cmd file " + self.get_cmd_name() + " doesn't exist. Make sure it exists before proceeding.")
            sys.exit()
        return json.load(open(cmd_file_path,"r"), object_pairs_hook=OrderedDict)

'''It can also be run as an independent script'''
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='QIIFA hidl compatibility script')
    parser.add_argument('--g',type=str,required=False,choices=["golden","check"],help='generate golden db or run check')
    args = parser.parse_args()
    hidl_checker_main(qiifa_hidl_checker(),args.g)
