#!/usr/bin/python
# -*- coding: utf-8 -*-
#Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

class plugin_interface:
    def __init__():
        Logger.logInternal.info("in plugin base class implementation: __init__")

    def register(self):
        Logger.logInternal.info("in plugin base class implementation: register")

    def config(self, QIIFA_type, libsPath=None, CMDPath=None):
        Logger.logInternal.info("in plugin base class implementation: config")

    def generateGoldenCMD(self, libsPath=None, storagePath=None, create_args_lst=None):
        Logger.logInternal.info("in plugin base class implementation: generateGoldenCMD")

    def IIC(self, **kwargs):
        Logger.logInternal.info("in plugin base class implementation: IIC")
