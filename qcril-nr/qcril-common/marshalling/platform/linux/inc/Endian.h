/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef QCRIL_ENDIAN_INCLUDED
#define QCRIL_ENDIAN_INCLUDED

#include <endian.h>
#include <climits>
#define TO_NW16(_value) htobe16(_value)
#define TO_NW32(_value) htobe32(_value)
#define TO_NW64(_value) htobe64(_value)
#define FROM_NW16(_value) be16toh(_value)
#define FROM_NW32(_value) be32toh(_value)
#define FROM_NW64(_value) be64toh(_value)
#define TO_NWINT64(_value) ((int64_t)htobe64((int64_t)_value))
#define FROM_NWINT64(_value) ((int64_t)be64toh((int64_t)_value))

#if (WORD_BIT == 32)
#define TO_NWINT(_value) ((int)htobe32((uint32_t)_value))
#define FROM_NWINT(_value) ((int)be32toh((uint32_t)_value))
#endif

#endif
