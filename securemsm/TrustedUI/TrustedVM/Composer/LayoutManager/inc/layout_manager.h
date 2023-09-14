/*===========================================================================
  Copyright (c) 2013-2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#ifndef LAYOUT_MGR_H_
#define LAYOUT_MGR_H_

/** @file layout_manager.h
  @brief
  This file contains the interfaces to the Secure UI Layout Manager.
  */



#include "SecureTouchLayout.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** @addtogroup layout_manager
  @{ */

/** Layout manager return codes. */
typedef enum {
    LAYOUT_MGR_SUCCESS = 0,
    /**< Operation was successful. */
    LAYOUT_MGR_GENERAL_ERROR = -1,
    /**< General error.  */
    LAYOUT_MGR_BAD_LAYOUT = -2,
    /**< Received an invalid layout structure. */
    LAYOUT_MGR_GOT_NULL_INPUT = -3,
    /**< Received a null parameter. */
    LAYOUT_MGR_UNSUPPORTED = -4,
    /**< Operation is not supported with the specified parameters. */
    LAYOUT_MGR_OBJECT_NOT_FOUND = -5,
    /**< Specified object does not exist in the layout.  */
    LAYOUT_MGR_TEXT_EXCEED_BUFFER = -6,
    /**< Received text exceeds the given buffer size. */
    LAYOUT_MGR_UNEXPECTED_TIMEOUT = -7,
    /**< Unexpected timeout event occurred.  */
    LAYOUT_MGR_BUTTON_BUSY = -8,
    /**< Button is currently in the PRESSED state, so it cannot be disabled. */
    LAYOUT_MGR_FAILED_GEN_RAND = -9,
    /**< Random number generation failed. */
    LAYOUT_MGR_INVALID_UTF8_STR = -10,
    /**< Received an invalid UTF8 string. */
    LAYOUT_MGR_TEXT_EXCEED_OBJECT = -11,
    /**< Text exceeds the object size limitations.  */
    LAYOUT_MGR_UNCHANGED = -12,
    /**< No change occurred in the layout. */
    LAYOUT_MGR_LAYOUT_NOT_LOADED = -13,
    /**< Layout is not loaded; layout_mgr_load_layout() must be called
     before  calling this function. */
    LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED = -14,
    /**< Object is not the required type. */
    LAYOUT_MGR_FONT_MISSING_CHAR = -15,
    /**< Font is missing a character. */
    LAYOUT_MGR_DYNAMIC_FONT_ERROR = -16,
    /**< Dynamic font error. */
    LAYOUT_MGR_MEM_ALLOC_ERROR = -17,
    /**< Memory allocation error. */
    LAYOUT_MGR_IMAGE_EXCEED_OBJECT = -18,
    /**< Image exceeded the object. */
    LAYOUT_MGR_INVALID_IMAGE = -19,
    /**< Image has an unsupported format or is corrupted. */

    LAYOUT_MGR_ENTER_PRESSED = 1,
    /**< Function button Enter was pressed. */
    LAYOUT_MGR_CANCEL_PRESSED = 2,
    /**< Function button Cancel was pressed. */
    LAYOUT_MGR_F1_PRESSED = 3,
    /**< Function button F1 was pressed. */
    LAYOUT_MGR_F2_PRESSED = 4,
    /**< Function button F2 was pressed. */
    LAYOUT_MGR_F3_PRESSED = 5,
    /**< Function button F3 was pressed.  */
    /** @cond */
    LAYOUT_MGR_ERR_SIZE = 0x7FFFFFFF
    /**< @endcond */
} layout_mgr_err_t;

typedef enum {
    LAYOUT_MGR_EVENT_NONE = 0,
    /**< No event. */
    LAYOUT_MGR_EVENT_UP = 1,
    /**< UP touch event. */
    LAYOUT_MGR_EVENT_DOWN = 2,
    /**< DOWN touch event. */
    LAYOUT_MGR_EVENT_MOVE = 3,
    /**< MOVE touch event. @newpage */
    /** @cond */
    LAYOUT_MGR_EVENT_SIZE = 0x7FFFFFFF
    /**< @endcond */
} layout_mgr_event_t;

/** Defines the currently touched object in the screen. */
typedef struct {
    const char* objectName;       /**< Object name. */
    layout_mgr_event_t eventType; /**< Event type. */
} layout_mgr_touched_obj_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  External function that loads a layout struct. This function must be called
  before using most other functions in the Layout Manager API.

  @param[in] layoutPage  Pointer to the Secure UI screen layout that
  statically represents the appearance of the screen.
  Later, this layout will be validated and loaded on
  the screen.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_BAD_LAYOUT

@dependencies
None.
*/

layout_mgr_err_t layout_mgr_load_layout(LayoutPage_t* layoutPage);

/*********************************************************************************/

/**
  External function for setting the buffer of the label of some input object
  in the layout.

  @note1hang This function does not set the text itself (curr_len is zeroed).

  @param[in] objectName  Pointer to the object name (as the unique identifier
  in the layout).
  @param[in] buffer      Pointer to the buffer to be set for this object.
  @param[in] len         Length of the buffer, in bytes.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
@newpage
*/
layout_mgr_err_t layout_mgr_set_buffer_for_input(const char* objectName,
                                                 uint8_t* buffer, uint32_t len);

/************************************************************************************/

/**
  External function for getting the buffer of an input object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] buffer      Double pointer to the buffer set for this object.
  @param[out] len         Pointer to length of the buffer.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_buffer_of_input(const char* objectName,
                                                const uint8_t** buffer,
                                                uint32_t* len);

/******************************************************************************/

/**
  External function for setting the show flag of an object in the layout.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] show        Show flag to be set.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_show_flag(const char* objectName,
                                          uint32_t show);

/*********************************************************************************/
/**
  External function for getting the show flag value of an object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] show        Pointer to the show flag.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_show_flag(const char* objectName,
                                          uint32_t* show);

/******************************************************************************/

/**
  External function for setting a focused input object in the layout.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
@newpage
*/
layout_mgr_err_t layout_mgr_set_focus_input(const char* objectName);

/**********************************************************************************/
/**
  External function for setting the image buffer and the alignment of some image
  object in the layout.

  @param[in] objectName   Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] imageBuffer  Pointer to the buffer that contains the PNG
  representation of the required image.
  @param[in] vAlign       Vertical alignment.
  @param[in] hAlign       Horizontal alignment.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_IMAGE_EXCEED_OBJECT \n
#LAYOUT_MGR_INVALID_IMAGE \n
#LAYOUT_MGR_UNSUPPORTED -- Unsupported parameter values. \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an image type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_image(const char* objectName,
                                      const uint8_t* imageBuffer,
                                      VerticalAlignment_t vAlign,
                                      HorizontalAlignment_t hAlign);

/*******************************************************************************/

/**
  External function for setting the active keyboard in the layout.

  @param[in] keyboardName  Pointer to the keyboard name to be set as active.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_active_keyboard(const char* keyboardName);

/**********************************************************************************/

/**
  External function for keyboard randomization in the layout.

  @param[in] keyboardName  Pointer to the keyboard name to be randomized.
  @param[in] isOneTime     Set to TRUE if randomization is to be one time
  per device lifetime.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_FAILED_GEN_RAND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_randomize_keyboard(const char* keyboardName,
                                               uint8_t isOneTime);

/*********************************************************************************/

/**
  External function for setting the length of the text label of an input
  object in the layout.

  The length is to be less than or equal to the specified maximum length.
  If the length is longer, the text is truncated to this length.

  @param[in] objectName     Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] max_chars_len  Maximum length, in characters.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_truncate_input_string(const char* objectName,
                                                  uint32_t max_chars_len);

/**********************************************************************************/

/**
  External function for deleting the last character of the focused input object.
  If the focused input is empty, nothing should happen.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_OBJECT_NOT_FOUND -- In case of no focused input. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/

layout_mgr_err_t layout_mgr_delete_input_last_character(void);

/************************************************************************************/

/**
  External function for setting the buffer of an input object to zero.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_clear_input_buffer(const char* objectName);

/************************************************************************************/

/**
  External function for setting the hidden character of an input object.
  In the case of DISP_MODE_HIDDEN (as declared in #LayoutInputDisplayMode_t),
  this character is displayed instead of the clear string characters.

  @param[in] objectName   Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] hidden_char  Hidden character in UTF-8 format.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_input_hidden_char(const char* objectName,
                                                  utf8_char_t hidden_char);

/**************************************************************************************/
/**
  External function for getting the hidden character of an input object.
  In the case of DISP_MODE_HIDDEN (as declared in #LayoutInputDisplayMode_t),
  this character is displayed instead of the clear string characters.

  @param[in] objectName    Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] hidden_char  Pointer to the hidden character.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_input_hidden_char(const char* objectName,
                                                  utf8_char_t* hidden_char);

/*************************************************************************************/
/**
  External function for setting the display mode of some input object in the
  layout.

  @datatypes
#LayoutInputDisplayMode_t

@param[in] objectName  Pointer to the object name (as a unique identifier
in the layout).
@param[in] mode        Display mode to be set.

@return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_input_display_mode(
    const char* objectName, LayoutInputDisplayMode_t mode);

/************************************************************************************/
/**
  External function for getting the display mode of some input object in the
  layout.

  @datatypes
#LayoutInputDisplayMode_t

@param[in]  objectName  Pointer to the object name (as a unique identifier
in the layout).
@param[out] mode        Pointer to the display mode that is set.

@return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_input_display_mode(
    const char* objectName, LayoutInputDisplayMode_t* mode);

/************************************************************************************/
/**
  External function for getting the status of an input object in the layout.

  @param[in]  objectName    Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] input_status  Pointer to the current status: focused or not
  focused.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_input_status(const char* objectName,
                                             LayoutInputStatus_t* input_status);

/***********************************************************************************/
/**
  External function for checking that a text string does not exceed the
  object size.

  @param[in]  objectName   Pointer to the object name (as a unique identifier
  in the layout) with which to check the text.
  @param[in]  text         Buffer containing the UTF-8 representation of the
  string to be checked.
  @param[in]  len          Length of the string to be checked, in bytes.
  @param[out] fitting_len  Length of the substring that can fit the object,
  in bytes.
  @param[out] char_length  Length of the substring that can fit the object,
  in characters.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_GENERAL_ERROR \n
#LAYOUT_MGR_INVALID_UTF8_STR \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_check_exceeding_text(const char* objectName,
                                                 const uint8_t* text,
                                                 uint32_t len,
                                                 uint32_t* fitting_len,
                                                 uint32_t* char_length);

/***********************************************************************************/
/**
  External function for setting some text string to an object.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] text        Buffer containing the UTF-8 representation of the
  string to be set.
  @param[in] len         Length of the string to be set, in bytes.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_GENERAL_ERROR \n
#LAYOUT_MGR_INVALID_UTF8_STR \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text(const char* objectName,
                                     const uint8_t* text, uint32_t len);

/************************************************************************************/
/**
  External function for getting the text string of an object.

  @param[in]  objectName   Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] text         Buffer containing the UTF-8 representation of the
  string.
  @param[out] len          Length of the string, in bytes.
  @param[out] char_length  Length of the substring that can fit the object,
  in characters.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text(const char* objectName,
                                     const uint8_t** text, uint32_t* len,
                                     uint32_t* char_length);

/*******************************************************************************/
/**
  External function for setting the text font of an object.

  @note1hang This function is not supported for input type objects.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] font_path   Pointer to the absolute path to a font file.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_font(const char* objectName,
                                          const char* font_path);

/***********************************************************************************/
/**
  External function for getting the path for the dynamic font of an object.
  This function should return NULL for input type objects.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] font_path   Double Pointer to the ASCII string of the font path.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_font(const char* objectName,
                                          const char** font_path);

/***********************************************************************************/
/**
  External function for setting the text color of an object in the layout.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] color       Color to be set.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_color(const char* objectName,
                                           Color_t color);

/**********************************************************************************/
/**
  External function for getting the text color of an object in the layout.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] color      Pointer to the color that is set.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_color(const char* objectName,
                                           Color_t* color);

/**********************************************************************************/
/**
  External function for setting the text alignment, horizontal and vertical,
  of an object in the layout.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] v_align     Vertical alignment.
  @param[in] h_align     Horizontal alignment.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_alignment(const char* objectName,
                                               VerticalAlignment_t v_align,
                                               HorizontalAlignment_t h_align);

/***********************************************************************************/
/**
  External function for getting the text alignment, horizontal and vertical,
  of an object in the layout.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] v_align     Vertical alignment.
  @param[out] h_align     Horizontal alignment.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_alignment(const char* objectName,
                                               VerticalAlignment_t* v_align,
                                               HorizontalAlignment_t* h_align);

/***********************************************************************************/
/**
  External function for setting the flag to allow line breaking of text of
  an object.


  @param[in] objectName           Pointer to the object name (as a unique
  identifier in the layout).
  @param[in] allow_line_breaking  TRUE (allow) or FALSE (do not allow).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_line_breaking(
    const char* objectName, uint32_t allow_line_breaking);

/*********************************************************************************/
/**
  External function for getting the flag to allow line breaking of text of
  an object.


  @param[in]  objectName           Pointer to the object name (as a unique
  identifier in the layout).
  @param[out] allow_line_breaking  Pointer to the value: TRUE (allow) or
  FALSE (do not allow).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_text_line_breaking(
    const char* objectName, uint32_t* allow_line_breaking);

/**************************************************************************************/
/**
  External function for checking that a text string does not exceed the
  object size. The string is used for the hint label of an input object.

  @param[in]  objectName   Pointer to the object name (as a unique identifier
  in the layout).
  @param[in]  text         Pointer to the buffer containing the UTF-8
  representation of the string.
  @param[in]  len          Length of the string to be checked, in bytes.
  @param[out] fitting_len  Pointer to the length (in bytes) of the substring
  that can fit the object.
  @param[out] char_length  Pointer to the length (in characters) of the
  substring that can fit the object.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_GENERAL_ERROR \n
#LAYOUT_MGR_INVALID_UTF8_STR \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_check_exceeding_text_hint(const char* objectName,
                                                      const uint8_t* text,
                                                      uint32_t len,
                                                      uint32_t* fitting_len,
                                                      uint32_t* char_length);

/************************************************************************************/
/**
  External function for setting a text string, which is used for the hint
  label of an input object.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] text        Pointer to the buffer containing the UTF-8
  representation of the string.
  @param[in] len         Length of the string to be set, in bytes.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_GENERAL_ERROR \n
#LAYOUT_MGR_INVALID_UTF8_STR \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_hint(const char* objectName,
                                          const uint8_t* text, uint32_t len);

/**************************************************************************************/
/**
  External function for getting the text string, which is used for the hint
  label of an input object.

  @param[in] objectName    Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] text         Double pointer to the buffer containing the UTF-8
  representation of the string.
  @param[out] len          Pointer to the length (in bytes) of the string.
  @param[out] char_length  Pointer to the length (in characters) of the
  substring that can fit the object.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_text_hint(const char* objectName,
                                          const uint8_t** text, uint32_t* len,
                                          uint32_t* char_length);

/********************************************************************************/
/**
  External function for setting the text font, which is used for the hint
  label of an input object.

  @note1hang This function is not supported for input type objects.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] font_path   Pointer to the ASCII string of the font path.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_FONT_MISSING_CHAR \n
#LAYOUT_MGR_DYNAMIC_FONT_ERROR \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_font_hint(const char* objectName,
                                               const char* font_path);

/********************************************************************************/
/**
  External function for getting the path for the dynamic font, which is used
  for the hint label of an input object.

  This function should return NULL for input type objects.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] font_path   Double pointer to the ASCII string of the font path.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_font_hint(const char* objectName,
                                               const char** font_path);

/**********************************************************************************/
/**
  External function for setting the text color, which is used for the hint
  label of an input object.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] color       Text color.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_color_hint(const char* objectName,
                                                Color_t color);

/************************************************************************************/
/**
  External function for getting the text color, which is used for the hint label
of an
  input object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] color       Pointer to the text color.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_color_hint(const char* objectName,
                                                Color_t* color);

/**********************************************************************************/
/**
  External function for setting the text alignment: horizontal and vertical.
  The alignment is used for the hint label of an input object.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] v_align     Vertical alignment.
  @param[in] h_align     Horizontal alignment.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_alignment_hint(
    const char* objectName, VerticalAlignment_t v_align,
    HorizontalAlignment_t h_align);

/**************************************************************************************/
/**
  External function for getting the text alignment, horizontal and vertical;
  used for the hint label of an input object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] v_align     Pointer to the vertical alignment.
  @param[out] h_align     Pointer to the horizontal alignment.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_alignment_hint(
    const char* objectName, VerticalAlignment_t* v_align,
    HorizontalAlignment_t* h_align);

/**************************************************************************************/
/**
  External function for setting the flag to allow line breaking of text. Line
  breaks are used for the hint label of an input object.


  @param[in] objectName           Pointer to the object name (as a unique
  identifier in the layout).
  @param[in] allow_line_breaking  TRUE (allow) or FALSE (do not allow).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_text_line_breaking_hint(
    const char* objectName, uint32_t allow_line_breaking);

/***********************************************************************************/
/**
  External function for getting the flag to allow line breaking of text. Line
  breaking is used for the hint label of an input object.

  @param[in]  objectName           Pointer to the object name (as a unique
  identifier in the layout).
  @param[out] allow_line_breaking  Pointer to the to the value: TRUE (allow)
  or FALSE (do not allow).

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_UNSUPPORTED \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_line_breaking_hint(
    const char* objectName, uint32_t* allow_line_breaking);

/*************************************************************************************/
/**
  External function for getting the layout flavor: pin, login, msg, or custom.

  @param[out] flavor  Layout flavor.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_layout_flavor(LayoutFlavor_t* flavor);

/**********************************************************************************/
/**
  External function for getting the layout size.

  @param[out] layout_height  Pointer to the layout height in pixels.
  @param[out] layout_width   Pointer to the layout width in pixels.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_layout_size(uint32_t* layout_height,
                                            uint32_t* layout_width);

/*************************************************************************************/
/**
  External function for getting the currently displayed keyboard.

  @param[out] keyboardName  Double pointer to the name of the keyboard.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_active_keyboard(const char** keyboardName);

/**************************************************************************************/
/**
  External function for getting the currently selected input object.

  @param[out] focused_input_name  Double pointer to the name of the input
  object.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_focus_input(const char** focused_input_name);

/***********************************************************************************/
/**
  External function for getting the size of an object.

  @param[in]  objectName     Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] object_height  Pointer to the object height in pixels.
  @param[out] object_width   Pointer to the object width in pixels.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_object_size(const char* objectName,
                                            uint32_t* object_height,
                                            uint32_t* object_width);

/************************************************************************************/
/**
  External function for getting the type of a button object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] type        Pointer to the button type: data or function.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_type(const char* objectName,
                                            LayoutButtonType_t* type);

/***********************************************************************************/
/**
  External function for getting the function of a button object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] function    Pointer to the button function.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
#LAYOUT_MGR_UNSUPPORTED -- Button is not a function type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_button_function(
    const char* objectName, LayoutButtonFunctionType_t* function);

/************************************************************************************/
/**
  External function for getting the data of a button object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] data        Pointer to the button data, which is UTF-8 encoded.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
#LAYOUT_MGR_UNSUPPORTED -- Button is not a data type.

@dependencies
layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_data(const char* objectName,
                                            utf8_char_t* data);

/*********************************************************************************/
/**
  External function for setting the state of a button object.

  @param[in] objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[in] state       Button state: released, pressed, or disabled.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
#LAYOUT_MGR_UNSUPPORTED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_set_button_state(const char* objectName,
                                             LayoutButtonStatus_t state);

/***************************************************************************************/
/**
  External function for getting the state of a button object.

  @param[in]  objectName  Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] state       Pointer to the button state: released, pressed, or
  disabled.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type.

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_button_state(const char* objectName,
                                             LayoutButtonStatus_t* state);

/****************************************************************************************/
/**
  External function for getting the image buffer, size, and alignment of an
  image object in the layout.

  @param[in]  objectName    Pointer to the object name (as a unique identifier
  in the layout).
  @param[out] image_buffer  Double pointer to the buffer that contains the PNG
  representation of the image.
  @param[out] v_align       Pointer to the vertical alignment.
  @param[out] h_align       Pointer to the horizontal alignment.
  @param[out] image_height  Pointer to the image height in pixels.
  @param[out] image_width   Pointer to the image width in pixels.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT \n
#LAYOUT_MGR_OBJECT_NOT_FOUND \n
#LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an image type. \n
#LAYOUT_MGR_LAYOUT_NOT_LOADED

@dependencies
layout_mgr_load_layout() must be called before calling this function. @newpage
*/
layout_mgr_err_t layout_mgr_get_image(const char* objectName,
                                      const uint8_t** image_buffer,
                                      VerticalAlignment_t* v_align,
                                      HorizontalAlignment_t* h_align,
                                      uint32_t* image_height,
                                      uint32_t* image_width);

/**************************************************************************************/
/**
  External function for setting the default timeout (in milliseconds) of the
  touch session. This timeout is the maximum time to wait for user input.

  @param[in] timeout  Default timeout to be returned in layout_mgr_eval_event()
  in case the Layout Manager does not request a different
  timeout for operating the common behavior of the layout. \n
  - 0 -- return immediately
  - -1 -- no timeout (wait indefinitely)
  - If not applicable, use NO_TIMEOUT.

  @return
#LAYOUT_MGR_SUCCESS

@dependencies
None.
*/
layout_mgr_err_t layout_mgr_set_default_timeout(int32_t timeout);

/**********************************************************************************/
/**
  External function for getting the default timeout (in milliseconds) of the
  touch session. This timeout is the maximum time to wait for user input.


  @param[out] timeout  Pointer to the default timeout to be returned in
  layout_mgr_eval_event(). \n
  - 0 -- return immediately
  - -1 -- no timeout (wait indefinitely)
  - If not applicable, use NO_TIMEOUT.

  @return
#LAYOUT_MGR_SUCCESS \n
#LAYOUT_MGR_GOT_NULL_INPUT

@dependencies
None.
*/
layout_mgr_err_t layout_mgr_get_default_timeout(int32_t* timeout);

/**************************************************************************************/

/**
  Cleans up the allocated buffers.

  @return
  None.
  */
void free_layout_buffers();

/** @} */ /* end_addtogroup layout_manager */

#endif /* LAYOUT_MGR_H_ */
