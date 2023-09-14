ifeq ($(strip $(TARGET_USES_FACEAUTH)),true)
#FaceAuth Build Rules
PRODUCT_PACKAGES += android.hardware.biometrics.face@1.0-service.faceauth
PRODUCT_PACKAGES += android.hardware.biometrics.face@1.0-service.faceauth.rc
endif
