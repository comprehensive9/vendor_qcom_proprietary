#QCV Core
QCV_UTILS := libsoc_helper
QCV_UTILS += libsoc_helper_jni
QCV_UTILS += libsoc_helper_static
QCV_UTILS += init.qti.qcv.rc
QCV_UTILS += init.qti.qcv.sh

#QCV Test Modules
QCV_UTILS += libsoc_helper_test
QCV_UTILS += LibsocHelperTest

PRODUCT_PACKAGES += $(QCV_UTILS)
