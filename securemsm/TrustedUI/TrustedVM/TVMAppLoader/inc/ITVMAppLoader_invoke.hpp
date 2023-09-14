#pragma once
/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/** @file  ITVMAppLoader.idl */

/**
 * @brief
 * TVMAppLoader interface provides methods to load a TrustedUI App in the
 * TEE (Trusted Execute Environment)
 */

/** @cond */

// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"
#include "proxy_base.hpp"
#include "impl_base.hpp"
#include "ITVMAppLoader.hpp"

class TVMAppLoaderImplBase : protected ImplBase, public ITVMAppLoader {
   public:
    TVMAppLoaderImplBase() {}
    virtual ~TVMAppLoaderImplBase() {}

   protected:
    virtual int32_t invoke(ObjectOp op, ObjectArg* a, ObjectCounts k) {
        switch (ObjectOp_methodID(op)) {
            case OP_loadApp: {
                if (k != ObjectCounts_pack(1, 0, 0, 1) ||
                    a[0].b.size != 12) {
                    break;
                }
                const struct s1 {
                    uint32_t m_UID;
                    uint32_t m_memparcel_handle;
                    uint32_t m_mem_size;
                }* i = (const struct s1*)a[0].b.ptr;
                return loadApp(i->m_UID, i->m_memparcel_handle, i->m_mem_size, a[1].o);
            }
            case OP_unloadApp: {
                if (k != ObjectCounts_pack(1, 0, 0, 0) ||
                    a[0].b.size != 4) {
                    break;
                }
                const uint32_t* UID_ptr = (const uint32_t*)a[0].b.ptr;
                return unloadApp(*UID_ptr);
            }
            default: { return Object_ERROR_INVALID; }
        }
        return Object_ERROR_INVALID;
    }
};
