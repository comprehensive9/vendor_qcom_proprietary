#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class runs the workflow for abi test cases.

SCRIPT LOGIC:
 - The qiifa_main.py --type abi command is called and the output
   is parsed using regex into a dictionary that
   contains key value pairs of the library(copy of libdummy) name
   and the resulting use case for the library (copy of libdummy).
"""

import re

from compatibility_check import CompatibilityCheck
from utils import run_cmd
from abi_setup import ABISetup


class ABICompatibility(CompatibilityCheck):
    def __init__(self):
        super(ABICompatibility, self).__init__()
        ABISetup(self.build_path).main()
        self._compat_dict = self._parse_type_abi_output()

    def compatibility(self, test_case_kwargs):
        """Determines the compatability of the given test case.

        The compat_dict only contains libraries that are either
        incompatible or extending.

        Compatible libraries are not found within the dictionary.
        """

        try:
            library_name = test_case_kwargs['library_name']
        except KeyError:
            raise Exception('Test case missing library_name.')

        # Return "golden reference dumps not present" if golden reference dumps are not generated
        if 'cannot access' in self._run_check_golden_dumps(library_name):
            return 'golden dumps not present'

        # Return "lsdumps not present" if lsdumps are not generated
        if 'cannot access' in self._run_check_lsdumps(library_name):
            return 'lsdumps not present'

        # Return the compatibility of the library if found within _compat_dict
        if library_name in self._compat_dict:
            return self._compat_dict[library_name]
        return 'compatible'

    def _parse_type_abi_output(self):
        """Parses the output of qiifa_main --type abi using
        regex and creates a dict of the results.
        """

        compat_dict = {}
        output = self._run_type_abi_cmd()

        # Use regex and find lines that contain the library and compatbility
        lib_reports = re.findall('library:.*CHANGES', output)
        for lib in lib_reports:
            lib_compat_info = lib.split(' ')
            lib_name = lib_compat_info[1][:-2]
            lib_compat = lib_compat_info[-2].lower()
            compat_dict[lib_name] = lib_compat
        return compat_dict

    def _run_type_abi_cmd(self):
        """Runs python qiifa_main.py --type abi and returns the
        stderr of running the script.
        """

        commands_lst = ['cd $ANDROID_BUILD_TOP',
                        'cd vendor/qcom/proprietary/commonsys-intf/QIIFA-fwk/',
                        'python qiifa_main.py --type abi']

        err = run_cmd(commands_lst)['stderr']
        return err

    def _run_check_golden_dumps(self, library_name):
        """Checks if golden reference dumps are created.
        """

        commands_lst = ['cd $ANDROID_BUILD_TOP',
                        'cd vendor/qcom/proprietary/commonsys-intf/QIIFA-cmd/abi-dumps/unittest/abi-dumps/arm64/',
                        'ls -ltr ' + library_name]

        err = run_cmd(commands_lst)['stderr']
        return err

    def _run_check_lsdumps(self, library_name):
        """Checks if lsdumps are created.
        """

        commands_lst = ['cd $ANDROID_BUILD_TOP',
                        'cd out/target/product/qssi/qiifa_qssi_current_cmd/abi-dumps/techpackages/QIIFA-unittest/arm64/',
                        'ls -ltr ' + library_name]

        err = run_cmd(commands_lst)['stderr']
        return err
