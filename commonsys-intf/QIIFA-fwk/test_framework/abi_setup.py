#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class will setup the abi plugin to be able to run for test_framework.

Can run this file after a new libTestCase is added and will produce
golden lsdump for the new test case.

"""

import json
import sys
import os

from utils import run_cmd, copy_file


class ABISetup(object):

    _tmp_dir = '/var/tmp'
    _abi_tc_libs_name = 'abi_tc_libs'
    _ref_libtestcase_name = 'libTestCase'
    _unittest_conf_name = 'qiifa_unittest_config.json'

    _so_files_path = 'out/target/product/qssi/vendor/lib64'
    _qiifa_main_path = 'vendor/qcom/proprietary/commonsys-intf/QIIFA-fwk'
    _abi_config_path = os.path.join(_qiifa_main_path,
                                    'plugins/qiifa_abi_checker/abi_config.json')
    _abi_tc_libs_path = os.path.join(_qiifa_main_path,
                                     'test_framework/abi_tc_libs')
    _ref_libtestcase_path = os.path.join(_abi_tc_libs_path,
                                         _ref_libtestcase_name)
    _golden_dir = 'vendor/qcom/proprietary/commonsys-intf/QIIFA-cmd/' \
                  'abi-dumps/unittest'

    def __init__(self, build_path):
        self._build_path = build_path
        self._file_path = os.path.abspath(os.path.dirname(__file__))

    def _abs_path(self, path):
        """Add the path to croot to the given path."""

        abs_path = os.path.join(self._build_path, path)
        return abs_path

    def main(self):
        """Runs the various parts of the abi setup."""
        self.add_unittest_config_to_abi_config()
        if not self.is_updated():
            self.copy_abi_tc_libs_to_tmp()
            self.create_ref_copies()
            edited = self.run_mm()
            if edited:
                self.compile_from_croot()
            self.create_abi()
            self.restore_original_abi_tc_libs()

    def is_updated(self):
        """Determines if the setup should be run."""

        golden_dirs = []

        # Walk through the unittest golden dir and find the golden lsdumps libs
        level = 0
        for _, dirs, _ in os.walk(self._abs_path(self._golden_dir)):
            if level == 2:
                golden_dirs = set(dirs)
                break
            level += 1

        # This will be true during very first abi setup
        if level != 2:
            return False

        # Iterates through the tc dirs and checks if golden copies exist
        for _, dirs, _ in os.walk(self._abs_path(self._abi_tc_libs_path),
                                  topdown=False):
            for tc_dir in dirs:
                if tc_dir not in golden_dirs:
                    return False
        return True

    def copy_abi_tc_libs_to_tmp(self):
        """Create a copy of abi_tc_libs within the _tmp_dir."""

        abi_tc_libs_path = self._abs_path(self._abi_tc_libs_path)
        cmd_lst = ['rm -r {}/{}'.format(self._tmp_dir, self._abi_tc_libs_name),
                   'cp -r {} {}'.format(abi_tc_libs_path, self._tmp_dir)]
        run_cmd(cmd_lst)

    def create_ref_copies(self):
        """Reset all libTestCase libs to the contents of the
        golden reference.
        """

        cp_files = []
        ref_tc_abs_path = self._abs_path(self._ref_libtestcase_path)
        for root, dirs, files in os.walk(ref_tc_abs_path, topdown=False):
            files.remove('Android.mk')
            cp_files = files

        for root, dirs, files in os.walk(self._abs_path(self._abi_tc_libs_path),
                                         topdown=False):
            for tc_dir in dirs:
                for cp_file in cp_files:
                    f_path = os.path.join(ref_tc_abs_path, cp_file)
                    dir_abs_path = os.path.join(root, tc_dir)
                    copy_file(f_path, dir_abs_path)

    def run_mm(self):
        """Run mm on the libTestCase libs that don't have corresponding .so
        files and add them to qiifa_unittest_config.json.
        """

        edited = False
        so_file_dirs = set()
        for _, _, files in os.walk(self._abs_path(self._so_files_path),
                                   topdown=False):
            for so_file in files:
                so_file_dirs.add(so_file[:-3])

        for root, dirs, _ in os.walk(self._abs_path(self._abi_tc_libs_path),
                                     topdown=False):
            for tc_dir in dirs:
                if tc_dir not in so_file_dirs:
                    tc_dir_abs_path = os.path.join(root, tc_dir)
                    cmd_lst = ['cd {}'.format(self._build_path),
                               'source build/envsetup.sh',
                               'lunch qssi-userdebug',
                               'cd {}'.format(tc_dir_abs_path),
                               'mm']
                    print(run_cmd(cmd_lst))
                conf_edited = self.add_so_to_unittest_config(tc_dir)
                edited = edited or conf_edited

        # Copy the edited version of the unittest_config to tmp_dir
        self.copy_unittest_config_to_tmp()

        # Create the golden unittest directory if it doesn't already exist
        unittest_edited = self.create_unittest_dir()
        return edited or unittest_edited

    def copy_unittest_config_to_tmp(self):
        """Copy the edited version of unittest_config to the copy of
        abi_tc_libs within _tmp_dir.
        """

        test_conf_path = self._abs_path(os.path.join(self._abi_tc_libs_path,
                                                     self._unittest_conf_name))
        abi_tc_tmp_path = os.path.join(self._tmp_dir,
                                       self._abi_tc_libs_name)
        cmd_lst = ['cp {} {}'.format(test_conf_path, abi_tc_tmp_path)]
        run_cmd(cmd_lst)

    def create_unittest_dir(self):
        """Create the unittest dir for the golden lsdumps."""

        golden_dir_abs_path = os.path.join(self._build_path, self._golden_dir)
        try:
            os.mkdir(golden_dir_abs_path)
        except OSError:
            return False
        return True

    def add_unittest_config_to_abi_config(self):
        """Add the the qiifa_unittest_config.json path to
        techpackages_json_file_list.
        """

        with open(self._abs_path(self._abi_config_path), 'r') as fp:
            abi_config = json.load(fp)
            test_conf_path = os.path.join(self._abi_tc_libs_path,
                                          self._unittest_conf_name)
            for d in abi_config:
                key_str = 'techpacakages_json_file_list'
                if key_str in d:
                    if test_conf_path not in d[key_str]:
                        d[key_str].append(test_conf_path)
                    d['enabled'] = "true"

        with open(self._abs_path(self._abi_config_path), 'w') as fp:
            json.dump(abi_config, fp, indent=2)

    def add_so_to_unittest_config(self, tc_dir):
        """Given test case directory, add the .so file attached
        to the dir to qiifa_unittest_config.json.
        """

        test_conf_path = self._abs_path(os.path.join(self._abi_tc_libs_path,
                                                     self._unittest_conf_name))
        with open(test_conf_path, 'r') as fp:
            test_config = json.load(fp)
            lib_lst = test_config['library_list']
            entry = '/vendor/lib64/{}.so'.format(tc_dir)
            if entry not in lib_lst:
                lib_lst.append(entry)
                lib_lst.sort()  # Sort to keep .so files in named order
            else:
                # If no change to the file, then simply return False
                return False
            test_config['library_list'] = lib_lst

        with open(test_conf_path, 'w') as fp:
            json.dump(test_config, fp, indent=2)
        return True

    def compile_from_croot(self):
        """Compile the build from croot."""

        comp_cmd = 'bash build.sh -j40 dist --qssi_only' \
                   ' EXPERIMENTAL_USE_OPENJDK9=1.8'
        cmd_lst = ['cd {}'.format(self._build_path),
                   'source build/envsetup.sh',
                   'lunch qssi-userdebug',
                   comp_cmd]
        print(run_cmd(cmd_lst))

    def create_abi(self):
        """Run qiifa_main --create to create golden lsdumps."""

        cmd_lst = ['cd {}'.format(self._build_path),
                   'source build/envsetup.sh',
                   'lunch qssi-userdebug',
                   'cd {}'.format(self._abs_path(self._qiifa_main_path)),
                   'python qiifa_main.py --create abi']
        print(run_cmd(cmd_lst))

    def restore_original_abi_tc_libs(self):
        """Restore the original version of abi_tc_libs from within
        _tmp_dir to within test_framework.
        """

        abi_tc_libs_abs_path = self._abs_path(self._abi_tc_libs_path)
        cmd_lst = ['rm -r "{}"'.format(abi_tc_libs_abs_path),
                   'cp -r "{}/{}" "{}"'.format(self._tmp_dir,
                                               self._abi_tc_libs_name,
                                               self._file_path)]
        run_cmd(cmd_lst)


if __name__ == '__main__':
    if len(sys.argv) == 2:
        LIB_NAME = sys.argv[1]
        setup = ABISetup(LIB_NAME)
        setup.main()
    else:
        print('Error. Need croot path as arg.')
