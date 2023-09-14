#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This class will dynamically call the respective plugin compatibility
class for the given test case depending on the plugin.

SCRIPT LOGIC:
 - The _plugin_cls_compat_dict contains key value pairs of the plugin
   name and plugin compatibility class instances.
 - The compatibility method will get the test_case_kwargs as a
   parameter and will call the respective plugin compatiblity
   class method and return the result.
"""


import importlib


class TestEngine(object):

    def __init__(self, plugin_lst):
        self._plugin_lst = plugin_lst
        self._plugin_cls_compat_dict = self._get_plugin_compat_dict()

    def _get_plugin_compat_dict(self):
        """Iterate through the plugin_lst, create an instance of the
        compatibility classes of each plugin, then add to dictionary.

        The dictionary will contain key value pairs of the plugin name
        and plugin compatibility class instances.
        """

        compat_dict = {}
        for plugin in self._plugin_lst:
            compat_dict[plugin] = self._get_plugin_compat_cls(plugin)
        return compat_dict

    def _get_plugin_compat_cls(self, plugin):
        """Use importlib to dynamically get the compatibility class
        of the given plugin.
        """

        plugin_compat_fname = plugin.lower() + '_compatibility'
        plugin_module = importlib.import_module(plugin_compat_fname)
        plugin_class = getattr(plugin_module, plugin + 'Compatibility')
        return plugin_class()

    def compatibility(self, test_case_kwargs):
        """Calls the appropriate is compatible method depending on
        the plugin of the given test case.
        """

        try:
            plugin = test_case_kwargs['plugin']
        except KeyError:
            raise Exception('Test case kwargs missing plugin key.')

        plugin_compat_cls = self._plugin_cls_compat_dict[plugin]
        result = plugin_compat_cls.compatibility(test_case_kwargs)
        return result
