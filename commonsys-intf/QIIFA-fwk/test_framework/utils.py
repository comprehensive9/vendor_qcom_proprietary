#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""This file will contain static utility functions."""

import subprocess
import shutil


def run_cmd(cmd_lst):
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

    # Remove unecessary whitespace and newline from strs within proc_dict
    for key, val in proc_dict.items():
        if isinstance(val, str):
            proc_dict[key] = val.strip()

    return proc_dict


def copy_file(old_path, new_path):
    """Will copy a file from an old path to a new path. This
    will also overwrite the file if already exists in new path.
    """

    try:
        shutil.copy2(old_path, new_path)
    except shutil.Error:
        return False
    return True


def ver_lst_to_dict(ver_lst):
    """Converts from a version list to a version dict."""
    ver_dict = {}
    for major in ver_lst:
        if '-' in major:
            # Split the string by the - char
            dash_split = major.split('-')
            earliest_ver = dash_split[0]
            max_minor_ver = dash_split[1]

            # Split the string by the . char
            dot_split = earliest_ver.split('.')
            min_minor_ver = dot_split[1]
            maj_ver = dot_split[0]

            minor_ver_lst = [i for i in range(int(min_minor_ver),
                                              int(max_minor_ver) + 1)]
        else:
            # Split the string by the . char
            dot_split = major.split('.')
            minor_ver = dot_split[1]
            maj_ver = dot_split[0]

            # The minor version list will only have one value
            minor_ver_lst = [int(minor_ver)]
        ver_dict[maj_ver] = minor_ver_lst

    return ver_dict


def ver_dict_to_lst(ver_dict):
    """Converts from a version dict to a version list."""
    ver_lst = []
    for major, minor_lst in ver_dict.items():

        # Continue to the next major ver if no minor ver within the list
        if len(minor_lst) == 0:
            continue

        start = minor_lst[0]
        end = start
        for i in range(1, len(minor_lst)):
            next_num = minor_lst[i]
            if end + 1 == next_num:
                end += 1
            else:
                combined = major + '.' + str(start)
                if start != end:
                    combined += '-' + str(end)
                ver_lst.append(combined)
                start = next_num
                end = start

        # Add the very last version string that wasn't added within loop
        combined = major + '.' + str(start)
        if start != end:
            combined += '-' + str(end)
        ver_lst.append(combined)
    return ver_lst
