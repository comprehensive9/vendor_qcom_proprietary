ENABLE_FASTCV := true
ENABLE_SCVE := true
ENABLE_CVP := true
ENABLE_OPENVX := true
ifeq ($(TARGET_USES_QMAA), true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_FASTCV), true)
ENABLE_FASTCV := false
endif
ifneq ($(TARGET_USES_QMAA_OVERRIDE_SCVE), true)
ENABLE_SCVE := false
endif
ifneq ($(TARGET_USES_QMAA_OVERRIDE_CVP), true)
ENABLE_CVP := false
endif
ifneq ($(TARGET_USES_QMAA_OVERRIDE_OPENVX), true)
ENABLE_OPENVX := false
endif
endif

#FASTCV
FASTCV := libapps_mem_heap
FASTCV += libapps_mem_heap.so
FASTCV += libdspCV_skel
FASTCV += libdspCV_skel.so
FASTCV += libfastcvadsp
FASTCV += libfastcvadsp.so
FASTCV += libfastcvadsp_skel
FASTCV += libfastcvadsp_skel.so
FASTCV += libfastcvdsp_skel
FASTCV += libfastcvdsp_skel.so
FASTCV += libfastcvadsp_stub
FASTCV += libfastcvdsp_stub
FASTCV += libfastcvopt
#FASTCV_TEST_BINARIES
FASTCV_INTERNAL += fastcv_test_oem
FASTCV_INTERNAL += fastcv_test
FASTCV_INTERNAL += fastcv_test_noopencv
#SCVE
SCVE := _conf_eng_num_sym_font40_4transd_zscore_morph_.trn2876.trn
SCVE += _conf_eng_num_sym_font40_conc2_meshrn__de__1_1__zscore_morph.trn10158.trn
SCVE += _conf_eng_num_sym_font40_rbp_data5100_patch500_5x5_24x24_dim727.trn31585.trn
SCVE += _eng_font40_4transmeshrnorm6x6_leaflda85_ligature_ext14_c70_sp1lI_newxml3.trn31299.trn
SCVE += _numpunc_font40_4transmeshrnorm_leafnum1.trn9614.trn
SCVE += _numpunc_font40_conc2_DEFn__BGTouchy6x6n__1_1__morph.trn32025.trn
SCVE += _numpunc_parteng_font40_4transmeshr_morph.trn400.trn
SCVE += character.cost
SCVE += CharType.dat
SCVE += chinese.lm
SCVE += chinese_address_20150304.bin
SCVE += ChinesePunctuation.rst
SCVE += cnn_small5_synthrev_sw_sampled2_bin
SCVE += dcnConfigForEngCNN.txt
SCVE += dcnConfigForMultiCNN.txt
SCVE += english_address_20150213.bin
SCVE += english_dictionary_20150213.bin
SCVE += forestData.bin
SCVE += glvq_kor_2197classes_576_100dim_i42_centroidNorm.dat
SCVE += glvq_kor_consonant_19classes_64_16dim_i0_linearNorm.dat
SCVE += GLVQDecoder_fixed.ohie
SCVE += gModel.dat
SCVE += hGLVQ_kor_RLF80_float.hie
SCVE += korean.lm
SCVE += korean_address_20150129.bin
SCVE += korean_dictionary_20150414.bin
SCVE += LDA_kor_2197classes_576dim_centroidNorm.dat
SCVE += LDA_kor_consonant_19classes_64dim_linearNorm.dat
SCVE += libhvxMathVIO
SCVE += libhvxMathVIO.so
SCVE += libobjectMattingApp_skel
SCVE += libobjectMattingApp_skel.so
SCVE += libscveBlobDescriptor
SCVE += libscveBlobDescriptor_skel
SCVE += libscveBlobDescriptor_skel.so
SCVE += libscveBlobDescriptor_stub
SCVE += libscveCleverCapture
SCVE += libscveCleverCapture_skel
SCVE += libscveCleverCapture_skel.so
SCVE += libscveCleverCapture_stub
SCVE += libscveCommon
SCVE += libscveCommon_stub
SCVE += libscveFaceLandmark_skel
SCVE += libscveFaceLandmark_skel.so
SCVE += libscveFaceLandmarks
SCVE += libscveFaceLandmarks_stub
SCVE += libscveFaceRecognition
SCVE += libscveFaceRecognition_skel
SCVE += libscveFaceRecognition_skel.so
SCVE += libscveFaceRecognition_stub
SCVE += libscveImageCloning
SCVE += libscveImageCorrection
SCVE += libscveImageRemoval
SCVE += libscveMotionVector
SCVE += libscveObjectMatting
SCVE += libscveObjectMatting_stub
SCVE += libscveObjectSegmentation
SCVE += libscveObjectSegmentation_skel
SCVE += libscveObjectSegmentation_skel.so
SCVE += libscveObjectSegmentation_stub
SCVE += libscveObjectTracker
SCVE += libscveObjectTracker_stub
SCVE += libscvePanorama
SCVE += libscvePanorama_lite
SCVE += libscveScan3D
SCVE += libscveScan3D.so
SCVE += libPlaneSegmentationWrapper
SCVE += libPlaneSegmentationWrapper.so
SCVE += libscveT2T_skel
SCVE += libscveT2T_skel.so
SCVE += libscveTextReco
SCVE += libscveTextReco_skel
SCVE += libscveTextReco_skel.so
SCVE += libscveTextReco_stub
SCVE += libscveTextRecoPostProcessing
SCVE += libscveVideoSummary
SCVE += libscveVideoSummary_skel
SCVE += libscveVideoSummary_skel.so
SCVE += libscveVideoSummary_stub
SCVE += nontextremoval_eng.model
SCVE += nontextremoval_multilang.model
SCVE += punRangeData.rst
SCVE += vendor.qti.hardware.scve.panorama@1.0-adapter-helper
SCVE += vendor.qti.hardware.scve.panorama@1.0
SCVE += vendor.qti.hardware.scve.panorama@1.0-halimpl
SCVE += vendor.qti.hardware.scve.panorama@1.0-impl
SCVE += vendor.qti.hardware.scve.panorama@1.0-service.rc
SCVE += vendor.qti.hardware.scve.panorama@1.0-service
SCVE += vendor.qti.hardware.scve.objecttracker@1.0-adapter-helper
SCVE += vendor.qti.hardware.scve.objecttracker@1.0
SCVE += vendor.qti.hardware.scve.objecttracker@1.0-halimpl
SCVE += vendor.qti.hardware.scve.objecttracker@1.0-impl
SCVE += vendor.qti.hardware.scve.objecttracker@1.0-service.rc
SCVE += vendor.qti.hardware.scve.objecttracker@1.0-service

SCVE_INTERNAL := scveTestCleverCapture
SCVE_INTERNAL += scveTestFaceLandmarks
SCVE_INTERNAL += scveTestFaceRecognition
SCVE_INTERNAL += scveTestImageCloning
SCVE_INTERNAL += scveTestImageCorrection
SCVE_INTERNAL += scveTestImageRemoval
SCVE_INTERNAL += scveTestObjectMatting
SCVE_INTERNAL += scveTestObjectSegmentation
SCVE_INTERNAL += scveTestObjectTracker
SCVE_INTERNAL += scveTestPanorama
SCVE_INTERNAL += scveTestScan3D
SCVE_INTERNAL += scveTestTextReco
SCVE_INTERNAL += scveTestVideoSummary

#CVP COMMON
CVP_COMMON := libcvp_common
CVP_COMMON += libcvp_common.so
CVP_COMMON += libcvpcpuRev_skel
CVP_COMMON += libcvpcpuRev_skel.so

#CVP 1.0
CVP1_0 := libcvp
CVP1_0 += libcvp.so
CVP1_0 += libcvp_stub
CVP1_0 += libcvp_stub.so
CVP1_0 += libcvpdsp_skel
CVP1_0 += libcvpdsp_skel.so
CVP1_0 += vendor.qti.hardware.cvp@1.0-adapter-helper
CVP1_0 += vendor.qti.hardware.cvp@1.0
CVP1_0 += vendor.qti.hardware.cvp@1.0-halimpl
CVP1_0 += vendor.qti.hardware.cvp@1.0-impl
CVP1_0 += vendor.qti.hardware.cvp@1.0-service.rc
CVP1_0 += vendor.qti.hardware.cvp@1.0-service

#CVP 2.0
CVP2_0 := libcvp2
CVP2_0 += libcvp2.so
CVP2_0 += libcvp2_hfi
CVP2_0 += libcvp2_hfi.so
CVP2_0 += libcvpdsp_2_0
CVP2_0 += libcvpdsp_2_0.so
CVP2_0 += model1.dat
CVP2_0 += libcvpdsp_skel
CVP2_0 += libcvpdsp_skel.so

#CVP 2.1
CVP2_1 := libcvp2_1
CVP2_1 += libcvp2_1.so
CVP2_1 := libcvp_util
CVP2_1 += libcvp_util.so
CVP2_1 += libcvpdsp_2_1
CVP2_1 += libcvpdsp_2_1.so
CVP2_1 += model1.dat
CVP2_1 += model2.dat
CVP2_1 += evautil.b00
CVP2_1 += evautil.b01
CVP2_1 += evautil.b02
CVP2_1 += evautil.b03
CVP2_1 += evautil.b04
CVP2_1 += evautil.b05
CVP2_1 += evautil.b06
CVP2_1 += evautil.b07
CVP2_1 += evautil.mdt
CVP2_1 += evautil64.b00
CVP2_1 += evautil64.b01
CVP2_1 += evautil64.b02
CVP2_1 += evautil64.b03
CVP2_1 += evautil64.b04
CVP2_1 += evautil64.b05
CVP2_1 += evautil64.b06
CVP2_1 += evautil64.b07
CVP2_1 += evautil64.mdt

CVP_INTERNAL += cvpTest
CVP_INTERNAL += cvp2Test
CVP_INTERNAL += cvp2TestSSRCtrlPath32
CVP_INTERNAL += cvp2TestSSRDataPath32
CVP_INTERNAL += libcvpdspTest_skel
CVP_INTERNAL += libcvpdspTest_skel.so

#OPENVX
OPENVX := libopenvx
OPENVX += libopenvxdsp_skel
OPENVX += libopenvxdsp_skel.so

#FACE3D
FACE3D := libQ6MSFR_manager_skel
FACE3D += libQ6MSFR_manager_skel.so
FACE3D += qcnn_concat_file_model-0324_2_encrypted
FACE3D += libQ6MSFR_manager_stub
FACE3D += libQ6MSFR_manager_stub.so
FACE3D += libFace3DTA
FACE3D += libFace3DTA.so
FACE3D += libFace3D_hlos
FACE3D += libFace3D_hlos.so
FACE3D += libface3d_dev
FACE3D += libface3d_dev.so
FACE3D += libcamera2ndk_vendor
FACE3D += libcamera2ndk_vendor.so

FACE3D_INTERNAL := face3d_test
FACE3D_INTERNAL += face3d_test64

# Enable compilation of FASTCV
ifeq ($(ENABLE_FASTCV), true)
PRODUCT_PACKAGES += $(FASTCV)
PRODUCT_PACKAGES += $(FASTCV_INTERNAL)
endif

# Enable compilation of SCVE
ifeq ($(ENABLE_SCVE), true)
PRODUCT_PACKAGES += $(SCVE)
PRODUCT_PACKAGES += $(SCVE_INTERNAL)
endif

# Enable compilation of CVP
ifeq ($(ENABLE_CVP), true)
PRODUCT_PACKAGES += $(CVP_COMMON)

ifeq ($(call is-board-platform-in-list,lahaina),true)
   PRODUCT_PACKAGES += $(CVP2_1)
endif
ifeq ($(call is-board-platform-in-list,kona),true)
   PRODUCT_PACKAGES += $(CVP2_0)
endif
ifeq ($(call is-board-platform-in-list,msmnile lito),true)
   PRODUCT_PACKAGES += $(CVP1_0)
endif

PRODUCT_PACKAGES += $(CVP_INTERNAL)
endif

# Enable compilation of CVP
ifeq ($(ENABLE_OPENVX), true)
PRODUCT_PACKAGES += $(OPENVX)
endif

PRODUCT_PACKAGES += $(FACE3D)
PRODUCT_PACKAGES += $(FACE3D_INTERNAL)
