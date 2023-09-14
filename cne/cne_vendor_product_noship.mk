
ifneq ($(TARGET_BOARD_AUTO),true)

#CNE
CNE := cnd

ifeq ($(GENERIC_ODM_IMAGE),true)
CNE += cnd-generic.rc
else
CNE += cnd.rc
CNE += com.qualcomm.qti.cne.xml
endif

CNE += CneApp
CNE += libcne
CNE += libcneapiclient
CNE += libwms
CNE += libxml

#this lib is no more compiled
#CNE += libvendorconn
#CNE += libmasc

#CNE_DBG
#CNE_DBG += test2client

#MUTUALEX
MUTUALEX := mutualex
MUTUALEX += mutualex.rc

#MODEMMANAGER
ifeq ($(PRODUCT_ENABLE_QESDK),true)
MODEMMANAGER := modemManager
MODEMMANAGER += modemManager.rc
PRODUCT_PACKAGES += $(MODEMMANAGER)
endif

PRODUCT_PACKAGES_ENG += cneapitest
PRODUCT_PACKAGES += $(CNE)
PRODUCT_PACKAGES += $(MUTUALEX)

endif

#CACERT
CACERT := CACertService
CACERT += libcacertclient
CACERT += libjnihelper

PRODUCT_PACKAGES += $(CACERT)
