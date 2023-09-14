/*
 * Copyright (c) 2013, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "InputStatus.h"
#include "SecureTouchLayout.h"

/**
 * Verify the layout contained in a page.
 */
uint32_t verifyLayout(LayoutPage_t *page,
                      SecureTouch_InputStatus_t *inputStatus,
                      LayoutCores_t *refs, uint32_t const width,
                      uint32_t const height);

/**
 * Find which object in a page is located at a specified position.
 */
uint32_t findObject(uint32_t const x, uint32_t const y,
                    LayoutPage_t *const page, LayoutObject_t **obj);

uint32_t updatePage(const uint32_t x, const uint32_t y, const uint32_t up,
                    LayoutPage_t *page, LayoutCores_t *refs,
                    LayoutObject_t **clickedObj,
                    SecureTouch_InputStatus_t *inputStatus,
                    SecureTouchResponse_t *response,
                    SecureTouch_Event_t *event);
