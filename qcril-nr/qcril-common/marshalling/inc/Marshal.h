/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 *   Not a contribution.
 *
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef QCRIL_MARSHAL_INCLUDED
#define QCRIL_MARSHAL_INCLUDED

#include <Platform.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <Endian.h>
#include <QtiRilStringUtils.h>
#include <type_traits>
#include <vector>


#define RUN_AND_CHECK(action)              \
  (({                                      \
    Marshal::Result __ret = action;          \
    if (__ret != Marshal::Result::SUCCESS) { \
      return __ret;                          \
    }                                      \
  }))

#define WRITE_AND_CHECK(arg,...) RUN_AND_CHECK(write(arg, ##__VA_ARGS__))
#define READ_AND_CHECK(arg,...) RUN_AND_CHECK(read(arg, ##__VA_ARGS__))

#define RIL_likely(x) (__builtin_expect(!!(x), true))

class Marshal {
    private:
        std::string mData;
        mutable size_t current_pos = 0;

        template <typename T>
        static T toNetwork(T value);
        template <typename T>
        static T fromNetwork(T value);

    public:
        enum class Result {
            SUCCESS,
            FAILURE,
            OVERFLOWED,
            TRUNCATED,
            NOT_ENOUGH_DATA,
        };

        Marshal():
            mData(),
            current_pos(0)
        {}
        Marshal(const Marshal &o):
            mData(o.mData),
            current_pos(o.current_pos)
        {}
        Marshal(Marshal &&o):
            mData(std::move(o.mData)),
            current_pos(std::move(o.current_pos))
        {}

        Marshal& operator += (const Marshal& m) {
            size_t sz = m.mData.size() - m.current_pos;
            ensureAvailable(sz);
            std::copy(m.mData.begin() + m.current_pos, m.mData.end(), mData.begin() + current_pos);
            current_pos += sz;
            return *this;
        }


        operator std::string () const {
            return toString();
        }

        std::string toString() const {
            return mData;
        }

        operator const std::string &() {
            return mData;
        }

        const uint8_t *data() const {
            return reinterpret_cast<const uint8_t *>(mData.c_str());
        }

        size_t dataSize() const {
            return mData.size();
        }

        size_t dataAvail() const {
            return mData.size() - current_pos;
        }

        size_t dataPosition() const {
            return current_pos;
        }

        size_t dataCapacity() const {
            return mData.capacity();
        }

        int32_t setDataSize(size_t size) {
            mData.resize(size);
            return 0;
        }

        void setDataPosition(size_t pos) {
            if (pos <= mData.size()) {
                current_pos = pos;
            }
        }

        int32_t setDataCapacity(size_t size) {
            mData.reserve(size);
            return 0;
        }

        int32_t setData(const std::string data) {
            this->mData = data;
            current_pos = 0;
            return 0;
        }

        void freeData() {
            mData.resize(0);
            current_pos = 0;
        }

        /**
         * write: Main template used for non-arithmetic types. No default implementation provided
         * which means that any type must provide an explicit specialization with full
         * implementation.
         */
        template <typename T, std::enable_if_t< !std::is_arithmetic<T>::value &&
            !std::is_enum<T>::value, int> = 0>
        Result write(const T &value);

        /**
         * write: Main template used for arithmetic types. A default implementation is provided
         * which will convert the value into network format by calling toNetwork, and then
         * call writeNoSwap<T> to write the value into the buffer.
         */

        template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 1>
        Result write(const T &value) {
            T lvalue = toNetwork(value);
            return writeNoSwap(const_cast<const T &>(lvalue));
        }

        template <typename T>
        struct remove_cvref {
            using type = typename std::remove_reference<typename std::remove_cv<T>::type>::type;
        };

        /**
         * write: Main template for pointer types. Only specialized for T = char
         */
        template <typename T>
        Result write(const T* const &arg);

        /**
         * read: Main template for pointer types. Only specialized for T = char
         */
        template <typename T>
        Result read(T*&arg) const;

        template <typename T, template <typename...> typename Container>
        Result write(const Container<T> &arg) {
            if (arg.size() > std::numeric_limits<int32_t>::max()) {
                return Result::FAILURE;
            }
            int32_t outsz = arg.size();
            WRITE_AND_CHECK(outsz);
            for(const T &i: arg) {
                WRITE_AND_CHECK(i);
            }
            return Result::SUCCESS;
        }

        template <typename T, template <typename...> typename Container>
        Result read(Container<T> &arg) const {
            int32_t insz = 0;
            READ_AND_CHECK(insz);
            if (insz >= 0) {
                Container<T> tmp{};
                for(int i = 0; i < insz; i++) {
                    T item;
                    READ_AND_CHECK(item);
                    tmp.push_back(item);
                }
                arg = std::move(tmp);
            }
            return Result::SUCCESS;
        }


        /**
         * write: Main template for fixed array types. The default implementation forwards to
         * write(T*, SZ);
         */
        template <typename T, std::size_t SZ, typename BT = typename remove_cvref<T>::type,
                 std::enable_if_t<!std::is_same<BT, char>::value,int> = 0>
        Result write(const T (&arg)[SZ]) {
            return write<BT>(static_cast<const BT *>(arg), SZ);
        }


        /**
         * write: Limiting version of the fixed-array type write. This will write at most sz elements
         * from arg into the Marshal. sz must be smaller than SZ.
         */
        template <typename T, std::size_t SZ, typename BT = typename remove_cvref<T>::type,
                 std::enable_if_t<!std::is_same<BT, char>::value,int> = 0>
        Result write(const T (&arg)[SZ], size_t sz) {
            if (sz > SZ) {
                return Result::OVERFLOWED;
            }
            return write<BT>(static_cast<const BT *>(arg), sz);
        }

        template <std::size_t SZ>
        Result write(const char (&arg)[SZ]) {
            if (SZ > std::numeric_limits<int32_t>::max()) {
                return Result::FAILURE;
            }

            const char* end = std::char_traits<char>::find(arg, SZ, '\x00');
            size_t size = SZ;
            if (end) {
                size = end - arg;
            }
            return write<char>(static_cast<const char*>(arg), size);
        }

        template <std::size_t SZ>
        Result read(char (&arg)[SZ]) const {
            char *tmp = static_cast<char *>(arg);
            int32_t size = 0;

            READ_AND_CHECK(size);

            if (size < -1) return Result::FAILURE;
            if (size <= 0) {
                arg[0] = 0;
                return Result::SUCCESS;
            }

            size_t rem = 0;
            if (size > SZ - 1) {
                rem = size - (SZ - 1);
                size = SZ - 1;
            }

            RUN_AND_CHECK(read<char>(tmp, size));
            tmp[size] = 0;
            if (rem) {
                if (isEnoughToRead(rem)) {
                    current_pos += rem;
                } else {
                    return Result::NOT_ENOUGH_DATA;
                }
            }
            return Result::SUCCESS;
        }

        template <typename T, std::size_t SZ, typename BT = typename remove_cvref<T>::type,
                 std::enable_if_t<!std::is_same<BT, char>::value,int> = 0>
        Result read(T (&arg)[SZ], size_t &actualsz) const {
            BT *tmp = static_cast<BT *>(arg);
            int32_t sz = 0;
            READ_AND_CHECK(sz);

            if (sz < -1) return Result::FAILURE;
            if (sz <= 0) {
                actualsz = 0;
                return Result::SUCCESS;
            }

            size_t rem = 0;
            if (sz > SZ) {
                rem = sz - SZ;
                sz = SZ;
            }

            RUN_AND_CHECK( readNoReadSize<BT>(tmp, sz));

            actualsz = sz;

            if (rem) {
                if (isEnoughToRead(rem)) {
                    current_pos += rem;
                } else {
                    return Result::NOT_ENOUGH_DATA;
                }
            }
            return Result::SUCCESS;
        }

        /**
         * read: Main template for fixed array types. The default implementation forwards to
         * read(T*, SZ);
         */
        template <typename T, std::size_t SZ, typename BT = typename remove_cvref<T>::type,
                 std::enable_if_t<!std::is_same<BT, char>::value,int> = 0>
        Result read(T (&arg)[SZ]) const {
            BT *tmp = static_cast<BT *>(arg);
            return read<BT>(tmp, SZ);
        }

        /**
         * write: Main template for pointer types with size. The default implementation
         * repeatedly calls write<T>(value) for a total of sz elements
         */
         template <typename T>
         Result write(const T * const &value, size_t size) {
             int32_t osz = 0;
             if (value) {
                 osz = size;
                 write(osz);
                 std::for_each(value, value + size, [this] (const T &val) {
                             write(val);
                         });
             } else {
                 write<int32_t>(-1);
             }
             return Result::SUCCESS;
        }

    private:
         template <typename T, std::enable_if_t<!std::is_arithmetic<T>::value, int> = 0>
         Result readNoReadSize(T *&arg, size_t size) const {
             for (size_t i = 0; i < size; i++) {
                READ_AND_CHECK(arg[i]);
             }
             return Result::SUCCESS;
         }

         template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
         Result readNoReadSize(T *&arg, size_t size) const {
            if (!isEnoughToRead(sizeof(T) * size))
                return Result::NOT_ENOUGH_DATA;
            std::for_each(arg, arg + size, [this] (T &val) { rawRead(val); });
            return Result::SUCCESS;
         }

    public:

        /**
         * read: Main template for pointer types with size. The default implementation
         * reads the size first, and repeatedly calls read<T>(arg) for a total of sz elements.
         * The caller has to provide
         * pre-allocated sz number of elements of type T in arg.
         * Any additional elements remaining in the Marshal object (as per the read size) are
         * discarded.
         * If sz is greater than the size read from the stream, the remaining allocated elements
         * are uninitialized
         */
        template <typename T>
        Result read(T *&arg, size_t sz) const {
            int32_t size;
            read(size);
            if (size == -1) {
                arg = nullptr;
                return Result::SUCCESS;
            } else if (size < 0) {
                return Result::FAILURE;
            }

            size_t rem = 0;
            if (size > sz) {
                rem = size - sz;
                size = sz;
            }
            RUN_AND_CHECK(readNoReadSize(arg, size));

            if (rem) {
                if (isEnoughToRead(rem)) {
                    current_pos += rem;
                } else {
                    return Result::NOT_ENOUGH_DATA;
                }
            }
            return Result::SUCCESS;
        }

        /**
         * readAndAlloc: This function will first read a size, and it will allocate an array
         * of size elements of type T, and repeatedly call read<T>(array[i]) for a total of
         * sz elements.
         * The corresponding array is stored in arg, and its size in sz.
         */
        template <typename T>
        Result readAndAlloc(T *&arg, size_t &sz) const;

        /**
         * availableBytes: Return the number of bytes currently available without having to
         * resize
         */
        size_t availableBytes() const {
            return mData.size() - current_pos;
        }

        /**
         * ensureAvailable: Resize the data buffer so that at least len bytes are available
         * for writes * starting from the current position. No-op if there are already
         * enough bytes available
         */
        void ensureAvailable(size_t len) {
            if (availableBytes() < len) {
                mData.resize(current_pos + len);
            }
        }

        bool isEnoughToRead(size_t bytes) const {
            bool ret = false;
            if (availableBytes() >= bytes) {
                ret = true;
            }
            return ret;
        }
        template <typename T>
        bool isEnoughToRead() const {
            return isEnoughToRead(sizeof(T));
        }

        void *writeInplace(size_t len) {
            void *ret = nullptr;
            ensureAvailable(len);
            ret = &*(mData.begin() + current_pos);
            current_pos += len;
            return ret;
        }

        int32_t writeInt8(int8_t val) {
            write(val);
            return 0;
        }
        int32_t writeUint8(uint8_t val) {
            write(val);
            return 0;
        }
        int32_t writeInt16(int16_t val) {
            write(val);
            return 0;
        }
        int32_t writeUint16(uint16_t val) {
            write(val);
            return 0;
        }
        int32_t writeInt32(int32_t val) {
            write(val);
            return 0;
        }
        int32_t writeUint64(uint64_t val) {
            write(val);
            return 0;
        }
        int32_t writeFloat(float val) {
            write(val);
            return 0;
        }
        int32_t writeDouble(double val) {
            write(val);
            return 0;
        }
        int32_t writeCString(const char *str) {
            size_t len = -1;
            if (str) {
                len = strlen(str);
            }
            return writeCString(str, len);
        }
        int32_t writeCString(const char *str, size_t insz) {
            int32_t len = -1;
            if (str) {
                const char *nullchr = static_cast<const char *>(memchr(str, 0, insz));
                size_t actual_len = insz;
                if (nullchr != nullptr) {
                    ptrdiff_t diff = nullchr - str;
                    actual_len = static_cast<size_t>(diff); // signed positive value to unsigned
                }
                if (actual_len <= std::numeric_limits<int32_t>::max()) {
                    len = static_cast<int32_t>(actual_len);
                } else {
                    return -1;
                }
            }
            if (len < -1) return -1;
            write(len);
            if (len == -1 || len == 0) return 0;
            ensureAvailable(len);
            std::copy(str, str + len, mData.begin() + current_pos);
            current_pos += len;
            return 0;
        }
        int32_t writeString16(const std::u16string s) {
            std::for_each(s.begin(), s.end(), [this] (uint16_t c) {
                    write(c);
            });
            return 0;
        }

        const void *readInplace(size_t len) const {
            void *ret = nullptr;
            if(len <= availableBytes()) {
                ret = (void *)&*(mData.begin() + current_pos);
            }
            return ret;
        }
        int32_t readInt8(int8_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t)read(*out);
            }
            return ret;
        }
        int32_t readUint8(uint8_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t)read(*out);
            }
            return ret;
        }
        int32_t readInt16(int16_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t)read(*out);
            }
            return ret;
        }
        int32_t readUint16(uint16_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t)read(*out);
            }
            return ret;
        }
        int32_t readInt32(int32_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }
        int32_t readInt32() const {
            int32_t out = 0;
            read(out);
            return out;
        }
        int32_t readUint32(uint32_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }
        uint32_t readUint32() const {
            uint32_t out = 0;
            read(out);
            return out;
        }
        int64_t readInt64() const {
            int64_t out = 0;
            read(out);
            return out;
        }
        int32_t readInt64(int64_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }
        uint64_t readUint64() const {
            uint64_t out = 0;
            read(out);
            return out;
        }
        int32_t readUint64(uint64_t *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }
        float readFloat() const {
            float out = 0;
            read(out);
            return out;
        }
        int32_t readFloat(float *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }
        double readDouble() const {
            double out = 0;
            read (out);
            return out;
        }
        int32_t readDouble(float *out) const {
            int32_t ret = 1;
            if (out) {
                return (int32_t) read(*out);
            }
            return ret;
        }

        int32_t writeBool(bool val);
        int32_t readBool(bool* out) const;
        bool readBool() const;

        const char *readCString() const;

        /**
         * writeNoSwap: Main template used for arithmetic types. No template is provided
         * for non-arithmetic types, as this is expected to be used for arithmetic types
         * only. A default implementation is provided * which will write the value into
         * the buffer without endianness conversion.
         */
        template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 1>
        Result writeNoSwap(T value) {
            const char *s = reinterpret_cast<const char *>(&value);
            size_t length = sizeof(value);

            ensureAvailable(length);
            std::copy(s,
                    s + length,
                    mData.begin() + current_pos);
            current_pos += length;

            return Result::SUCCESS;
        }

        template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 1>
        Result readNoSwap(T& output) const {
            std::string s;

            std::copy(mData.begin() + current_pos,
                    mData.begin() + current_pos + sizeof(output),
                    s.begin());
            current_pos += sizeof(output);
            output = *reinterpret_cast<const T *>(s.c_str());
            return Result::SUCCESS;
        }

        /**
         * read: Main template used for non-arithmetic types. No default implementation provided
         * which means that any type must provide an explicit specialization with full
         * implementation.
         */
        template <typename T, std::enable_if_t< !std::is_arithmetic<T>::value &&
            !std::is_enum<T>::value, int> = 0>
        Result read(T& output) const;

        /**
         * read: Main template used for arithmetic types. A default implementation is provided
         * which will call readNoSwap<T> to read the value from the buffer, and then convert it
         * from network format into host format by calling fromNetwork<t>
         */
        template <typename T, std::enable_if_t< std::is_arithmetic<T>::value, int> = 1>
        Result read(T& output) const{
            if (RIL_likely(isEnoughToRead(sizeof(T))))
                return rawRead(output);
            return Result::NOT_ENOUGH_DATA;
        }

        template <typename T, std::enable_if_t< std::is_arithmetic<T>::value, int> = 1>
        Result rawRead(T& output) const{
            T tmp;
            Result r = readNoSwap(tmp);
            if (r == Result::SUCCESS) {
                tmp = fromNetwork(tmp);
                output = tmp;
            }
            return r;
        }
        using charptr = char *;

        template <typename T, std::enable_if_t<std::is_enum<T>::value, int> = 2>
        Result write(const T&arg) {
            static_assert(sizeof(typename std::underlying_type<T>::type) <= sizeof(int32_t), "Currently enums larger than 32 bits are not supported by Marshal");
            WRITE_AND_CHECK(static_cast<int32_t>(arg));
            return Result::SUCCESS;
        }
        template <typename T, std::enable_if_t<std::is_enum<T>::value, int> = 2>
        Result read(T &arg) const {
            static_assert(sizeof(typename std::underlying_type<T>::type) <= sizeof(int32_t), "Currently enums larger than 32 bits are not supported by Marshal");
            if (!isEnoughToRead(sizeof(int32_t))) {
                return Result::NOT_ENOUGH_DATA;
            }
            int32_t tmp = 0;
            READ_AND_CHECK(tmp);
            arg = static_cast<T>(tmp);
            return Result::SUCCESS;
        }

        /**
         * release: Main template used for all non fundamental types. No default implementation
         * provided which means that any type must provide an explicit specialization with full
         * implementation.
         */
        template <typename T,
                  std::enable_if_t<!std::is_fundamental<T>::value && !std::is_enum<T>::value, int> = 1>
        Result release(T& arg);

        /**
         * release: Main template used for all fundamental types. A default implementation is
         * provided which do no operation for basic types
         */
        template <typename T, std::enable_if_t<std::is_fundamental<T>::value, int> = 2>
        Result release(T &arg) {
          // No operation
          (void)arg;
          return Result::SUCCESS;
        }

        /**
         * release: Main template used for all enum types. A default implementation is
         * provided which do no operation for enums types
         */
        template <typename T, std::enable_if_t<std::is_enum<T>::value, int> = 3>
        Result release(T &arg) {
          // No operation
          (void)arg;
          return Result::SUCCESS;
        }

        /**
         * release: Main template for pointer types.
         */
        template <typename T>
        Result release(T *&arg, size_t sz);

        /**
         * release: Main template for fixed array types.
         */
        template <typename T, std::size_t SZ>
        Result release(T (&arg)[SZ], size_t sz);

        /**
         * release: Main template for pointer types. Only specialized for T = char and uint8_t
         */
        template <typename T>
        Result release(T *&arg);
};

template <>
inline Marshal::Result Marshal::write<char>(const char * const &arg);

template <>
inline Marshal::Result Marshal::write<char>(const char * const &arg, std::size_t sz);

template <>
inline Marshal::Result Marshal::write<char *>(char * const &arg) {
    return write<char>(arg);
}
template <>
inline Marshal::Result Marshal::read<char>(char *&out, size_t length) const;

template <>
inline Marshal::Result Marshal::readAndAlloc<char>(char *&arg, size_t &sz) const;

template <>
Marshal::Result Marshal::read(std::string &out) const;

template <>
Marshal::Result Marshal::write<char>(const char * const &arg);
template <>
Marshal::Result Marshal::read<char>(char *&out) const;
template <>
Marshal::Result Marshal::release<char>(char *&arg);

template <>
Marshal::Result Marshal::write(const char * const &arg, size_t sz);

template <>
Marshal::Result Marshal::release<uint8_t>(uint8_t *&arg);

// template <>
// Marshal::Result Marshal::write(const std::string &str);

// template <>
// Marshal::Result Marshal::write(const Marshal::charptr &str);

template <>
  unsigned char Marshal::toNetwork(unsigned char value);
template <>
  unsigned char Marshal::fromNetwork(unsigned char value);

template <>
          char *Marshal::toNetwork(char *value);
template <>
          char *Marshal::fromNetwork(char *value);

template <>
       uint16_t Marshal::toNetwork(uint16_t value);
template <>
       uint16_t Marshal::fromNetwork(uint16_t value);

template <>
       uint32_t Marshal::toNetwork(uint32_t value);
template <>
       uint32_t Marshal::fromNetwork(uint32_t value);

template <>
            int Marshal::toNetwork(int value);
template <>
            int Marshal::fromNetwork(int value);

template <>
            double Marshal::toNetwork(double value);
template <>
            double Marshal::fromNetwork(double value);

template <>
       uint64_t Marshal::toNetwork(uint64_t value);
template <>
       uint64_t Marshal::fromNetwork(uint64_t value);

template <>
       int64_t Marshal::toNetwork(int64_t value);
template <>
       int64_t Marshal::fromNetwork(int64_t value);

template <>
inline unsigned char Marshal::toNetwork(unsigned char value) {
    return value;
}

template <>
inline unsigned char Marshal::fromNetwork(unsigned char value) {
    return value;
}

template <>
inline char Marshal::toNetwork(char value) {
    return value;
}

template <>
inline char Marshal::fromNetwork(char value) {
    return value;
}

template <>
inline char *Marshal::toNetwork(char *value) {
    return value;
}

template <>
inline char *Marshal::fromNetwork(char *value) {
    return value;
}

template <>
inline std::string Marshal::toNetwork(std::string value) {
    return value;
}
template <>
inline uint16_t Marshal::toNetwork(uint16_t value) {
    return TO_NW16(value);
}

template <>
inline uint16_t Marshal::fromNetwork(uint16_t value) {
    return FROM_NW16(value);
}

template <>
inline uint32_t Marshal::toNetwork(uint32_t value) {
    return TO_NW32(value);
}

template <>
inline uint32_t Marshal::fromNetwork(uint32_t value) {
    return FROM_NW32(value);
}

template <>
inline int Marshal::toNetwork(int value) {
    return TO_NWINT(value);
}

template <>
inline int Marshal::fromNetwork(int value) {
    return FROM_NWINT(value);
}

template <>
inline double Marshal::toNetwork(double value) {
    static_assert(sizeof(double)==sizeof(uint64_t));
    uint64_t tmp = TO_NW64(*reinterpret_cast<uint64_t*>(&value));
    return *reinterpret_cast<double*>(&tmp);
}

template <>
inline double Marshal::fromNetwork(double value) {
    static_assert(sizeof(double)==sizeof(uint64_t));
    uint64_t tmp = FROM_NW64(*reinterpret_cast<uint64_t*>(&value));
    return *reinterpret_cast<double*>(&tmp);
}

template <>
inline uint64_t Marshal::toNetwork(uint64_t value) {
    return TO_NW64(value);
}

template <>
inline uint64_t Marshal::fromNetwork(uint64_t value) {
    return FROM_NW64(value);
}

template <>
inline int64_t Marshal::toNetwork(int64_t value) {
    return TO_NWINT64(value);
}

template <>
inline int64_t Marshal::fromNetwork(int64_t value) {
    return FROM_NWINT64(value);
}

template <>
inline Marshal::Result Marshal::read(std::string &out) const {
    uint32_t length = 0;
    Result res = read(length);
    if (res == Result::SUCCESS) {
        if (isEnoughToRead(length)) {
            out.resize(0);
            out.insert(0, mData, current_pos, length);
            current_pos += length;
        } else {
            return Result::NOT_ENOUGH_DATA;
        }
    } else {
        return Result::FAILURE;
    }

    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::write<char>(const char * const &arg) {
    size_t length = 0;
    Result res = Result::FAILURE;
    if (!arg) {
        return write<int32_t>(-1);
    }
    length = strlen(arg);

    res = write(arg, length);
    return res;
}

template <>
inline Marshal::Result Marshal::write(const char *const (&arg), std::size_t sz) {
    int32_t outsz = -1;
    if (sz > std::numeric_limits<int32_t>::max()) {
        return Result::FAILURE;
    }
    if (arg) {
        outsz = static_cast<int32_t>(sz);
        WRITE_AND_CHECK(outsz);
        ensureAvailable(sz);
        std::copy(arg, arg + sz, mData.begin() + current_pos);
        current_pos += sz;
    } else {
        WRITE_AND_CHECK(outsz);
    }
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::read(char *&arg, std::size_t sz) const {
    if (!arg) {
        return Result::FAILURE;
    }

    if (sz == 0) {
        return Result::SUCCESS;
    }

    if (isEnoughToRead(sz)) {
        std::copy(mData.begin() + current_pos, mData.begin() + current_pos + sz, arg);
        current_pos += sz;
    } else {
        return Result::NOT_ENOUGH_DATA;
    }
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::read<char>(char *&out) const{
    size_t length = 0;
    char *val;
    int32_t inlen = 0;
    auto res = Result::SUCCESS;

    READ_AND_CHECK(inlen);

    if (inlen == -1) {
        out = nullptr;
    } else if (inlen < 0) {
        res = Result::FAILURE;
    } else {
        length = inlen;
        val = new char[length + 1]{};
        RUN_AND_CHECK(read(val, length));
        out = val;
    }
    return res;
}

template <typename T>
inline Marshal::Result Marshal::release(T *&arg, size_t sz) {
    if (arg) {
        for (size_t i = 0; i < sz; i++) {
            release(arg[i]);
        }
    }
    delete []arg;
    arg = nullptr;
    return Result::SUCCESS;
}

template <typename T, std::size_t SZ>
inline Marshal::Result Marshal::release(T (&arg)[SZ], size_t sz) {
    if (sz > SZ) {
        return Result::FAILURE;
    }
    for (size_t i = 0; i < sz; i++) {
        release(arg[i]);
    }
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::release<char>(char *&arg) {
    delete []arg;
    arg = nullptr;
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::release<uint8_t>(uint8_t *&arg) {
    delete []arg;
    arg = nullptr;
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::readAndAlloc(char *(&arg), std::size_t &sz) const {
    int32_t size = 0;
    READ_AND_CHECK(size);
    if (size == -1) {
        arg = nullptr;
        sz = 0;
        return Result::SUCCESS;
    }
    if (size < 0) return Result::FAILURE;

    char *out = new char [size + 1]{};
    if (!out) return Result::FAILURE;
    auto res = read(out, size);
    if (res != Marshal::Result::SUCCESS) {
        delete []out;
        return res;
    }
    arg = out;
    sz = size;
    return Result::SUCCESS;
}

template <typename T>
inline Marshal::Result Marshal::readAndAlloc(T *&arg, size_t &outlen) const {
    int32_t len;
    READ_AND_CHECK(len);

    if (len < -1) return Result::FAILURE;
    if (len <= 0) {
        arg = nullptr;
        outlen = 0;
        return Result::SUCCESS;
    }

    // check availability for arithmetic type before hand
    // for other types, check availability one by one
    if constexpr(std::is_arithmetic_v<T>) {
        if (!isEnoughToRead(sizeof(T) * len))
            return Result::NOT_ENOUGH_DATA;
    }

    T *out = new T [len]{};
    if (!out) return Result::FAILURE;

    for (size_t i = 0; i < len; i++ ) {
        Marshal::Result res;
        if constexpr(std::is_arithmetic_v<T>)
            res = rawRead(out[i]);
        else
            res = read(out[i]);
        if (res != Marshal::Result::SUCCESS) {
            delete []out;
            return res;
        }
    }
    arg = out;
    outlen = len;
    return Result::SUCCESS;
}

template <>
inline Marshal::Result Marshal::write(const std::string &str) {
    uint32_t osz = str.size();
    if (str.size() > std::numeric_limits<uint32_t>::max()) {
        return Result::OVERFLOWED;
    }
    WRITE_AND_CHECK(osz);
    ensureAvailable(str.size());
    std::copy(str.begin(), str.end(), mData.begin() + current_pos);
    current_pos += str.length();
    return Result::SUCCESS;
}

inline const char *Marshal::readCString() const {
    char *s = nullptr;
    size_t sz = 0;
    int32_t insz = 0;
    read(insz);
    if (insz >= 0) {
        sz = insz;
        s = new char[sz + 1]{};
        read(s, sz);
    }
    return s;
}

template <>
inline Marshal::Result Marshal::write(const bool& arg) {
    return write(static_cast<uint8_t>(arg));
}

template <>
inline Marshal::Result Marshal::read(bool& arg) const {
    uint8_t val = 0;
    if (!isEnoughToRead(1)) return Result::NOT_ENOUGH_DATA;
    READ_AND_CHECK(val);
    arg = val;
    return Result::SUCCESS;
}

inline int32_t Marshal::writeBool(bool val) {
    return static_cast<int32_t>(write(val));
}

inline int32_t Marshal::readBool(bool* out) const {
    if (out) {
        return static_cast<int32_t>(read(*out));
    }
    return 1;
}

inline bool Marshal::readBool() const {
    bool out = false;
    read(out);
    return out;
}

#endif
