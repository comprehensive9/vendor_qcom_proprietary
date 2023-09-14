/**============================================================================
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================*/

#ifndef _TUIAPI_H_
#define _TUIAPI_H_

#include "TUIInputTypes.h" /* For TUIInputData */
#include "TUIErrorCodes.h" /* For error codes */
#include "object.h"        /* Mink object */
/**----------------------------------------------------------------------------
 * @title TUIAPI.h
 * @brief APIs used by TUI Applications to communicate with TUI framework
 *        for achieving TUI functionality
-----------------------------------------------------------------------------*/

/**----------------------------------------------------------------------------
  @title  TUISessionConfig
  @brief  Session configuration parameters
-----------------------------------------------------------------------------*/
typedef struct TUISessionConfig {
  uint32_t displayId;          /* Display device identifier */
  uint32_t numDisplayBuffers;  /* Number of Display buffers needed for TUISession */
  uint32_t colorFormat;
  uint32_t inputUID;           /* Input device identifier */
  uint32_t useCaseHeapSize;    /* Heap Size requirement for TUI use case */
  Object   appCBO;             /* HLOS Callback object, used only by QTEE Apps */
};

/**----------------------------------------------------------------------------
  @title  TUIDisplayConfig
  @brief  Display configuration parameters used during TUI image composition
-----------------------------------------------------------------------------*/
typedef struct TUIDisplayConfig {
  uint32_t height;      /* Display resolution height*/
  uint32_t width;       /* Display resolution width */
  uint32_t stride;      /* Display width with padding */
  uint32_t colorFormat; /* color format used for composing ex: RGB*/
  uint32_t offHeight;   /* Height offset for the Image */
  uint32_t offWidth;    /* Width offset for the Image */
};

/**----------------------------------------------------------------------------
  @title  TUIBufferHandle
  @brief  Buffer address and size
-----------------------------------------------------------------------------*/
typedef struct TUIBufferHandle {
  uint8_t *vAddr;   /* virtual address */
  uint32_t size;    /* size of the buffer */
};

/**----------------------------------------------------------------------------
  @title  TUIDisplayBuffer
  @brief  Display buffer
-----------------------------------------------------------------------------*/
typedef struct TUIDisplayBuffer {
  TUIDisplayConfig config;       /* display config for the buffer */
  TUIBufferHandle  buffHandle;   /* buffer address */
};

/**----------------------------------------------------------------------------
  @title  TUICreateSession
  @brief  Establishes connection with TUI framework, and allows application
          to acquire and use Display and Input peripherals. On successful session
          creation, a non-zero sessionId is returned. All other session APIs
          must pass this sessionId as the input.
  @param  in TUISessionConfig config Display and Input configuration
  @param  out uint32_t sessionId session identifier
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUICreateSession(const TUISessionConfig *config, uint32_t* sessionId);

/**----------------------------------------------------------------------------
  @title  TUIDeleteSession
  @brief  Tears down connection with TUI framework
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIDeleteSession(uint32_t sessionId);

/**----------------------------------------------------------------------------
  @title  TUIStartSession
  @brief  Initiates a Trusted UI session by claiming ownership of Display and
          Input peripherals. On successful return of this method, the
          initScreenBuffer will be displayed on the screen.
          After TUIStartSession() completes successfully, Apps can dequeue
          secure buffers, get user input using TUIGetInputData mehtod
          and render it using TUIEnqueueDisplayBuffer
  @param  in TUIDisplayBuffer* initScreenBuffer
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIStartSession(uint32_t sessionId,
                                        TUIDisplayBuffer* initScreenBuffer);

/**----------------------------------------------------------------------------
  @title  TUIStopSession
  @brief  Terminates a Trusted UI session by releasing ownership of Display and
          Input peripherals
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIStopSession(uint32_t sessionId);

/**----------------------------------------------------------------------------
  @title  TUIGetInputData
  @brief  Applications use this method to get the next available input data
  @param  in uint32_t sessionId
  @param  in int32_t inputTimeout time in milliseconds after which input wait
                     will stop
  @param  out TUIInputData inputData
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIGetInputData(uint32_t sessionId, int32_t inputTimeout,
                          TUIInputData* inputData);

/**----------------------------------------------------------------------------
  @title  TUIGetDisplayProperties
  @brief  Allows applications to get Display configuration for a specific display
          device
          Note: This method should be called after creating a TUI Session
  @param  out TUIDisplayConfig
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIGetDisplayProperties(uint32_t sessionId, TUIDisplayConfig *cfgInfo);

/**----------------------------------------------------------------------------
  @title  TUIEnqueueBuffer
  @brief  This method takes a buffer address as input and renders it on the
          Display. It also validates that the buffer is a secure buffer and
          belongs to this session. On successful validation, it is enqueued to
          display.
  @param  in TUIDisplayBuffer buffer to be rendered
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIEnqueueDisplayBuffer(uint32_t sessionId,
                                  TUIDisplayBuffer *buffer);

/**----------------------------------------------------------------------------
  @title  TUIDequeueBuffer
  @brief  This method provides a display buffer for applications to compose into
          Default mapping is cached
  @param  in uint32_t sessionId
  @param  out TUIDisplayBuffer buffer dequeued
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIDequeueDisplayBuffer(uint32_t sessionId,
                                  TUIDisplayBuffer *buffer);

/**----------------------------------------------------------------------------
  @title  TUIAllocateAppBuffer
  @brief  Allocates a secure buffer for the application, Apps can use these
          as scratch buffers during image composition.
          These buffers do not qualify to be used as Display buffers as a input
          to TUIEnqueueDisplayBuffer.
  @param  in TUIBufferHandle* Requested buffer size
  @param  out TUIBufferHandle* Size and address after buffer allocation
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIAllocateAppBuffer(uint32_t sessionId,
                                TUIBufferHandle* buffer);

/**----------------------------------------------------------------------------
  @title  TUIFreeAppBuffer
  @brief  This method frees a App buffer
  @param  in TUIBufferHandle buffer to be freed
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIFreeAppBuffer(uint32_t sessionId,
                            TUIBufferHandle* buffer);

#endif /* _TUIAPI_H_ */
