/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"
#include "IHLOSListenerCBO.hpp"

/** @file  IHLOSListenerCBO.idl */

/**
 * @brief
 * HLOSListenerCBO interface provides methods to communicate to the Trusted UI HAL in HLOS.
 */

/** @cond */

class HLOSListenerCBOImplBase : protected ImplBase, public IHLOSListenerCBO {
   public:
    HLOSListenerCBOImplBase() {}
    virtual ~HLOSListenerCBOImplBase() {}

    virtual int32_t invoke(ObjectOp op, ObjectArg* a, ObjectCounts k) {
        switch (ObjectOp_methodID(op)) {
            case OP_onComplete: {
                return onComplete();
            }
            case OP_onError: {
                if (k != ObjectCounts_pack(1, 0, 0, 0) ||
                    a[0].b.size != 4) {
                    break;
                }
                const int32_t* error_ptr = (const int32_t*) a[0].b.ptr;
                return onError(*error_ptr);
            }
            case OP_handleCmd: {
                if (k != ObjectCounts_pack(1, 1, 0, 0)) {
                    break;
                }
                const void* cmd_ptr = (const void*)a[0].b.ptr;
                size_t cmd_len = a[0].b.size / 1;
                void* rsp_ptr = (void*)a[1].b.ptr;
                size_t rsp_len = a[1].b.size / 1;
                int32_t r = handleCmd(cmd_ptr, cmd_len, rsp_ptr, rsp_len, &rsp_len);
                a[1].b.size = rsp_len * 1;
                return r;
            }
            default: { return Object_ERROR_INVALID; }
        }
        return Object_ERROR_INVALID;
    }
};
