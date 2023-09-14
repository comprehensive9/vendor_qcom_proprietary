#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module
#
# GENERAL DESCRIPTION
#   Location Service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(addsuffix /Android.mk,$(addprefix $(LOCAL_PATH)/, core lcp framework-glue))
