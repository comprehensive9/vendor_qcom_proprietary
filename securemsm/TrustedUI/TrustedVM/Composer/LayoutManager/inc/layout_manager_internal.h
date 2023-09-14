/*===========================================================================
  Copyright (c) 2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#ifndef LAYOUT_MGR_INTERNAL_H_
#define LAYOUT_MGR_INTERNAL_H_

/** @file layout_manager_internal.h
 * @brief
 * This file contains the interfaces to the secure ui layout manager that we
 * don't ship out.
 */


/*===========================================================================

  EDIT HISTORY FOR FILE

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/07/16   dr      Add orientation support and move to use the display_manager
  05/22/16   sn      Modified the evaluation function to return also the event
  type of last touched object.
  04/11/16   sn      Initial Version.

  ===========================================================================*/
#include "SecureTouchLayout.h"
#include "layout_manager.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum { TIMEOUT_EVENT, TOUCH_EVENT, EVENT_SIZE = 0x7FFFFFFF } event_t;

//TODO: Where to add this define
typedef struct screen_surface {
    uint32_t buffer_size;
    uint8_t* buffer;
    uint32_t width;
    uint32_t height;
    uint32_t pixelWidth;
} screen_surface_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/******************************************************************************************/

/**
  @brief
  External function that updates the layout following a given touch/timeout
  event.
  Outputs the current focused input object name, and the required timeout (in
  milliseconds) for the next timeout event;
  timeout 0 means return immediately, and timeout -1 means no timeout at all.

  @param[in] event - The  received touch/timeout event
  @param[in] eventParams - Event parameters, for instance the touch event
  location parameters.
  @param[out] touchedObject - The current focused input object to be returned
  @param[in] timeout - Timeout for the screen, timemeout 0 means return
  immediately, and timeout -1 (NO_TIMEOUT) means no timeout at all.

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_UNEXPECTED_TIMEOUT - Unexepected timeout event
  LAYOUT_MGR_INVALID_UTF8_STR - Invalid focusedInput labels
  LAYOUT_MGR_TEXT_EXCEED_BUFFER - Added data overflows the buffer
  LAYOUT_MGR_UNSUPPORTED - Unsupported timeout operation

*/
layout_mgr_err_t layout_mgr_eval_event(event_t event, void* eventParams,
                                       layout_mgr_touched_obj_t* touchedObject,
                                       int32_t* timeout);

/*************************************************************************************/

/**
  @brief
  External function that wraps a call to the renderer to render the current
  layout to the given surface.

  @param[in] surface - screen surface containing the buffer which will be passed
  to the renderer

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_UNCHANGED
  LAYOUT_MGR_TEXT_EXCEED_OBJECT
  LAYOUT_MGR_BAD_LAYOUT
  */
layout_mgr_err_t layout_mgr_render_layout(const screen_surface_t* surface);

/********************************************************************************/

/**
  @brief
  External function for getting the length of the text label of some input
  object in the layout.
  The length is returned in bytes and also in characters.

  @param[in] objectName - Object name (as unique identifier in the layout)
  @param[out] chars_len - utf-8 character length of the text label
  @param[out] bytes_len - Bytes length of the text label

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_OBJECT_NOT_FOUND
  LAYOUT_MGR_INVALID_UTF8_STR
  */
layout_mgr_err_t layout_mgr_get_input_len(const char* objectName,
                                          uint32_t* chars_len,
                                          uint32_t* bytes_len);

/************************************************************************************/

/**
  @brief
  External function for setting the state of some button object in the layout.

  @param[in] objectName - The object name (as unique identifier in the layout)
  @param[in] disabled - The state of the button to be set

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_OBJECT_NOT_FOUND
  LAYOUT_MGR_UNSUPPORTED
  LAYOUT_MGR_BUTTON_BUSY
  */
layout_mgr_err_t layout_mgr_set_button_disabled(const char* objectName,
                                                uint32_t disabled);

/************************************************************************************/
/**
  @brief
  External function that wraps a call to the renderer to render the bg of the
  current layout,
  to the given surface.

  @param[in] surface - The given surface to render the layout bg to

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_BAD_LAYOUT
  */
layout_mgr_err_t layout_mgr_render_bg_image(const screen_surface_t* surface);

/**********************************************************************************/
/**
  @brief
  External function to release the previously rendered bg of the current layout.
  */
void layout_mgr_release_bg_image();

/**********************************************************************************/

/**
  @brief
  Validates layout size can fit current screen resolution.
  The caller must provide the dimensions according to the desired screen
  orientation

  @param[in] screen_width - screen width (horizontal) in pixels
  @param[in] screen_width - screen height (vertical) in pixels

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_BAD_LAYOUT
  */
layout_mgr_err_t layout_mgr_validate_layout(uint32_t screen_width,
                                            uint32_t screen_height);

/***********************************************************************************/
/**
  @brief
  It populates the layout background color (Color_t) in param bgColor.

  @param[in] bgColor - bg color of current layout

  @return
  LAYOUT_MGR_SUCCESS
  LAYOUT_MGR_GOT_NULL_INPUT
  LAYOUT_MGR_LAYOUT_NOT_LOADED
  */
layout_mgr_err_t layout_mgr_get_layout_bgColor(Color_t* bgColor);

#endif /* LAYOUT_MGR_INTERNAL_H_ */
