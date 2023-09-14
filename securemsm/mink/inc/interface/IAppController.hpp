#pragma once
// Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

/** @cond */

// AUTOGENERATED FILE: DO NOT EDIT

#include <cstdint>
#include "object.h"
#include "proxy_base.hpp"

class IIAppController {
   public:
    static const uint32_t CBO_INTERFACE_WAIT = UINT32_C(1);

    static const int32_t ERROR_APP_SUSPENDED = INT32_C(10);
    static const int32_t ERROR_APP_BLOCKED_ON_LISTENER = INT32_C(11);
    static const int32_t ERROR_APP_UNLOADED = INT32_C(12);
    static const int32_t ERROR_APP_IN_USE = INT32_C(13);
    static const int32_t ERROR_NOT_SUPPORTED = INT32_C(14);
    static const int32_t ERROR_CBO_UNKNOWN = INT32_C(15);
    static const int32_t ERROR_APP_UNLOAD_NOT_ALLOWED = INT32_C(16);
    static const int32_t ERROR_APP_DISCONNECTED = INT32_C(17);
    static const int32_t ERROR_USER_DISCONNECT_REJECTED = INT32_C(18);
    static const int32_t ERROR_STILL_RUNNING = INT32_C(19);

    virtual ~IIAppController() {}

    virtual int32_t openSession(uint32_t cancelCode_val, uint32_t connectionMethod_val, uint32_t connectionData_val, uint32_t paramTypes_val, uint32_t exParamTypes_val, const void* i1_ptr, size_t i1_len, const void* i2_ptr, size_t i2_len, const void* i3_ptr, size_t i3_len, const void* i4_ptr, size_t i4_len, void* o1_ptr, size_t o1_len, size_t* o1_lenout, void* o2_ptr, size_t o2_len, size_t* o2_lenout, void* o3_ptr, size_t o3_len, size_t* o3_lenout, void* o4_ptr, size_t o4_len, size_t* o4_lenout, const ProxyBase &imem1_ref, const ProxyBase &imem2_ref, const ProxyBase &imem3_ref, const ProxyBase &imem4_ref, uint32_t* memrefOutSz1_ptr, uint32_t* memrefOutSz2_ptr, uint32_t* memrefOutSz3_ptr, uint32_t* memrefOutSz4_ptr, ProxyBase &session_ref, uint32_t* retValue_ptr, uint32_t* retOrigin_ptr) = 0;
    virtual int32_t unload() = 0;
    virtual int32_t getAppObject(ProxyBase &obj_ref) = 0;
    virtual int32_t installCBO(uint32_t uid_val, const ProxyBase &obj_ref) = 0;
    virtual int32_t disconnect() = 0;
    virtual int32_t restart() = 0;

   protected:
    static const ObjectOp OP_openSession = 0;
    static const ObjectOp OP_unload = 1;
    static const ObjectOp OP_getAppObject = 2;
    static const ObjectOp OP_installCBO = 3;
    static const ObjectOp OP_disconnect = 4;
    static const ObjectOp OP_restart = 5;
};

class IAppController : public IIAppController, public ProxyBase {
   public:
    IAppController() {}
    IAppController(Object impl) : ProxyBase(impl) {}
    virtual ~IAppController() {}

    virtual int32_t openSession(uint32_t cancelCode_val, uint32_t connectionMethod_val, uint32_t connectionData_val, uint32_t paramTypes_val, uint32_t exParamTypes_val, const void* i1_ptr, size_t i1_len, const void* i2_ptr, size_t i2_len, const void* i3_ptr, size_t i3_len, const void* i4_ptr, size_t i4_len, void* o1_ptr, size_t o1_len, size_t* o1_lenout, void* o2_ptr, size_t o2_len, size_t* o2_lenout, void* o3_ptr, size_t o3_len, size_t* o3_lenout, void* o4_ptr, size_t o4_len, size_t* o4_lenout, const ProxyBase &imem1_ref, const ProxyBase &imem2_ref, const ProxyBase &imem3_ref, const ProxyBase &imem4_ref, uint32_t* memrefOutSz1_ptr, uint32_t* memrefOutSz2_ptr, uint32_t* memrefOutSz3_ptr, uint32_t* memrefOutSz4_ptr, ProxyBase &session_ref, uint32_t* retValue_ptr, uint32_t* retOrigin_ptr) {
        ObjectArg a[15];
        struct s1 {
            uint32_t m_cancelCode;
            uint32_t m_connectionMethod;
            uint32_t m_connectionData;
            uint32_t m_paramTypes;
            uint32_t m_exParamTypes;
        } i;
        a[0].b = (ObjectBuf) {&i, 20};
        struct s6 {
            uint32_t m_memrefOutSz1;
            uint32_t m_memrefOutSz2;
            uint32_t m_memrefOutSz3;
            uint32_t m_memrefOutSz4;
            uint32_t m_retValue;
            uint32_t m_retOrigin;
        } o;
        a[5].b = (ObjectBuf) {&o, 24};
        i.m_cancelCode = cancelCode_val;
        i.m_connectionMethod = connectionMethod_val;
        i.m_connectionData = connectionData_val;
        i.m_paramTypes = paramTypes_val;
        i.m_exParamTypes = exParamTypes_val;
        a[1].bi = (ObjectBufIn) {i1_ptr, i1_len * 1};
        a[2].bi = (ObjectBufIn) {i2_ptr, i2_len * 1};
        a[3].bi = (ObjectBufIn) {i3_ptr, i3_len * 1};
        a[4].bi = (ObjectBufIn) {i4_ptr, i4_len * 1};
        a[6].b = (ObjectBuf) {o1_ptr, o1_len * 1};
        a[7].b = (ObjectBuf) {o2_ptr, o2_len * 1};
        a[8].b = (ObjectBuf) {o3_ptr, o3_len * 1};
        a[9].b = (ObjectBuf) {o4_ptr, o4_len * 1};
        a[10].o = imem1_ref.get();
        a[11].o = imem2_ref.get();
        a[12].o = imem3_ref.get();
        a[13].o = imem4_ref.get();

        int32_t result = invoke(OP_openSession, a, ObjectCounts_pack(5, 5, 4, 1));
        if (Object_OK != result) { return result; }

        *o1_lenout = a[6].b.size / 1;
        *o2_lenout = a[7].b.size / 1;
        *o3_lenout = a[8].b.size / 1;
        *o4_lenout = a[9].b.size / 1;
        *memrefOutSz1_ptr = o.m_memrefOutSz1;
        *memrefOutSz2_ptr = o.m_memrefOutSz2;
        *memrefOutSz3_ptr = o.m_memrefOutSz3;
        *memrefOutSz4_ptr = o.m_memrefOutSz4;
        session_ref.consume(a[14].o);
        *retValue_ptr = o.m_retValue;
        *retOrigin_ptr = o.m_retOrigin;

        return result;
    }

    virtual int32_t unload() {
        return invoke(OP_unload, 0, 0);
    }

    virtual int32_t getAppObject(ProxyBase &obj_ref) {
        ObjectArg a[1];

        int32_t result = invoke(OP_getAppObject, a, ObjectCounts_pack(0, 0, 0, 1));
        if (Object_OK != result) { return result; }

        obj_ref.consume(a[0].o);

        return result;
    }

    virtual int32_t installCBO(uint32_t uid_val, const ProxyBase &obj_ref) {
        ObjectArg a[2];
        a[0].b = (ObjectBuf) {&uid_val, sizeof(uint32_t)};
        a[1].o = obj_ref.get();

        return invoke(OP_installCBO, a, ObjectCounts_pack(1, 0, 1, 0));
    }

    virtual int32_t disconnect() {
        return invoke(OP_disconnect, 0, 0);
    }

    virtual int32_t restart() {
        return invoke(OP_restart, 0, 0);
    }

};
