/*****************************************************************************
============================
Copyright (c)  2004-2012,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
 File: VideoTransmitter.h
 Description: Implementation class for VideoTransmitter

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-12-11    Rakesh K         First Version
 1-Dec-16   Sanjeev Mittal        Rearchiecture Version
 *****************************************************************************/

#ifndef __VIDEOTRANSMITTER_H__
#define __VIDEOTRANSMITTER_H__
#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
#include <VTRecorder.h>
#include <VideoOmx.h>
#include <OMX_VideoExt.h>
#include <OMX_IndexExt.h>
#include <media/IOMX.h>
#include <binder/MemoryDealer.h>
#include <gui/IProducerListener.h>
#include <media/MediaCodecBuffer.h>

/* To include the instance for NAtive handle structure */
#include "NativeHandle.h"
#include <gui/IGraphicBufferProducer.h>
#include <gui/BufferQueue.h>
#include <gui/BufferQueue.h>
#include <gui/IGraphicBufferConsumer.h>
#include <gui/IConsumerListener.h>
#include <gralloc_priv.h>

//define component names
#if (defined MSM8909 || defined MSM8937)
#define H263_VIDEO_ENCODER       (char*)"OMX.qcom.video.encoder.h263sw\0"
#else
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263\0"
#endif

#define MPEG4_VIDEO_ENCODER (char*)"OMX.qcom.video.encoder.mpeg4\0"
#define H264_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.avc\0"
#define H265_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.hevc\0"

namespace android
{

struct VideoDecBufferInfo {
  IOMX::buffer_id buffer_id;

  int fencefd;
  void *pBuffer;
  size_t alloclen;
  size_t filllen;
  ssize_t offset;
  boolean usedbyIOMX;
};

class Gralloc1Utils
{
  public:
    Gralloc1Utils();
    static Gralloc1Utils* GetInstance() {
      static Gralloc1Utils* instance = new Gralloc1Utils();
      return instance;
    }
    int Retain(private_handle_t *hnd);
    int Release(private_handle_t *hnd);
  private:
    const hw_module_t *module;
    gralloc1_device_t* device;
    GRALLOC1_PFN_RETAIN retain;
    GRALLOC1_PFN_RELEASE release;
};

class RecordingCameraBuffer;
class VideoTransmitter;

class VideoCameraforHal1: public VtCamera
{
  private:
    VideoTransmitter *mTx;

  public:

    VideoCameraforHal1() {
      mTx = NULL;
    };
    virtual ~VideoCameraforHal1() {};
    VideoTransmitter* getClient() {
      return mTx;
    };
    void registerCameraCallbacks(boolean flag);
    static void engine_custom_params_callback(void* params);
    static void engine_camera_notify_callback(int32_t msgType,
        int32_t ext1,
        int32_t ext2,
        void* user);
    static void engine_camera_handle_callback_timestamp(nsecs_t timestamp,
        native_handle_t *handle);

    void setClient(VideoTransmitter *target) {
      mTx = target;
    }
};

class VideoTransmitter: public VideoOmx, public VTRecorder
{

  public:

    sp<RecordingCameraBuffer> mRecordingBuffer;
    VideoTransmitter();
    virtual ~VideoTransmitter();

    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void* pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG* pCodecConfig);
    virtual QPE_VIDEO_ERROR DeInit();
    virtual QPE_VIDEO_ERROR Start();
    virtual QPE_VIDEO_ERROR Stop();
    virtual QPE_VIDEO_ERROR Pause();
    virtual QPE_VIDEO_ERROR Resume();
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig);
    virtual ANativeWindow* getRecordingSurface(JNIEnv * env);
    virtual QPE_VIDEO_ERROR TearDown();
    virtual QPE_VIDEO_ERROR AdaptVideoBitRate(uint32_t iBitRate);


    virtual QPE_VIDEO_ERROR AdaptVideoFrameRate(uint32_t iFrameRate);
    virtual QPE_VIDEO_ERROR SetRotationAngle(uint16_t rotation);
    virtual QPE_VIDEO_ERROR GenerateH264IDRFrame();


    virtual void HandleEvent(void* data);
    QPE_VIDEO_ERROR MovetoLoadedState();
    QPE_VIDEO_ERROR Validate_params(QP_VIDEO_CONFIG& tCodecConfig);
    QPE_VIDEO_ERROR SendEncodedFramesToApp(void* pOmxData);
    QPE_VIDEO_ERROR SendEncodedFramesToAppfromIOMX(
      OMX_TICKS timestamp, uint64_t appsTimestamp,
      VideoDecBufferInfo* pVideoBufferInfo,
      OMX_U32 range_offset, OMX_U32 range_length);
    void QPVIDEOOMXFreeBuffers();
    QPE_VIDEO_ERROR EncoderFreeHandle(void);
    int VerifyFillBuffersDone(void);
    // encoder Helper functions
    void IOMXVideoEncEventHandler(sp<IOMXNode> node, OMX_EVENTTYPE eEvent,
                                  OMX_U32 Data1, OMX_U32 Data2);
    void IOMXVideoEncEmptyBufferDone(sp<IOMXNode> node,
                                     IOMX::buffer_id buffer);
    void IOMXVideoEncFillBufferDone(sp<IOMXNode>  node, IOMX::buffer_id buffer,
                                    OMX_U32 range_offset, OMX_U32 range_length, OMX_U32 flags,
                                    OMX_TICKS timestamp//, OMX_PTR platform_private, OMX_PTR data_ptr,OMX_U32 pmem_offset
                                   );

    //callback to get Frames from camera
    QPE_VIDEO_ERROR WaitTillAllBuffersAreConsumed();
    QPE_VIDEO_ERROR ProcessFramesFromCamera();
    QPE_VIDEO_ERROR ProcessRecordingFrame(uint64_t timestamp, void *dataPtr,
                                          size_t size);
    virtual QPE_VIDEO_ERROR AdaptVideoIdrPeriodicity(int32_t iFrameInt);
    void onMessages(const std::list<omx_message> &messages);
    enum eVideoState getRecorderState() {
      return iRecorderState;
    }

    QPE_VIDEO_BUFFER_MODE getBufferMode() {
      return m_codecConfig.iInputVideoBufferMode;
    }

    uint32_t getFrameRate() {
      return m_codecConfig.iFrameRate;
    }

    /* This API will be called to mark LTR frame.*/
    void ConfigLTRMark(int32_t slLtrIdx);
    /* This API will be called to use LTR frame.*/
    void ConfigLTRUse(int32_t slLtrIdx);
    /* Process ExtraData */
    void ProcessExtraData(uint32_t ulRangeOffset, uint32_t ulRangeLen,
                          void* pFrameBuf, uint32_t* pulLtrExtraData);

    QPE_VIDEO_ERROR ProcessImageFrame(uint64_t  timestamp,
                                      void *dataPtr, size_t size);

    int isCameraResolutionMatching();
    int32_t cameraHeight;
    int32_t cameraWidth;

  protected:

  private:
    //member variables

    QP_VIDEO_CONFIG m_codecConfig;
    enum eVideoState iRecorderState;

    uint8_t m_iMinInputBufCount;
    uint8_t m_iMinOutputBufCount;
    uint64_t m_iBaseTimeStamp;
    uint64_t m_iTimeStamp;
    QPE_VIDEO_CODEC m_iConfiguredCodec;

    volatile OMX_STATETYPE m_IOMXState;
    boolean m_codecConfigured;
    uint32 m_PrevTimeStamp;
    uint8_t m_IDRframeinterval;
    bool m_Metamode;

    uint32 m_LastIDRtimestamp;
    bool m_IDRRetransmitOn;

    OMX_U32 m_OmxFrameFlags;

    //member functions
    QPE_VIDEO_ERROR CameraInit(void* pOmxData);
    QPE_VIDEO_ERROR CameraShutDown();
    QPE_VIDEO_ERROR ConfigureCommonEncoderInputPort(
      QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureCommonEncoderOutputPort(
      QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR EncodeConfigure(
      QP_VIDEO_CONFIG tCodecConfig);
    QPE_VIDEO_ERROR ConfigureH263EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureMPEG4EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureH264EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureH265EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR LoadIOMXEncoderComponents();
    status_t ConfigureBitrateMode();

    QPE_VIDEO_ERROR ProcessRecordingFrame_HAL3(uint64_t  timestamp,
        void *dataPtr, size_t size);


    // This is a thin wrapper class that lets us listen to
// IProducerListener::onBufferReleased from mOutput.

    class InputBufferListener : public BnProducerListener,
      public IBinder::DeathRecipient
    {
      public:
        virtual ~InputBufferListener();
        // From IProducerListener
        virtual void onBufferReleased();
        // From IBinder::DeathRecipient
        virtual void binderDied(const wp<IBinder> &who);

    };



};

class RecordingCameraBuffer : public BufferQueue::ConsumerListener
{

  private:
    VideoTransmitter *mTx;
    virtual void onFrameAvailable(const BufferItem& item);
    virtual void onBuffersReleased();
    virtual void onSidebandStreamChanged();
    uint32_t height;
    uint32_t width;

  public:
    virtual ~RecordingCameraBuffer();
    RecordingCameraBuffer(uint32_t height, uint32_t width) {
      this->height = height;
      this->width = width;
      CRITICAL3("RecordingCameraBuffer Height = %d, width =%d", height, width);
    }
    void createqueuebuffer(uint32_t height, uint32_t width);
    void setClient(VideoTransmitter *target) {
      mTx = target;
    }
    void setHeight(uint32_t height ) {
      this->height = height;
      CRITICAL2("RecordingCameraBuffer height =%d", height);
    }
    void setWidth(uint32_t width ) {
      this->width = width;
      CRITICAL2("RecordingCameraBufferwidth =%d", width);
    }

};

class OmxTransmitterObserver : public BnOMXObserver
{
  private:
    VideoTransmitter *mTarget;
  public:

    OmxTransmitterObserver() {
    }

    void setCodec(VideoTransmitter *target) {
      mTarget = target;
    }
    virtual void onMessages(const std::list<omx_message> &messages) {
      if (mTarget != NULL) {
        mTarget->onMessages(messages);
      }
    }
    virtual void registerBuffers(const sp<IMemoryHeap> & /*mem*/) {}

  protected:
    virtual ~OmxTransmitterObserver() {}


};

}
;

typedef struct QOMX_VIDEO_INTRAPERIODTYPE {
  OMX_U32 nSize;           /** Size of the structure in bytes */
  OMX_VERSIONTYPE nVersion;/** OMX specification version information */
  OMX_U32 nPortIndex;      /** Portindex which is extended by this structure */
  OMX_U32 nIDRPeriod;      /** This specifies coding a frame as IDR after every nPFrames
                                of intra frames. If this parameter is set to 0, only the
                                first frame of the encode session is an IDR frame. This
                                field is ignored for non-AVC codecs and is used only for
                                codecs that support IDR Period */
  OMX_U32 nPFrames;         /** The number of "P" frames between two "I" frames */
  OMX_U32 nBFrames;         /** The number of "B" frames between two "I" frames */
} QOMX_VIDEO_INTRAPERIODTYPE;



/*"OMX.QCOM.index.config.intraperiod" */
#define QOMX_IndexConfigVideoIntraperiod 0x7F000007

/*"OMX.QCOM.index.param.video.QPRange" */
#define OMX_QcomIndexParamVideoQPRange 0x7F000022

/*"OMX.QCOM.index.param.video.LTRCount"*/
#define QOMX_IndexParamVideoLTRCount 0x7F000026

/*"OMX.QCOM.index.config.video.LTRUse"*/
#define QOMX_IndexConfigVideoLTRUse 0x7F000028

/*"OMX.QCOM.index.config.video.LTRMark"*/
#define QOMX_IndexConfigVideoLTRMark 0x7F000029

/* OMX.google.android.index.prependSPSPPSToIDRFrames */
#define OMX_QcomIndexParamSequenceHeaderWithIDR 0x7F00002A

/*"OMX.google.android.index.storeMetaDataInBuffers"*/
#define OMX_QcomIndexParamVideoMetaBufferMode 0x7F00001C

#define OMX_QcomIndexParamIndexExtraDataType 0x7F000016

typedef struct QOMX_INDEXEXTRADATATYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_BOOL bEnabled;
  OMX_INDEXTYPE nIndex;
} QOMX_INDEXEXTRADATATYPE;

/**
 * AVC profile types, each profile indicates support for various
 * performance bounds and different annexes.
 */
typedef enum QOMX_VIDEO_AVCPROFILETYPE {
  QOMX_VIDEO_AVCProfileBaseline      = OMX_VIDEO_AVCProfileBaseline,
  QOMX_VIDEO_AVCProfileMain          = OMX_VIDEO_AVCProfileMain,
  QOMX_VIDEO_AVCProfileExtended      = OMX_VIDEO_AVCProfileExtended,
  QOMX_VIDEO_AVCProfileHigh          = OMX_VIDEO_AVCProfileHigh,
  QOMX_VIDEO_AVCProfileHigh10        = OMX_VIDEO_AVCProfileHigh10,
  QOMX_VIDEO_AVCProfileHigh422       = OMX_VIDEO_AVCProfileHigh422,
  QOMX_VIDEO_AVCProfileHigh444       = OMX_VIDEO_AVCProfileHigh444,
  QOMX_VIDEO_AVCProfileConstrainedBaseline = OMX_VIDEO_AVCProfileConstrainedBaseline,
  QOMX_VIDEO_AVCProfileConstrainedHigh     = OMX_VIDEO_AVCProfileConstrainedHigh,
  /* QCom specific profile indexes */
  QOMX_VIDEO_AVCProfileConstrained         = OMX_VIDEO_AVCProfileVendorStartUnused
} QOMX_VIDEO_AVCPROFILETYPE;

typedef struct OMX_QCOM_VIDEO_PARAM_QPRANGETYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_U32 minQP;
  OMX_U32 maxQP;
} OMX_QCOM_VIDEO_PARAM_QPRANGETYPE;

typedef enum OMX_QCOM_EXTRADATATYPE {
  OMX_ExtraDataFrameInfo =       0x7F000001,
  OMX_ExtraDataH264 =          0x7F000002,
  OMX_ExtraDataVC1 =           0x7F000003,
  OMX_ExtraDataFrameDimension =      0x7F000004,
  OMX_ExtraDataVideoEncoderSliceInfo = 0x7F000005,
  OMX_ExtraDataConcealMB =       0x7F000006,
  OMX_ExtraDataInterlaceFormat =     0x7F000007,
  OMX_ExtraDataPortDef =         0x7F000008,
  OMX_ExtraDataMP2ExtnData =       0x7F000009,
  OMX_ExtraDataMP2UserData =       0x7F00000a,
  OMX_ExtraDataVideoLTRInfo =      0x7F00000b,
  OMX_ExtraDataFramePackingArrangement = 0x7F00000c,
  OMX_ExtraDataQP =            0x7F00000d,
  OMX_ExtraDataInputBitsInfo =     0x7F00000e,
  OMX_ExtraDataVideoEncoderMBInfo =    0x7F00000f,
  OMX_ExtraDataVQZipSEI  =       0x7F000010,
  OMX_ExtraDataDisplayColourSEI =    0x7F000011,
  OMX_ExtraDataLightLevelSEI =     0x7F000012,
  OMX_ExtraDataOutputCropInfo =      0x7F000014,
  OMX_ExtraDataInputROIInfo =      0x7F000058,
} OMX_QCOM_EXTRADATATYPE;

/**
 * Marks the next encoded frame as an LTR frame.
 * STRUCT MEMBERS:
 *  nSize              : Size of Structure in bytes
 *  nVersion           : OpenMAX IL specification version information
 *  nPortIndex         : Index of the port to which this structure applies
 *  nID                : Specifies the identifier of the LTR frame to be marked
 *                       as reference frame for encoding subsequent frames.
 */
typedef struct QOMX_VIDEO_CONFIG_LTRMARK_TYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_U32 nID;
} QOMX_VIDEO_CONFIG_LTRMARK_TYPE;

/**
 * Specifies an LTR frame to encode subsequent frames.
 * STRUCT MEMBERS:
 *  nSize              : Size of Structure in bytes
 *  nVersion           : OpenMAX IL specification version information
 *  nPortIndex         : Index of the port to which this structure applies
 *  nID                : Specifies the identifier of the LTR frame to be used
                         as reference frame for encoding subsequent frames.
 *  nFrames            : Specifies the number of subsequent frames to be
                         encoded using the LTR frame with its identifier
                         nID as reference frame. Short-term reference frames
                         will be used thereafter. The value of 0xFFFFFFFF
                         indicates that all subsequent frames will be
                         encodedusing this LTR frame as reference frame.
 */
typedef struct QOMX_VIDEO_CONFIG_LTRUSE_TYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_U32 nID;
  OMX_U32 nFrames;
} QOMX_VIDEO_CONFIG_LTRUSE_TYPE;


/**
 * LTR count index parameter.  This structure is used
 * to enable vendor specific extension on output port
 * to pass the LTR count information.
 *
 * STRUCT MEMBERS:
 *  nSize              : Size of Structure in bytes
 *  nVersion           : OpenMAX IL specification version information
 *  nPortIndex         : Index of the port to which this structure applies
 *  nCount             : Specifies the number of LTR frames stored in the
 *                       encoder component
 */
typedef struct QOMX_VIDEO_PARAM_LTRCOUNT_TYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_U32 nCount;
} QOMX_VIDEO_PARAM_LTRCOUNT_TYPE;

#endif //  __VIDEOTRANSMITTER_H__
