#DIAG
DIAG := libdiag
DIAG += diag_callback_sample
DIAG += diag_dci_sample
DIAG += diag_klog
DIAG += diag_mdlog
DIAG += diag_socket_log
DIAG += diag_qshrink4_daemon
DIAG += diag_uart_log
DIAG += PktRspTest
DIAG += test_diag
DIAG += libdiagjni

ifeq ($(TARGET_HAS_DIAG_ROUTER),true)
DIAG += diag-router
DIAG += vendor.qti.diaghal@1.0.vendor
DIAG += debug-diag
endif

PRODUCT_PACKAGES += $(DIAG)

#DIAG_DBG
DIAG_DBG := diag_buffering_test
DIAG_DBG += diag_callback_client
DIAG_DBG += diag_dci_client

PRODUCT_PACKAGES_DEBUG += $(DIAG_DBG)
