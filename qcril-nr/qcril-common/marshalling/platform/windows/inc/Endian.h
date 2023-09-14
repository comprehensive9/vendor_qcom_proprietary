/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef QCRIL_ENDIAN_INCLUDED
#define QCRIL_ENDIAN_INCLUDED

//#include <climits>
#include <algorithm>
#include <type_traits>
#define TO_NW16(_value) reversebytes<uint16_t>(_value)
#define TO_NW32(_value) reversebytes<uint32_t>(_value)
#define TO_NW64(_value) reversebytes<uint64_t>(_value)
#define FROM_NW16(_value) reversebytes<uint16_t>(_value)
#define FROM_NW32(_value) reversebytes<uint32_t>(_value)
#define FROM_NW64(_value) reversebytes<uint64_t>(_value)
#define TO_NWINT64(_value) (reversebytes<uint64_t>(_value)
#define FROM_NWINT64(_value) (reversebytes<uint64_t>(_value)

template < typename T,
         std::enable_if_t<std::is_integral<T>::value, int> = 0
>
T reversebytes(T &value) {
    char *ptr = reinterpret_cast<char *>(&value);
    char *end = ptr + sizeof(value);
    std::reverse(ptr,end);
    return value;
}

#define TO_NWINT(_value) reversebytes<int>(_value)
#define FROM_NWINT(_value) reversebytes<int>(_value)

#endif
