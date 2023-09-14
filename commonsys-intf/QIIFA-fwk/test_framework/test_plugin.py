#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class contains the test cases of the Test Framework that will be run.

IMPORTANT:
 - pytest version 4.6 required.

SCRIPT LOGIC:
 - The script contains the parse_args class that will parse the
   arguments passed along by qiifa_test.
 - The script contains the test_case_selector class that will select
   the test cases that the user indicates they want to run. That
   class will contain an instance of parse_args.
 - The script will call the get_test_cases_lst() within
   test_case_selector and loop through the test cases using
   pytest parametrize.
    - The script contains the test_engine class that will run
      the workflow of each test case by dynamically calling the
      plugin compatibility class given the plugin of the test case.
 - Finally the test_backwards_compatibility method will assert
   that the result is equivalent to the expected result.

IMPLEMENTATION DETAILS:
 - The test_backwards_compatibility method takes the plugin and
   test_case_name as params so that the output of pytest will
   contain those pieces of information.
 - Any other pieces of information that absolutely must be found
   within the output must be added similarly as params and subsequently
   added within the tuple list that is returned from
   test_case_selector.get_test_cases_lst() with the
   order of the parms preserved.
"""

import pytest

from test_engine import TestEngine
from parse_args import ParseArgs
from test_case_selector import TestCaseSelector


class TestPlugin(object):
    pytest.parse_args = ParseArgs()
    pytest.test_case_selector = TestCaseSelector(pytest.parse_args)
    pytest.test_eng = TestEngine(pytest.parse_args.get_plugin_lst())

    @pytest.mark.parametrize("plugin, test_case_name, test_case_kwargs",
                             pytest.test_case_selector.get_test_cases_lst())
    def test_backwards_compatability(self,
                                     plugin,
                                     test_case_name,
                                     test_case_kwargs):
        """Runs all the test cases for the given plugin using parametrize
        and checks if the result matches the expected result.
        """

        try:
            expected_result = test_case_kwargs['expected_result']
        except KeyError:
            raise Exception('Test case kwargs missing expected_result key.')

        result = pytest.test_eng.compatibility(test_case_kwargs)
        assert result == expected_result
