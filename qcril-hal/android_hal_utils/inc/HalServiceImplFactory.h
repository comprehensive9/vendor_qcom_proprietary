/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include <utils/StrongPointer.h>

#include <framework/Log.h>

#undef TAG
#define TAG "RILQ"

class HalServiceImplVersion {
 private:
  uint16_t mMajor;
  uint16_t mMinor;
  uint16_t mLocal;

 public:
  HalServiceImplVersion(uint16_t major, uint16_t minor, uint16_t local = 0)
      : mMajor(major), mMinor(minor), mLocal(local) {
  }

  bool operator<(const HalServiceImplVersion& other) const {
    QCRIL_LOG_DEBUG("major: %u, minor: %u local: %u", (unsigned)mMajor, (unsigned)mMinor,
                    (unsigned)mLocal);
    QCRIL_LOG_DEBUG("o.major: %u, o.minor: %u o.local: %u", (unsigned)other.mMajor,
                    (unsigned)other.mMinor, (unsigned)other.mLocal);
    return mMajor >= other.mMajor && mMinor >= other.mMinor && mLocal >= other.mLocal;
  }
  bool operator>=(const HalServiceImplVersion& other) const {
    return (!((*this) < (other)));
  }
  std::string toString() {
    std::string os;
    os += "{";
    os += ".mMajor=" + std::to_string(mMajor);
    os += ".mMinor=" + std::to_string(mMinor);
    os += ".mLocal=" + std::to_string(mLocal);
    os += "}";
    return os;
  }
};

template <class B>
class HalServiceImplFactory {

  template <class T>
  using FactMethod = android::sp<T> (HalServiceImplFactory::*)();

  template <class T>
  struct FactoryEntry {
    FactMethod<T> factMethod;
    HalServiceImplVersion version;
  };

  using SvcListType = std::list<FactoryEntry<B>>;

 public:
  template <class T>
  void registerImpl() {
    QCRIL_LOG_DEBUG("registerImpl");
    FactMethod<B> factMethod = &HalServiceImplFactory::getImpl<T>;
    SvcListType localList{ FactoryEntry<B>{ factMethod, T::getVersion() } };
    svcImplList.merge(localList, &halServiceCompare<B>);
  }

  class iterator {
   private:
    HalServiceImplFactory* implF;
    typename SvcListType::iterator mIt;

   public:
    iterator(HalServiceImplFactory* f, typename SvcListType::iterator i)
        : implF(f), mIt(i) {
    }
    iterator operator++() {
      auto i = *this;
      ++mIt;
      return i;
    }
    iterator operator++(int) {
      mIt++;
      return *this;
    }
    bool operator==(const iterator& rhs) {
      return (mIt == rhs.mIt);
    }
    bool operator!=(const iterator& rhs) {
      return (mIt != rhs.mIt);
    }
    android::sp<B> operator*() {
      android::sp<B> ret;
      FactMethod<B> m = mIt->factMethod;
      QCRIL_LOG_INFO("version: %s", mIt->version.toString().c_str());
      if (m && implF) {
        ret = (implF->*m)();
      }
      return ret;
    }
  };

  iterator begin() {
    return iterator(this, svcImplList.begin());
  }

  iterator end() {
    return iterator(this, svcImplList.end());
  }

 private:
  template <class T>
  android::sp<B> getImpl() {
    return android::sp<T>(new T());
  }

  template <class T>
  static inline bool halServiceCompare(const FactoryEntry<T>& first,
                                       const FactoryEntry<T>& second) {
    return first.version < second.version;
  };

  SvcListType svcImplList;
};

template <class T>
HalServiceImplFactory<T>& getHalServiceImplFactory() {
  static HalServiceImplFactory<T> sFactory{};
  return sFactory;
}
