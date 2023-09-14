#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class will open up the test_cases.json file and select the test
cases to be run depending on the arguments found within parse_args.

IMPORTANT:
 - The _json_path variable must contain the path to the
   test_cases.json file.

SCRIPT LOGIC:
 - The get_test_cases_lst will be used by test_plugin to get the
   test cases to be run.
 - The get_test_cases_lst will return a list of tuples that will
   match the params for the test_plugin test_backwards_compatiblity
   method and parametrize. The order should be preserved.
"""


import json
import os


class TestCaseSelector(object):

    _json_path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                              'test_cases.json')

    def __init__(self, parse_args):
        self._test_case_dict = self._load_json()
        self._parse_args = parse_args

    def _load_json(self):
        """Loads the json file that contains the test cases into
        a python dictionary.
        """

        with open(self._json_path, 'r') as fp:
            plugin_test_cases = json.load(fp)
            return plugin_test_cases

    def get_test_cases_lst(self):
        """Selects the test cases to be run depending on the cli
        arguments given by the user.
        """

        test_cases_lst = []
        for plugin in self._parse_args.get_plugin_lst():
            plugin_tcs = self._test_case_dict[plugin]
            for tc_name, tc_kwargs in plugin_tcs.items():
                tc_ind = tc_kwargs['index']
                index_arg_val = self._parse_args.get_index()

                # If index arg is not within range of indices, raise an error
                if index_arg_val > len(plugin_tcs) - 1:
                    error_msg = 'Invalid index argument entered. ' \
                                'Highest index available for the ' +\
                                plugin + ' plugin, is ' +\
                                str(len(plugin_tcs) - 1)
                    self._parse_args.raise_parser_error(error_msg)

                # Add the plugin to the test case kwargs
                tc_kwargs['plugin'] = plugin

                # If the index argument isn't used, it should have value of -1
                if index_arg_val == -1:
                    test_case_params = (plugin, tc_name, tc_kwargs)
                    test_cases_lst.append(test_case_params)
                else:

                    # Return test_case_params if exact index found
                    if index_arg_val == tc_ind:
                        test_case_params = (plugin, tc_name, tc_kwargs)
                        return [test_case_params]
        return test_cases_lst
