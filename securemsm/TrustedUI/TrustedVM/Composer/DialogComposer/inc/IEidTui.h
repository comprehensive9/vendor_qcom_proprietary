/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
#include "object.h"
#include "eid_tui.h"

static inline int32_t
IEidTui_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IEidTui_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IEidTui_GetTUIMessage(Object self, void *eid_ptr, size_t eid_len, size_t *eid_lenout)
{
  ObjectArg a[1];
  a[0].b = (ObjectBuf) { eid_ptr, eid_len * 1 };

  int32_t result = Object_invoke(self, IEidTui_OP_GetTUIMessage, a, ObjectCounts_pack(0, 1, 0, 0));

  *eid_lenout = a[0].b.size / 1;

  return result;
}

