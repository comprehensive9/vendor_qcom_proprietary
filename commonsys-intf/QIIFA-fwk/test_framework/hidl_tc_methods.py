#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.


"""This class methods that edit hidl files for specific test cases.

IMPORTANT:
 - The sys.path should contain path to
   vendor/qcom/proprietary/commonsys-intf/QIIFA-fwk/ dir to use xmltodict
 - This class must contain an implementation of all the tc_method
   variables found within test_cases.json.
 - tc_kwargs should be the only normal parameter when defining a
   method within this class.
 - Methods can have params that contain default values.
 - Any params that are used should be placed within tc_method_kwargs
   within test_cases.json and retrieved within a try catch statement
   from the tc_kwargs dictionary.
 - The abs_file_path key is added to all tc_kwargs dicts within
   hidl_compatibility.py.
 - The abs_file_path key is the file_path variable within test_cases.json

"""

import sys
import os
import re
sys.path.append('../')
from collections import OrderedDict

import plugins.qiifa_hidl_checker.xmltodict as xmltodict
from utils import ver_lst_to_dict, ver_dict_to_lst


class HIDLTCMethods(object):
    def __init__(self):
        pass

    ########################## helper functions ###############################
    def check_file_alter(self, altered, msg):
        # Raise an exception if method doesn't edit file
        if not altered:
            raise Exception(msg)

    def tc_kwargs_error(self, msg):
        raise Exception(msg)

    def write_xml_file(self, abs_file_path, xml):
        # Write the edited contents of the file
        with open(abs_file_path, 'w') as fp:
            xmltodict.unparse(xml, fp)

    def read_xml_from_file(self, abs_file_path, xml):
        if not xml:
            with open(abs_file_path, 'r') as fp:
                xml = xmltodict.parse(fp)
        return xml

    def write_txt_file(self, abs_file_path, contents):
        with open(abs_file_path, 'w') as fp:
            fp.writelines(contents)

    def find_interface_pattern(self, line):
        ''' helper function to find hal format in current.txt '''
        pattern = r" [a-zA-z]*.[a-zA-z]*.*@\d*.\d*::[a-zA-z0-9]*"
        text = re.findall(pattern, line)[0]
        folder = ('/'.join(str(text).split('.')[2:])).split('@')[0] + '/'
        version = (re.findall(r"\d*.\d*:", line)[0])[:-1]
        hal = text.split('::')[-1]
        search_hal = folder + version + '/' + hal + '.hal'
        return search_hal

    def check_hal_is_present(self, search_hal):
        ''' helper function to check if the file is present '''
        if os.path.exists(search_hal):
            return True
        return False

    ############################# main code ###################################
    def do_nothing(self, tc_kwargs):
        """Don't alter any files."""
        pass

    # HASH TEST CASE METHODS
    def modify_hash(self, tc_kwargs, altered=False):
        """Modify the hash of the given signature within current.txt by adding one to the value."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            intf_name = tc_kwargs['intf_name']
            hal_name = tc_kwargs['hal_name']
            version = tc_kwargs['version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, intf_name, hal_name, or version.')

        # The hash of this signature will be changed
        signature = hal_name + '@' + version + '::' + intf_name

        # Read and edit contents of file
        contents = []
        with open(abs_file_path, 'r') as fp:
            for line in fp:
                line_lst = line.split(' ')

                # Only look at lines with hash and signature
                if len(line_lst) == 2:
                    hsh = line_lst[0]
                    sig = line_lst[1].strip()
                    if signature == sig:
                        altered = True
                        hsh_in_dec = int(hsh, 16)

                        # Add one to the old hash in decimal
                        new_hsh = hex(hsh_in_dec + 1)[2:-1]
                        line_lst[0] = new_hsh
                        edited_line = ' '.join(line_lst)
                        contents.append(edited_line)
                        continue

                contents.append(line)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='Signature not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_txt_file(abs_file_path, contents)

    def modify_signature_version(self, tc_kwargs, altered=False):
        """Modify the version of the given signature within current.txt by newer version."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            intf_name = tc_kwargs['intf_name']
            hal_name = tc_kwargs['hal_name']
            version = tc_kwargs['prev_version']
            new_version = tc_kwargs['new_version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, intf_name, hal_name, or version.')

        # The hash of this signature will be changed
        signature = hal_name + '@' + version + '::' + intf_name

        # Read and edit contents of file
        contents = []
        with open(abs_file_path, 'r') as fp:
            for line in fp:
                line_lst = line.split(' ')

                # Only look at lines with hash and signature
                if len(line_lst) == 2:
                    hsh = line_lst[0]
                    sig = line_lst[1].strip()
                    if signature == sig:
                        altered = True
                        hsh_in_dec = int(hsh, 16)

                        # Add new version instead of the old version
                        new_sig = hal_name + '@' + new_version + '::' + intf_name
                        line_lst[1] = new_sig
                        edited_line = ' '.join(line_lst)
                        contents.append(edited_line)
                        continue

                contents.append(line)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered,msg='Signature not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_txt_file(abs_file_path, contents)

    def delete_hash(self, tc_kwargs, altered=False):
        """Delete the hash of the given signature within current.txt."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            intf_name = tc_kwargs['intf_name']
            hal_name = tc_kwargs['hal_name']
            version = tc_kwargs['version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, intf_name, hal_name, or version.')

        # The hash of this signature will be changed
        signature = hal_name + '@' + version + '::' + intf_name

        # Read and edit contents of file
        contents = []
        with open(abs_file_path, 'r') as fp:
            for line in fp:
                line_lst = line.split(' ')

                # Only look at lines that have hash and signature
                if len(line_lst) == 2:
                    hsh = line_lst[0]
                    sig = line_lst[1].strip()

                    # If the signature is found, don't add it to contents list
                    if signature == sig:
                        altered = True
                        continue
                contents.append(line)

        # Raise an exception if method doesn't edit file
        if not altered:
            raise Exception('Signature not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_txt_file(abs_file_path, contents)

    def add_dummy_hash(self, tc_kwargs):
        """Add a default dummy hash to the end of current.txt."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, intf_name, hal_name, or version.')

        # Read and edit contents of file
        with open(abs_file_path, 'r') as fp:
            contents = fp.readlines()

        # Add the new dummy line
        new_line = '0' * 64 + ' ' + 'android.dummy.testdummy@1.0::dummy'
        contents.append(new_line)

        # Write the edited contents of the file
        self.write_txt_file(abs_file_path, contents)

    # XML VERSION TEST CASE METHODS
    def add_hidl_version(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and adds a hidl verision to a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            new_version = tc_kwargs['new_version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, intf_name, or new_intf_name.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'version' in item and item['name'] == hal_name:
                altered = True

                # Get version. If version is not a list, make it into a list
                version = item['version']
                if not isinstance(version, list):
                    version = [version]

                # Add the new version
                new_ver_dict = ver_lst_to_dict([new_version])
                ver_dict = ver_lst_to_dict(version)
                for major, minor_lst in new_ver_dict.items():
                    for minor in minor_lst:
                        if major in ver_dict:
                            ver_dict[major].append(int(minor))
                        else:
                            ver_dict[major] = [int(minor)]

                # Set the version list to the edited version list
                ver_lst = ver_dict_to_lst(ver_dict)
                item['version'] = ver_lst

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def remove_hidl_version(self, tc_kwargs, xml=None, altered=False):
        """traverses the xml dictionary and removes a hidl verision from a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            remove_version = tc_kwargs['remove_version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name or remove_version.')

        # read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'version' in item and item['name'] == hal_name:
                altered = True

                # get version. if version is not a list, make it into a list
                version = item['version']
                if not isinstance(version, list):
                    version = [version]

                # remove the version
                remove_ver_dict = ver_lst_to_dict([remove_version])
                ver_dict = ver_lst_to_dict(version)
                try:
                    for major, minor_lst in remove_ver_dict.items():
                        for minor in minor_lst:
                            ver_dict[major].remove(int(minor))
                except (ValueError, KeyError):
                    self.tc_kwargs_error(msg='the given remove_version value does not exist within the current versions of ' + hal_name)

                # Set the version list to the edited version list
                ver_lst = ver_dict_to_lst(ver_dict)
                item['version'] = ver_lst

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='Signature not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_hidl_version(self, tc_kwargs, xml=None):
        """Replaces a previous version with a modifed version for a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            prev_version = tc_kwargs['prev_version']
            mod_version = tc_kwargs['mod_version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, prev_version, or mod_version.')

        # Create remove_tc_kwargs dict and then call remove hidl version method
        remove_tc_kwargs = {'remove_version': prev_version,
                            'hal_name': hal_name,
                            'abs_file_path': abs_file_path}
        xml = self.remove_hidl_version(remove_tc_kwargs)

        # Create add_tc_kwargs dict and then call add hidl version method
        add_tc_kwargs = {'new_version': mod_version,
                         'hal_name': hal_name,
                         'abs_file_path': abs_file_path}
        xml = self.add_hidl_version(add_tc_kwargs, xml)
        return xml

    # XML INTERFACE TEST CASE METHODS
    def remove_hidl_intf(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes a hidl interface from a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            remove_intf = tc_kwargs['remove_intf']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, or remove_intf.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'interface' in item and item['name'] == hal_name:

                # Get interface. If interface is not list, make it into a list
                interface = item['interface']
                if not isinstance(interface, list):
                    interface = [interface]

                # Iterate through interfaces and look for interface to remove
                for j, intf in enumerate(interface):
                    intf_name = intf['name']
                    if intf_name == remove_intf:
                        try:
                            interface.pop(j)
                            altered = True
                        except ValueError:
                            self.check_file_alter(altered, msg='he given remove_intf value does not exist within current intfs of ' + hal_name)
                        break
                item['interface'] = interface

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_hidl_intf(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes a hidl interface from a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            prev_intf = tc_kwargs['prev_intf']
            mod_intf = tc_kwargs['mod_intf']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, prev_intf, or mod_intf.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'interface' in item and item['name'] == hal_name:

                # Get interface. If interface is not list, make into a list
                interface = item['interface']
                if not isinstance(interface, list):
                    item['interface'] = [interface]

                # Iterate through interfaces and look for interface to remove
                for j, intf in enumerate(item['interface']):
                    intf_name = intf['name']
                    if intf_name == prev_intf:
                        intf['name'] = mod_intf
                        altered = True
                        break

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name or prev_intf not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def add_hidl_intf(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and adds a hidl interface to a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            new_intf = tc_kwargs['new_intf']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, or new_intf.')


        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'interface' in item and item['name'] == hal_name:
                altered = True

                # Get interface. If interface is not a list, make into a list
                interface = item['interface']
                if not isinstance(interface, list):
                    item['interface'] = [interface]

                # Adds the new interface with a default instance
                new_intf_tag = OrderedDict([(u'name', new_intf),
                                            (u'instance', u'default')])
                item['interface'].append(new_intf_tag)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    # XML INTERFACE INSTANCE TEST CASE METHODS
    def add_intf_instance(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and adds a hidl interface instance to a hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            intf_name = tc_kwargs['intf_name']
            new_instance = tc_kwargs['new_instance']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, intf_name, or new_instance.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'interface' in item and item['name'] == hal_name:

                # Get interface. If interface is not list, make into a list
                interface = item['interface']
                if not isinstance(interface, list):
                    item['interface'] = [interface]

                # Iterate through intfs and look for intf to add instance to
                for j, intf in enumerate(item['interface']):
                    if intf['name'] == intf_name:
                        altered = True

                        # Get intf. If the intf is not list, make into a list
                        instance = intf['instance']
                        if not isinstance(instance, list):
                            intf['instance'] = [instance]
                        intf['instance'].append(new_instance)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name or intf_name not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def remove_intf_instance(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes a hidl interface instance from a hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            intf_name = tc_kwargs['intf_name']
            remove_instance = tc_kwargs['remove_instance']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, intf_name, or remove_instance.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if 'name' in item and 'interface' in item and item['name'] == hal_name:

                # Get interface. If interface is not list, make into a list
                interface = item['interface']
                if not isinstance(interface, list):
                    item['interface'] = [interface]

                # Iterate through intfs and look for intf to add instance
                for j, intf in enumerate(item['interface']):
                    if intf['name'] == intf_name:

                        # Get interface. If interface is not list, make list
                        instance = intf['instance']
                        if not isinstance(instance, list):
                            intf['instance'] = [instance]

                        try:
                            intf['instance'].remove(remove_instance)
                            altered = True
                        except ValueError:
                            self.check_file_alter(altered, msg='remove_instance value does not exist within the current instances of ' + hal_name)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal_name or intf_name not found. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_intf_instance(self, tc_kwargs, xml=None):
        """Traverses the xml dictionary and modies a hidl interface instance of a given hal."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            intf_name = tc_kwargs['intf_name']
            prev_instance = tc_kwargs['prev_instance']
            mod_instance = tc_kwargs['mod_instance']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, intf_name, prev_instance, or mod_instance.')

        # Create remove_tc_kwargs dict and call remove intf instance method
        remove_tc_kwargs = {'hal_name': hal_name,
                            'intf_name': intf_name,
                            'remove_instance': prev_instance,
                            'abs_file_path': abs_file_path}
        xml = self.remove_intf_instance(remove_tc_kwargs)

        # Create add_tc_kwargs dict and call add hidl interface instance method
        add_tc_kwargs = {'hal_name': hal_name,
                         'intf_name': intf_name,
                         'new_instance': mod_instance,
                         'abs_file_path': abs_file_path}
        xml = self.add_intf_instance(add_tc_kwargs, xml)
        return xml

    def modify_comments(self, tc_kwargs, altered=False):
        """Modify the comment within current.txt by adding test to the comment."""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            comments_name = tc_kwargs['comment']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, comment.')

        # Read and edit contents of file
        contents = []
        with open(abs_file_path, 'r') as fp:
            for line in fp:

                comment = line.strip()

                if comments_name == comment:
                    altered = True

                    # Add test to the end of comment
                    line = comment + " test" + "\n"
                    print(line)
                    contents.append(line)
                    continue

                contents.append(line)

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='Comment not updated. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_txt_file(abs_file_path, contents)

    def modify_compatibility_matrix_hal_format(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and modify compatibility-matrix.hal.format"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            new_format_name = tc_kwargs['new_format']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, new_format.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if ind == 0:
                xml['compatibility-matrix']['hal'][0]['@format'] = new_format_name
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal format not modified. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def remove_compatibility_matrix_hal_format(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes compatibility-matrix.hal.format"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if ind == 0:
                del xml['compatibility-matrix']['hal'][0]['@format']
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal format not removed. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_compatibility_matrix_hal_optional(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and modify compatibility-matrix.hal.optional"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            new_optional_name = tc_kwargs['new_optional']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, new_optional.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if ind == 0:
                xml['compatibility-matrix']['hal'][0]['@optional'] = new_optional_name
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal optional not modified. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def remove_compatibility_matrix_hal_optional(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes compatibility-matrix.hal.optional"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if ind == 0:
                del xml['compatibility-matrix']['hal'][0]['@optional']
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal optional not removed. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def remove_compatibility_matrix_avb_vbmeta_version(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and removes compatibility-matrix.avb.vbmeta_version"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['avb']):
            if ind == 0:
                del xml['compatibility-matrix']['avb']['vbmeta-version']
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal optional not removed. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_compatibility_matrix_hal_name(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and modify compatibility-matrix.hal.name"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if item['name'] == hal_name:
                xml['compatibility-matrix']['hal'][ind]['name'] = '1' + hal_name
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal name not modified. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def modify_hal_version_major_minor_format(self, tc_kwargs, xml=None, altered=False):
        """Traverses the xml dictionary and modify compatibility-matrix.hal.version"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
            # Found within tc_method_kwargs variable within test_cases.json
            hal_name = tc_kwargs['hal_name']
            new_version = tc_kwargs['new_version']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path, hal_name, or new_version.')

        # Read xml from abs_file_path if not passes as argument
        xml = self.read_xml_from_file(abs_file_path, xml)

        # Read and edit contents of file
        for ind, item in enumerate(xml['compatibility-matrix']['hal']):
            if item['name'] == hal_name:
                xml['compatibility-matrix']['hal'][ind]['version'] = new_version
                altered = True

        # Raise an exception if method doesn't edit file
        self.check_file_alter(altered, msg='hal version not modified. File hasn\'t been edited.')

        # Write the edited contents of the file
        self.write_xml_file(abs_file_path, xml)
        return xml

    def check_hal_files_are_present(self, tc_kwargs, xml=None):
        """Traverses the txt file and check whether corresponding hal files are present"""
        try:
            abs_file_path = tc_kwargs['abs_file_path']
        except KeyError:
            self.tc_kwargs_error(msg='tc_kwargs missing abs_file_path.')

        # Setting the variable to hold the path of interfaces directory
        android_build_top = str(os.getenv('ANDROID_BUILD_TOP')) + '/hardware/interfaces/'

        files = []
        with open(abs_file_path, 'r') as fp:
            for line in fp:
                if line.startswith('#') or line.strip() == "":
                    continue
                else:
                    search_hal = self.find_interface_pattern(line.strip())
                    if not self.check_hal_is_present(android_build_top + search_hal):
                        files.append(android_build_top + search_hal)

        if len(files)!=0:
            print("The below hal files are missing:")
            for file in files:
                print(file)
            raise Exception('HAL files Missing')

        return True