# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""

.. data::  bin_names

    Given a file base name, returns the list of possible binary name on the
    current system. For eg, on windows, this will add the '.exe'

.. data::  packaged_bin_folder

    Name of the folder under the bin directory to be used on the current system.

"""

import os
import sys

from . import c_path

DIR_PATH = os.path.dirname(os.path.abspath(__file__))
BIN_PATH = c_path.join(DIR_PATH, '..', '..', '..', 'bin')


if sys.platform.startswith('linux'):
    bin_names = lambda x: [x]
    packaged_bin_folder = c_path.join(BIN_PATH, 'LIN')
elif sys.platform == 'win32':
    bin_names = lambda x: [x + '.exe']
    packaged_bin_folder = c_path.join(BIN_PATH, 'WIN')
elif sys.platform == 'cygwin':
    bin_names = lambda x: [x, x + '.exe']
    packaged_bin_folder = c_path.join(BIN_PATH, 'WIN')
elif sys.platform == 'darwin':
    bin_names = lambda x: [x]
    packaged_bin_folder = c_path.join(BIN_PATH, 'MAC', '10.8.5')
    import os
    os.environ['DYLD_LIBRARY_PATH']=packaged_bin_folder

if sys.platform.startswith('linux') or sys.platform in ['win32', 'cygwin', 'darwin']:
    alternate_bin_names = lambda x: [x]


def is_linux():
    return sys.platform.startswith('linux')


def is_windows():
    return sys.platform in ['win32']


def linux_only(func):
    def decorated(*args, **kwargs):
        if is_linux():
            func(*args, **kwargs)
        return func
    return decorated


def windows_only(func):
    def decorated(*args, **kwargs):
        if is_windows():
            func(*args, **kwargs)
        return func
    return decorated