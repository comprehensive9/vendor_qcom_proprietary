/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <unordered_map>
#include <memory>
#include <list>
#include <functional>

#include <framework/legacy.h>
#include <framework/Log.h>
#include "IWlanImpl.h"

using android::sp;

namespace vendor {
namespace qti {
namespace hardware {
namespace data {
namespace iwlan {

class IWlanImpl;

class IWlanServiceVersion
{
    private:
        uint16_t mMajor;
        uint16_t mMinor;
        uint16_t mLocal;
    public:
        IWlanServiceVersion(uint16_t major, uint16_t minor, uint16_t local = 0):
            mMajor(major), mMinor(minor), mLocal(local)
        { }

        bool operator< (const IWlanServiceVersion &other) const {
            QCRIL_LOG_DEBUG("major: %u, minor: %u local: %u", (unsigned)mMajor, (unsigned)mMinor, (unsigned)mLocal);
            QCRIL_LOG_DEBUG("o.major: %u, o.minor: %u o.local: %u", (unsigned)other.mMajor, (unsigned)other.mMinor, (unsigned)other.mLocal);
            return mMajor >= other.mMajor &&
                mMinor >= other.mMinor &&
                mLocal >= other.mLocal;
        }
        bool operator>= (const IWlanServiceVersion &other) const {
            return (!((*this) < (other)));
        }
};

class IWlanServiceFactory
{
    public:
        template <class B>
        using
        FactMethod = sp<B> (IWlanServiceFactory::*)(qcril_instance_id_e_type instance);

    private:
        template <class T>
        sp<IWlanImpl> getTIWlanImpl(qcril_instance_id_e_type instance) {
            return sp<T>(new T(instance));
        }

        template <class B>
        struct FactoryEntry {
            FactMethod<B> factMethod;
            IWlanServiceVersion version;
        };

        template <class B>
        static inline
        bool iwlanServiceCompare
        (
         const FactoryEntry<B>& first,
         const FactoryEntry<B>& second)
        {
            return first.version < second.version;
        };


        using IWlanImplListType = std::list<FactoryEntry<IWlanImpl>>;
        IWlanImplListType iwlanImplList;
        IWlanImplListType::iterator configIter;

    public:
        template <class T>
        void registerIWlanImpl(T *) {
            QCRIL_LOG_DEBUG("registerIWlanImpl");
            FactMethod<IWlanImpl> factMethod = &IWlanServiceFactory::getTIWlanImpl<T>;
            IWlanImplListType localList { FactoryEntry<IWlanImpl> {factMethod, T::getVersion()} };
            iwlanImplList.merge(localList, &iwlanServiceCompare<IWlanImpl>);
        }

        sp<IWlanImpl> getNextIWlanImpl(qcril_instance_id_e_type instance) {
            QCRIL_LOG_INFO("getNextIWlanImpl");
            sp<IWlanImpl> ret;
            if (!iwlanImplList.empty()) {
                if (configIter != iwlanImplList.end()) {
                    FactMethod<IWlanImpl> m = configIter->factMethod;
                    if (m) {
                        ret = (this->*m)(instance);
                    }
                    configIter++;
                }
            }
            return ret;
        }

        void initIWlanListIndex() {
            configIter = iwlanImplList.begin();
        }
};

IWlanServiceFactory &getIWlanServiceFactory();

}
}
}
}
}
