#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

"""Contents of conftest.py"""

import pytest


def pytest_addoption(parser):
    """Allows for pytest to include the --plugin argument when
    being invoked through the command line.
    """
    parser.addoption(
        "--plugin", action="store", nargs="+"
    )
    parser.addoption(
        "--index", action="store"
    )
