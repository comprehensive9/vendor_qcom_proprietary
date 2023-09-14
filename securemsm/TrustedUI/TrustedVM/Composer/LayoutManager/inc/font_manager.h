/*===========================================================================
  Copyright (c) 2015-2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#ifndef FONT_MGR_H_
#define FONT_MGR_H_

/** @file font_manager.h
 * @brief
 * This file contains utilities for handling dynamic fonts
 */



#include "gd.h"
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum {
    FONT_MGR_SUCCESS = 0,
    FONT_MGR_GENERAL_ERROR = -1,
    FONT_MGR_INPUT_PARAMS_ERROR = -2,
    FONT_MGR_INVALID_UTF8_STR = -3,
    FONT_MGR_MEM_ALLOCATION_ERROR = -4,
    FONT_MGR_BUFFER_ERROR = -4,
    FONT_MGR_LISTENER_ERROR = -6,
    FONT_MGR_NO_FONT_ENTRY_FOR_CHAR = -7,
    FONT_MGR_FONT_CACHE_ERROR = -8,
    FONT_MGR_FONT_SIZE_LIMIT_ERROR = -9,
    FONT_MGR_RSA_ERROR = -10,
    FONT_MGR_FONT_FILE_ERROR = -11,

    FONT_MGR_ERR_SIZE = 0x7FFFFFFF
} font_mgr_err_t;

typedef int32_t (*font_cb_t)(const void* cmd_ptr,
                     size_t cmd_len,
                     void* rsp_ptr,
                     size_t rsp_len,
                     size_t* rsp_lenout);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*****************************************************************************/

/**
  @brief
  External function that prepares a gdFont for a layout object.
  If the existing font is static and supports the needed text, nothing is
  changed.
  If not, a dynamic font is prepared for the object.
  */
font_mgr_err_t font_mgr_prepare_gdfont(const char* fontPath,
                                       const uint8_t* text, uint32_t txtLen,
                                       gdFontExt** gdFont);

/*******************************************************************************/

/**
  @brief
  Set font manager global state to copy static font data into dynamic font (1 -
  enable, 0 - disable)
  */
font_mgr_err_t font_mgr_cfg_enable_static_to_dynamic_font_copy(uint8_t enabled);

/********************************************************************************/

/**
  @brief
  External function for cleanup on application shutdown
  */
void font_mgr_cleanup(void);

/**
  @brief
  External function for registering a callback for font manager.
  */
font_mgr_err_t font_mgr_register_callback(font_cb_t cb);

/**
  @brief
  External function for Setting RSA parameters
  */
void font_mgr_config_rsa(uint8_t *rsa_exponent, uint8_t *rsa_modules);

#endif /* FONT_MGR_H_ */
