
/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2015, 2019, 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
*********************************************************************/


#ifndef __LAYOUT_H__
#define __LAYOUT_H__
#include <string.h>
#include "qsee_tui_dialogs.h"

/*
   Do not use, for internal usage only.
*/
LayoutPage_t* create_layout(LayoutFlavor_t layout_flavor);
LayoutPage_t* create_layout_1080x2340(LayoutFlavor_t layout_flavor);
LayoutPage_t* create_layout_eid(LayoutFlavor_t layout_flavor);

/** @ingroup customization_tool
   Receives a layout by name.

   @note1hang  If the generated layout file was compiled together with the
               calling application, this function returns any layout that was
               created with the Customization tool.

   @param[in]  layout_name  Layout name; must be unique per application.

   @return
   A pointer to the layout structure.

   @dependencies
   None.
*/
LayoutPage_t* get_layout_by_name(const char* layout_name);
LayoutPage_t* get_layout_by_name_1080x2340(const char* layout_name);
LayoutPage_t* get_layout_by_name_eid(const char* name);
void get_eid_rsa_config(uint8_t **rsa_exponent, uint8_t **rsa_modules);

static inline LayoutPage_t* create_layout_ex(uint32_t width,
                                             uint32_t height,
                                             LayoutFlavor_t layout_flavor)
{
       /* Using same layout for shimla and yupik as adding new
       layout will increase memory requirement for TVM*/
#ifdef ENABLE_MULTIPLE_LAYOUT_SUPPORT
       if ((width == 1080 && height == 2340) ||
           (width == 1080 && height == 2408))
           return create_layout_1080x2340(layout_flavor);
       else
           return create_layout(layout_flavor);
#else
           return create_layout(layout_flavor);
#endif

}

static inline LayoutPage_t* get_layout_by_name_ex(uint32_t width,
                                                  uint32_t height,
                                                  const char* layout_name)
{
       /* Using same layout for shimla and yupik as adding new
       layout will increase memory requirement for TVM*/
#ifdef ENABLE_MULTIPLE_LAYOUT_SUPPORT
       if ((width == 1080 && height == 2340) ||
           (width == 1080 && height == 2408))
           return get_layout_by_name_1080x2340(layout_name);
       else
           return get_layout_by_name(layout_name);
#else
           return get_layout_by_name(layout_name);
#endif
}

#endif
