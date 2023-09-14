#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.


"""Abstract class that makes sure that any compatibility classes
contain a compatibility method with the exact same signature.

IMPORTANT:
 - All compatibility classes should inherit from this class and
   should contain a compatibility method with the same signature.


"""


import subprocess
import abc


class CompatibilityCheck(object):

    def __init__(self):
        self.build_path = self.run_cmd(['echo "$ANDROID_BUILD_TOP"'])['stdout']

    @abc.abstractmethod
    def compatibility(self, test_case_kwargs):
        """Check for this signature among all subclasses."""
        raise NotImplementedError

    def run_cmd(self, cmd_lst):
        """Run command list using subprocess and return a dict that
        contains information about the subprocess.
        """

        process = subprocess.Popen('/bin/bash',
                                   stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        cmd = '\n'.join(cmd_lst)
        out, err = process.communicate(cmd)

        # Create a dictionary with information of the subprocess
        proc_dict = {}
        proc_dict['stdout'] = out
        proc_dict['stderr'] = err
        proc_dict['returncode'] = process.returncode

        # Remove unecessary whitespace and newlines from strs within proc_dict
        for key, val in proc_dict.items():
            if isinstance(val, str):
                proc_dict[key] = val.strip()

        return proc_dict


if __name__ == '__main__':
    print(CompatibilityCheck().build_path)
