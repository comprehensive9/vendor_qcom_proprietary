ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif
SSG_MINK_PROJECT_PATH := $(TOP)/$(QC_PROP_ROOT)/ssg/mink
HAS_SSG_MINK_PROJECT_PATH := false
HAS_SSG_MINK_PROJECT_PATH := $(shell if [[ -d $(SSG_MINK_PROJECT_PATH) || -d $(SSG_MINK_PROJECT_PATH) ]] ; then echo true; fi)

ifeq ($(HAS_SSG_MINK_PROJECT_PATH),true)
ifeq ($(call is-board-platform-in-list, msm8998 apq8098_latv sdm660 sdm845 sdm710 msmnile sdmshrike $(MSMSTEPPE) lito atoll bengal monaco $(TRINKET)),true)

include $(call all-subdir-makefiles)

endif # is-board-platform-in-list
endif # HAS_SSG_MINK_PROJECT_PATH
