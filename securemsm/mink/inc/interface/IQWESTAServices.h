/*****************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

 #include <stdint.h>
 #include "object.h"
 
 
 #define IQWESTAServices_OP_createServiceById 0
 #define IQWESTAServices_secChannelServiceID UINT32_C(102)
 
 
 static inline int32_t
 IQWESTAServices_release(Object self)
 {
   return Object_invoke(self, Object_OP_release, 0, 0);
 }
 
 static inline int32_t
 IQWESTAServices_retain(Object self)
 {
   return Object_invoke(self, Object_OP_retain, 0, 0);
 }
 
 static inline int32_t
 IQWESTAServices_createServiceById(Object self, const void *license_ptr, size_t license_len, uint32_t serviceId_val, Object *service_ptr)
 {
   ObjectArg a[3];
   a[0].bi = (ObjectBufIn) { license_ptr, license_len * 1 };
   a[1].b = (ObjectBuf) { &serviceId_val, sizeof(uint32_t) };
 
   int32_t result = Object_invoke(self, IQWESTAServices_OP_createServiceById, a, ObjectCounts_pack(2, 0, 0, 1));
 
   *service_ptr = a[2].o;
 
   return result;
 }