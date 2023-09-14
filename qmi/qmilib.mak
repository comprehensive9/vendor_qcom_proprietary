# -*- Mode: text -*-
#==============================================================================
# FILE: qmilib.mak
#
# SERVICE: 
#
# DESCRIPTION:
#
# Copyright 2012 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

default: all

RELPATHTOROOT = ../../../..

TARGET = libqmi
MAKEFILETYPE = shared
MAKEFILENAME = qmilib.mak

C_SRC =\
   src/qmi.c\
   src/qmi_atcop_srvc.c\
   src/qmi_eap_srvc.c\
   src/qmi_nas_srvc.c\
   src/qmi_qos_srvc.c\
   src/qmi_service.c\
   src/qmi_wds_srvc.c\
   src/qmi_wds_utils.c\
   src/qmi_errors.c\
   src/qmi_client.c ## realy is qcci_legacy, but just put here for now

NEEDINC=\
   $(QC_PROP_ROOT)/qmi/inc\
   $(QC_PROP_ROOT)/qmi/src\
   $(QC_PROP_ROOT)/qmi/platform\
   $(QC_PROP_ROOT)/qmi/proxy\
   $(QC_PROP_ROOT)/qmi/core/lib/inc\
   $(QC_PROP_ROOT)/qmi/services\
   $(QC_PROP_ROOT)/common/inc\
   $(QC_PROP_ROOT)/data/dsutils/inc\
   $(QC_PROP_ROOT)/diag/include\

NEEDLIB =\
   $(QC_PROP_ROOT)/qmi/services/libqmiservices\
   $(QC_PROP_ROOT)/qmi/qmi_client_qmux/libqmi_client_qmux\

CFLAGS += -m32
CFLAGS += -DFEATURE_DATA_LOG_QXDM
CFLAGS += -DFEATURE_RILTEST
CFLAGS += -include string.h

CFLAGS += -DFEATURE_QMI_ANDROID

LDFLAGS += -m32

BTBUILD=$(QC_PROP_ROOT)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

-include ./${MAKEFILENAME}.depend.mki

#======================================================================
# Add Mannual dependencies and makefile overrides here
#======================================================================

#EOF
