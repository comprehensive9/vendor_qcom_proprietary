/*===========================================================================
  Copyright (c) 2013-2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#ifndef SEC_RENDERER_H_
#define SEC_RENDERER_H_

/** @file secure_display_renderer.h
 * @brief
 * This file contains the interfaces to the layout renderer for secure display.
 */


#include "SecureTouchLayout.h"
#include "layout_manager_internal.h"

typedef enum {
    SEC_RENDER_SUCCESS = 0,
    SEC_RENDER_GENERAL_ERROR = -1,
    SEC_RENDER_UNSUPPORTED = -2,
    SEC_RENDER_STRING_TOO_BIG = -3,
    SEC_RENDER_ALLOCATION_ERROR = -4,
    SEC_RENDER_ERROR_PNG_SIG = -5,
    SEC_RENDER_ERROR_PNG_INFO = -6,
    SEC_RENDER_ERROR_FILL_EXCEED = -7,
    SEC_RENDER_ERROR_NULL_INPUT = -8,
    SEC_RENDER_GD_IMAGE_ERROR = -9,
    SEC_RENDER_GOT_BAD_INPUT = -10,
    SEC_RENDER_UNCHANGED = -11,
    SEC_RENDER_FONT_ERROR = -12,
    SEC_RENDER_BAD_STRING = -13,

    SEC_RENDER_ERR_SIZE = 0x7FFFFFFF
} sec_render_err_t;


/***********************************************************************************/

/**
  @brief
  External function that goes over a layout and renders the layout objects into
  the given secure buffer,
  each object will be rendered only if dirty bit is on.
  */

sec_render_err_t render_layout(LayoutPage_t* layout,
                               const screen_surface_t* surface);

/**************************************************************************************/

/**
  @brief
  External function for calculating utf-8 string length in bytes.
  */
sec_render_err_t get_utf8_len(const uint8_t* utf8_string, uint32_t num_chars,
                              uint32_t* length, uint32_t* last_char_index);

/***************************************************************************************/

/**
  @brief
  External function for rendering the current layout bg image to an external
  buffer. This is an optimization
  to allow faster rendering time of transparent objects.
  */
sec_render_err_t render_bg_image(LayoutPage_t* layout,
                                 const screen_surface_t* surface);

/**
  @brief
  External function to release the current layout's bg image
  rendered previously using render_bg_image.
  */
void release_bg_image();

/*****************************************************************************************/

/**
  @brief
  External function for checking if rendering a string is possible in some
  object.
  In case it's not, return the length of substring that fits it.
  */
sec_render_err_t check_render_text_object(
    LayoutObject_t obj, const uint8_t* text, uint32_t len, uint8_t check_hint,
    uint32_t* fitting_len, uint32_t* missing_char, uint32_t* len_char);
/*****************************************************************************************/

/**
  @brief
  External function for checking if rendering an image is possible in some
  object.
  In any case it returns also the size of the image in pixels.
  */
sec_render_err_t check_render_image_object(LayoutObject_t obj,
                                           const uint8_t* image_buffer,
                                           uint32_t* imWidth,
                                           uint32_t* imHeight);

#endif /* SEC_RENDERER_H_ */
