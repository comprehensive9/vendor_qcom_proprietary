#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.


"""This class runs the QIIFA Test Framework.

IMPORTANT:
 - Pytest version 4.6 must be pip installed on the version
   of python that will be run.
 - Run following commands before running this script.
  - source build/envsetup.sh
  - lunch <target>-userdebug
  - EX: lunch lahaina-userdebug
 - If any new plugins are added, they must be added to the
   _supported_plugins list within parse_args

HOW TO RUN:
 - To run all test cases of all plugins.
  - python<python version number> qiifa_test.py
  - EX: python2 qiifa_test.py

 - To run all test cases of a single plugin.
  - python<python version number> qiifa_test.py --plugin <plugin name>
  - EX: python2 qiifa_test.py --plugin ABI

 - To run the test case of the given index of a single plugin.
  - python<python version number> qiifa_test.py --plugin <plugin name>
    --num <test_case_index>
  - EX: python2 qiifa_test.py --plugin HIDL --num 3

SCRIPT LOGIC:
 - This specfic script will call pytest on the given
   _pytest_file_name within
   the same directory as this script.
 - The pytest file will be called using the subprocess module
   and any command line
   arguments given to this script will also be passed along.
 - The output of running pytest and the summary will be saved
   into log_dir

"""

import sys
import os

from parse_args import ParseArgs
from utils import run_cmd


class QiifaTest(object):

    _pytest_file_name = 'test_plugin.py'
    _log_dir = 'log_dir'
    _output_log_name = 'test_framework_output.log'
    _summary_log_name = 'test_framework_summary.log'

    def __init__(self):
        # Added so that user can call --help or -h argument on the script
        ParseArgs()

    def run_pytest_cmd(self):
        """Runs the pytest command on the specified functions
        or markers within the plugin test class.
        """

        python_version = sys.version[0]
        pytest_cmd_lst = ['python' + python_version,
                          '-m',
                          'pytest',
                          self._get_test_cls_path(),
                          '-v']

        # Pass along the cli arguments to the pytest command
        cli_args = self._cli_args_to_cmd()
        pytest_cmd_lst += cli_args
        cmd_lst = [' '.join(pytest_cmd_lst)]

        # Runs the command and waits for it to complete
        out = run_cmd(cmd_lst)['stdout']
        print(out)

        # Writes the output to the output log
        contents = ' '.join(cli_args) + '\n\n' + out
        self._write_contents_to_log_dir(self._output_log_name, contents)

        # Creates a log that contains the summary of the output
        self._create_framework_summary_log(out)

    def _get_test_cls_path(self):
        """Returns the path of the respective plugin test python file."""

        path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                            self._pytest_file_name)
        return path

    def _cli_args_to_cmd(self):
        """Directly receives the cli args from the user and returns it."""

        return sys.argv[1:]

    def _write_contents_to_log_dir(self, file_name, contents):
        """Writes the given contents to the given file_name within log_dir."""

        if not os.path.exists(self._log_dir):
            os.mkdir(self._log_dir)
        file_path = os.path.join(self._log_dir, file_name)
        with open(file_path, 'w') as fp:
            fp.write(contents)

    def _create_framework_summary_log(self, output):
        """Based on the output of running the framework, will parse
        the number of test cases passed and failed.

        This method is only parsing the top of the output when run
        with verbose argument.
        """

        passed, failed = 0, 0
        for line in output.split('\n'):
            if 'PASSED' in line:
                passed += 1
            elif 'FAILED' in line:
                failed += 1
            if 'FAILURES' in line:
                break

        summary = 'Pass: ' + str(passed) + ' Failed: ' + str(failed)
        self._write_contents_to_log_dir(self._summary_log_name, summary)


if __name__ == '__main__':
    QiifaTest().run_pytest_cmd()
