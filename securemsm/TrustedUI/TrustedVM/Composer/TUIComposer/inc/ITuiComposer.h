/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __TUI_COMPOSER_H__
#define __TUI_COMPOSER_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
/* Supported composer types */
typedef enum tuiComposerType {
    /**
     * Trusted UI dialog composer.
     *
     * Default composer for Trusted UI Solution.
     * Layout is generated using offline tool and the selected layout is
     * rendered using layout manager.
     */
    TUI_COMPOSER_TYPE_DIALOG = 0x1,

    /**
     * Custom composer.
     *
     * This custom composer draws a dot on each selected input co-ordinate.
     * This is added as a reference for plugging-in custom composers as per
     * TrustedUI client needs.
     */
    TUI_COMPOSER_TYPE_CUSTOM = 0x10000,

    TUI_COMPOSER_TYPE_MAX
} tuiComposerType_t;

/* Composer status */
typedef enum tuiComposerStatus {
    TUI_COMPOSER_STATUS_OK = 0x0,
    TUI_COMPOSER_STATUS_FAILED = 0x1,
    TUI_COMPOSER_STATUS_BAD_PARAM = 0x2,
    TUI_COMPOSER_STATUS_NO_MEM = 0x3,
    TUI_COMPOSER_STATUS_NO_INIT = 0x4,
    TUI_COMPOSER_STATUS_INVALID_LAYOUT = 0x5,
} tuiComposerStatus_t;

/* Graphics formats supported by composer */
typedef enum tuiComposerFormat {
    TUI_COMPOSER_FORMAT_RGBA = 0x1,
    TUI_COMPOSER_FORMAT_MAX
} tuiComposerFormat_t;

/* Composer configuration parameters */
typedef struct tuiComposerParam {
    /* Layout identifier */
    char *layoutId;

    /* Display width */
    uint32_t width;

    /* Display height */
    uint32_t height;

    /* Secure Indicator */
    uint32_t useSecureIndicator;

    /* Graphics format to be used for rendering */
    tuiComposerFormat_t format;
} tuiComposerParam_t;

/* Input types supported */
typedef enum tuiComposerInputType {
    TUI_COMPOSER_INPUT_TOUCH = 0x1,
    TUI_COMPOSER_INPUT_MAX
} tuiComposerInputType_t;

typedef enum tuiComposerInputCommand {
    /**
     * Command used to prepare init screen, this screen will be presented
     * as 1st frame on display (post successful aquisition of secureInput).
     */
    TUI_COMPOSER_INPUT_CMD_START = 0x1,
    /**
     * Command used to prepare termination screen, this screen will be presented
     * as last secure display frame on display (before starting termination of
     * secureInput).
     */
    TUI_COMPOSER_INPUT_CMD_STOP = 0x2,
    /**
     * Command used to prepare next screen based on user input data.
     */
    TUI_COMPOSER_INPUT_CMD_DATA = 0x3,
    /**
     * Command used to prepare next screen based on timeout for user input.
     */
    TUI_COMPOSER_INPUT_CMD_TIMEOUT = 0x4,
    /**
     * Command limit.
     */
    TUI_COMPOSER_INPUT_CMD_MAX = TUI_COMPOSER_INPUT_CMD_TIMEOUT,
} tuiComposerInputCommand_t;

typedef enum tuiComposerInputStatus {
    /**
     * Valid input received by composer, graphics buffer is composed with
     * the updated layout for presentation.
     */
    TUI_COMPOSER_INPUT_STATUS_CONTINUE = 0x1,

    /**
     * Invalid input received by composer, graphics buffer is not composed.
     * Session is expected to be terminated.
     */
    TUI_COMPOSER_INPUT_STATUS_CANCEL = 0x2,

    /**
     * Layout presentation is complete, no new layout to be presented.
     * Session is expected to be terminated.
     */
    TUI_COMPOSER_INPUT_STATUS_COMPLETE = 0x3,

    /**
     * Status limit.
     */
    TUI_COMPOSER_INPUT_STATUS_MAX = TUI_COMPOSER_INPUT_STATUS_COMPLETE,
} tuiComposerInputStatus_t;

#define TLAPI_TOUCH_EVENT_NONE  0
/** Down touch event. */
#define TLAPI_TOUCH_EVENT_DOWN  1
/** Move touch event. */
#define TLAPI_TOUCH_EVENT_MOVE  2
/** Up touch event. */
#define TLAPI_TOUCH_EVENT_UP    4

#define TUI_CMD_GET_STATUS 20

#define SECUIEID_ENTER_PRESSED   1
#define SECUIEID_CANCEL_PRESSED  0
#define SECUIEID_GENERAL_ERROR   -1

/* Max finger data support at a given instance for touch input */
#define MAX_FINGER_NUM 10
typedef struct tuiComposerTouchData {
    uint32_t event; /* 0: None, 1: Down, 2: Move, 4: Up */

    /* Touch X CoOrdinate */
    uint32_t xCoOrdinate;

    /* Touch Y CoOrdinate */
    uint32_t yCoOrdinate;

    /* Timestamp - unused */
    uint64_t timestamp;

    /* Touch CoOrdinate updated */
    uint32_t dirty;
} tuiComposerTouchData_t;

/* Composer touch input defines */
typedef struct tuiComposerTouchInput {
    tuiComposerTouchData_t finger[MAX_FINGER_NUM];
} tuiComposerTouchInput_t;

/**
 * Composer input defines
 * Currently, touch based input is supported.
 * But it can be extended to keypad based input or other supported input types.
 */
typedef struct tuiComposerInput {
    /* Input type */
    tuiComposerInputType_t type;

    /* Input Command */
    tuiComposerInputCommand_t cmd;

    /* Input Status based on the input command processing against selected layout */
    tuiComposerInputStatus_t status;

    /* Input timeout limit: It is used as both input and output */
    int32_t timeOut;

    /* Input data corresponding to input type */
    union {
        tuiComposerTouchInput_t touch;
    } data;
} tuiComposerInput_t;

/* Buffer related defines */
typedef struct tuiComposerBufferHandle {
    uint8_t *vAddr;
    uint32_t size;
} tuiComposerBufferHandle_t;

/* Display configuration associated with graphics buffer */
typedef struct tuiComposerDisplayConfig {
    /* Width */
    uint32_t width;

    /* Stride */
    uint32_t stride;

    /* Height */
    uint32_t height;

    /* Rendering Format */
    tuiComposerFormat_t format;
} tuiComposerDisplayConfig_t;

/* Display buffer structure */
typedef struct tuiComposerDisplayBuffer {
    /* Configuration */
    tuiComposerDisplayConfig_t config;

    /* Buffer handle */
    tuiComposerBufferHandle_t handle;
} tuiComposerDisplayBuffer_t;

/* Custom command payload define */
typedef struct tuiComposerCmdPayload {
    void *cmdPtr;
    uint32_t cmdSize;
    void *respPtr;
    uint32_t respSize;
} tuiComposerCmdPayload_t;

/**
 * ITuiComposerCallback:
 *
 * ITuiComposerCallback is callback interface expected to be implemented
 * by caller of ITuiComposer interface.
 * It allows ITuiComposer to request for helper buffer allocation
 * (if needed for rendering/composition operation).
 *
 */
typedef struct ITuiComposerCallback {
    /**
     * Callback data:
     *
     * Data associated with callback interface.
     * It is expected to be passed in each callback function.
     */
    void *cbData;

    /**
     * allocHelperBuff:
     *
     * Allocate helper buffer for requested size (in bytes).
     *
     * @param cbData ComposerCallback data.
     * @param buffer Buffer handle with requested size {@link
     * tuiComposerBufferHandle_t}.
     *
     * @return status Return status of this operation:
     *     TUI_COMPOSER_STATUS_OK:
     *         Allocation successfull.
     *     TUI_COMPOSER_STATUS_BAD_PARAM:
     *         Invalid parameter.
     *     TUI_COMPOSER_STATUS_NO_MEM:
     *         Allocation failed due to low memory.
     *     TUI_COMPOSER_STATUS_FAILED:
     *         Operation failed due to generic error.
     *
     */
    tuiComposerStatus_t (*allocHelperBuff)(void *cbData,
                                           tuiComposerBufferHandle_t *buffer);

    /**
     * freeHelperBuff:
     *
     * Free already allocated helper buffer.
     *
     * @param cbData ComposerCallback data.
     * @param buffer Buffer handle {@link tuiComposerBufferHandle_t}.
     *
     * @return status Return status of this operation:
     *     TUI_COMPOSER_STATUS_OK:
     *         Allocation successfull.
     *     TUI_COMPOSER_STATUS_BAD_PARAM:
     *         Invalid parameter.
     *     TUI_COMPOSER_STATUS_FAILED:
     *         Operation failed due to generic error.
     *
     */
    tuiComposerStatus_t (*freeHelperBuff)(void *cbData,
                                          tuiComposerBufferHandle_t *buffer);

    /**
     * handleCmd:
     *
     * Handle command which are from font manager.
     *
     * @param cmd_ptr    Command data.
     * @param cmd_len    Command length.
     * @param rsp_ptr    Response data.
     * @param rsp_len    Max length of response data.
     * @param rsp_lenout Actual length of response data.
     *
     * @return status Return status of this operation:
     *        0:
     *          Allocation successfull.
     *       -1:
     *          Invalid parameter or the state is aborting.
     *     < -1:
     *          Operation failed due to generic error.
     *
     */

    int32_t (*handleCmd)(const void* cmd_ptr,
                       size_t cmd_len,
                       void* rsp_ptr,
                       size_t rsp_len,
                       size_t* rsp_lenout);

} ITuiComposerCallback_t;

/**
 * ITuiComposer:
 *
 * TuiComposer public interface for secure graphics content
 * rendering/composition.
 *
 */
typedef struct ITuiComposer {
    /**
     * Transparent composer context pointer.
     * It is expected to be passed in all composer interface API calls.
     */
    void *ctx;

    /**
     * Type of concrete composer instantiated.
     */
    tuiComposerType_t type;

    /**
     * init:
     *
     * Initialize composer for requested layout.
     *
     * @param ctx Composer context.
     * @param param Composer init params {@link tuiComposerParam_t}.
     *
     * @return status Return status of this operation:
     *     TUI_COMPOSER_STATUS_OK:
     *         Init completed.
     *     TUI_COMPOSER_STATUS_BAD_PARAM:
     *         Invalid parameter.
     *     TUI_COMPOSER_STATUS_FAILED:
     *         Init failed.
     *
     */
    tuiComposerStatus_t (*init)(void *ctx, tuiComposerParam_t *param,
                                ITuiComposerCallback_t *cb);

    /**
     * compose:
     *
     * Process user input and compose a graphics buffer in accordance with
     * selected layout to be presented on display.
     *
     * @param ctx Composer context.
     * @param input User input {@link tuiComposerInput_t}.
     * @param buffer Display buffer to render into {@link
     * tuiComposerDisplayBuffer_t}.
     *
     * @return status Return status of this operation:
     *     TUI_COMPOSER_STATUS_OK:
     *         Init completed.
     *     TUI_COMPOSER_STATUS_BAD_PARAM:
     *         Invalid parameter.
     *     TUI_COMPOSER_STATUS_FAILED:
     *         Init failed.
     *
     */
    tuiComposerStatus_t (*compose)(void *ctx, tuiComposerInput_t *input,
                                   tuiComposerDisplayBuffer_t *buffer);

    /**
     * sendCommand:
     *
     * Send custom command to composer.
     *
     * @param ctx Composer context.
     * @param cmd Command identifier.
     * @param ctx Command payload structure.
     *
     * @return result Session result.
     *
     */
    tuiComposerStatus_t (*sendCommand)(void *ctx, uint32_t cmd,
                                       tuiComposerCmdPayload_t *payload);

    /**
     * deInit:
     *
     * De-initialise composer.
     *
     * @param ctx Composer context.
     *
     * @return status Return status of this operation:
     *     TUI_COMPOSER_STATUS_OK:
     *         deInit completed.
     *     TUI_COMPOSER_STATUS_BAD_PARAM:
     *         Invalid parameter.
     *     TUI_COMPOSER_STATUS_FAILED:
     *         deInit failed.
     *
     */
    tuiComposerStatus_t (*deInit)(void *ctx);

} ITuiComposer_t;

/* Factory methods to instantiate and destroy Composer objects. */

/**
 * createTUIComposer:
 *
 * Create a new composer object (of requested type) for secure graphics
 * content rendering/composition.
 *
 * @param type Composer type {@link tuiComposerType_t}.
 *
 * @return object Newly created composer object.
 *
 */
ITuiComposer_t *createTUIComposer(tuiComposerType_t type);

/**
 * destroyTUIComposer:
 *
 * Destroy an already created composer object.
 *
 * @param composer Composer object.
 *
 */
void destroyTUIComposer(ITuiComposer_t *composer);

#ifdef __cplusplus
}
#endif

#endif //__TUI_COMPOSER_H__
