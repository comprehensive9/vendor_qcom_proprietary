ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_VERSION)

ifeq ($(ADRENO_MIN_BUILD),)
ifeq ($(ADRENO_FULL_BUILD),)
# Check PLATFORM_SDK version numbers
# Add checks as necessary for {eq, ne, lt, le, gt, ge}
ifeq ($(ADRENO_EXT_VBUILD),)   # VGDB build?
# Linux host
CHECK_VERSION_LT = $(shell if [ $(1) -lt $(2) ] ; then echo true ; else echo false ; fi)
CHECK_VERSION_GE = $(shell if [ $(1) -ge $(2) ] ; then echo true ; else echo false ; fi)
else
# Windows host
CHECK_VERSION_LT = $(shell if $1 LSS $2 (echo true) else (echo false))
CHECK_VERSION_GE = $(shell if $1 GEQ $2 (echo true) else (echo false))
endif #ifneq ($(ADRENO_EXT_VBUILD),)

PLATFORM_SDK_ICECREAM_SANDWICH = 14
PLATFORM_SDK_JELLY_BEAN = 16
PLATFORM_SDK_JELLY_BEAN_MR1 = 17
PLATFORM_SDK_KIT_KAT = 19
PLATFORM_SDK_LPDK = 20
PLATFORM_SDK_MPDK = 23
PLATFORM_SDK_NPDK = 24
PLATFORM_SDK_NMR1PDK = 25
PLATFORM_SDK_OPDK    = 26
PLATFORM_SDK_OMR1PDK = 27
PLATFORM_SDK_PPDK    = 28
PLATFORM_SDK_QPDK    = 29
PLATFORM_SDK_RPDK    = 30
PLATFORM_SDK_SPDK    = 31

# If Android's PLATFORM_SDK_VERSION has not been updated
# try using the PLATFORM_VERSION to detect the version.
# Note: PLATFORM_VERSION is defined as an arbitrary string
# (e.g. "5.1" vs "M") and cannot be relied upon in general to
# identify the Android version.
ifeq ($(PLATFORM_VERSION), S)
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_SPDK)
endif
ifeq ($(PLATFORM_VERSION), R)
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_RPDK)
endif
ifeq ($(PLATFORM_VERSION), Q)
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_QPDK)
endif
ifeq ($(PLATFORM_VERSION), P)
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_PPDK)
endif
# If PLATFORM_VERSION equals O or ODR
ifneq ($(filter O ODR, $(PLATFORM_VERSION)), )
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_OPDK)
endif
# If PLATFORM_VERSION equals OMR1 or OPR1 or OPD1 or OPM1
ifneq ($(filter OMR1 OPR1 OPD1 OPM1, $(PLATFORM_VERSION)), )
ADRENO_PLATFORM_SDK_VERSION= $(PLATFORM_SDK_OMR1PDK)
endif
# Guess the MPDK version since it isn't defined yet
endif #ifeq ($(ADRENO_FULL_BUILD),)
endif #ifeq ($(ADRENO_MIN_BUILD),)
