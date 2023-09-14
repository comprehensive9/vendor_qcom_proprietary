#QMMI APK
QMMI_APK += android.hardware.light.xml
QMMI_APK += vendor.qti.hardware.factory.xml

#QMMI DAEMON
QMMI_DAEMON := mmid

#HIDL
FACTORY_HIDL = vendor.qti.hardware.factory@1.0
FACTORY_HIDL += vendor.qti.hardware.factory-V1.0-java
FACTORY_HIDL += vendor.qti.hardware.factory@1.1
FACTORY_HIDL += vendor.qti.hardware.factory-V1.1-java
FACTORY_HIDL += vendor.qti.hardware.factory@1.0-service

ifeq ($(call is-board-platform-in-list,kona lahaina lito holi atoll sm6150 msmnile sdm710 trinket bengal bengal_32 bengal_32go msm8998 sdm660 msm8937 monaco sdm845),true)
PRODUCT_PACKAGES += $(QMMI_APK)
PRODUCT_PACKAGES += $(QMMI_DAEMON)
PRODUCT_PACKAGES += $(FACTORY_HIDL)
endif
