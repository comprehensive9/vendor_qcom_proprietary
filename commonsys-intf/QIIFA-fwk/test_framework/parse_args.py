#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class will use the argparse module to parse the command line
arguments given by the user.

IMPORTANT:
 - Any arguments that are added here must also be added to conftest.py
 - The _supported_plugins list within this class contains the updated
   supported plugins for the framework
 - If any new plugins are added, they must be added to the
   _supported_plugins list

SCRIPT LOGIC:
 - The args are parsed within the _parse_arguments method
 - The logic of the arguments are checked within the
   check_argument_logic method.
 - The check_argument_logic method, and subsequently the
   _parse_arguments method also returns the values of
   the arguments given by the user
"""

import argparse


class ParseArgs(object):

    _supported_plugins = ['ABI', 'HIDL']

    def __init__(self):
        self._parser = None
        self._plugin_lst, self._index = self._parse_arguments()

    def _parse_arguments(self):
        """Parses the arguments passed along from qiifa_test to test_plugin."""

        self._parser = argparse.ArgumentParser()

        # Account for test_plugin file for pytest to run from qiifa_test
        self._parser.add_argument('file',
                                  nargs='?',
                                  default='hello')

        self._parser.add_argument('-v',
                                  action='store_true',
                                  help='indicate verbose pytest output.')
        self._parser.add_argument('--plugin',
                                  nargs='+',
                                  type=str,
                                  help='the plugin/s to be tested.')
        self._parser.add_argument('--index',
                                  nargs=1,
                                  type=int,
                                  help='the index of the test case to be run.')
        args = self._parser.parse_args()

        plugin_lst, index = self._check_argument_logic(args)
        return plugin_lst, index

    def _check_argument_logic(self, args):
        """Verifies that the logic of the given arguments is accurate."""

        # Determine if the flags were used
        plugin_flag = args.plugin is not None
        index_flag = args.index is not None

        # Verify that the index flag is only used when plugin argument is given
        if not plugin_flag and index_flag:
            msg = 'index arg can only be used when a plugin is specified.'
            self.raise_parser_error(msg)

        # Verify that the number entered is valid.
        if args.index and args.index[0] < 0:
            msg = 'Invalid number entered. Number must be above 0.'
            self.raise_parser_error(msg)

        # Insert values for the arguments within variables
        plugin_lst = args.plugin if plugin_flag else self._supported_plugins
        index = args.index[0] if index_flag else -1

        # Virify that the entered plugins are supported
        for i, plugin in enumerate(plugin_lst):
            plugin = plugin.upper()
            if plugin_flag and plugin not in self._supported_plugins:
                msg = 'Plugin not supported. Plugins that are ' \
                      'currently supported: ' + str(self._supported_plugins)
                self.raise_parser_error(msg)
            plugin_lst[i] = plugin
        return plugin_lst, index

    def get_plugin_lst(self):
        """Returns the list of plugin names that will be run."""

        return self._plugin_lst

    def get_index(self):
        """Returns the index of test cases to be run."""

        return self._index

    def raise_parser_error(self, message):
        """Raise a parser error with the given message."""

        self._parser.error(message)
