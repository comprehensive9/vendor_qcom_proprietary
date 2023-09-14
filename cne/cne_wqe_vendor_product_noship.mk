
ifneq ($(TARGET_BOARD_AUTO),true)

#CNE_WQE
CNE_WQE := andsfCne.xml
CNE_WQE += profileMwqem.xml
CNE_WQE += mwqem.conf
CNE_WQE += libwqe
CNE_WQE += SwimConfig.xml

PRODUCT_PACKAGES += $(CNE_WQE)

endif
