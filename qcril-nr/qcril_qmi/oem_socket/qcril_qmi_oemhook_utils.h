/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>

/* Length of the integer values used in OEM messages */
#define QCRIL_QMI_OEM_INT_VAL_LEN               4

#define QCRIL_OTHER_OEM_NAME_LENGTH      8            /* 8 bytes */
#define QCRIL_OTHER_OEM_REQUEST_ID_LEN   4            /* 4 bytes */
#define QCRIL_OTHER_OEM_REQUEST_DATA_LEN 4            /* 4 bytes */
#define QCRIL_OTHER_OEM_ITEMID_LEN       4            /* 4 bytes */
#define QCRIL_OTHER_OEM_ITEMID_DATA_LEN  4            /* 4 bytes */

#define QCRIL_HOOK_HEADER_SIZE           16
#define QCRIL_HOOK_OEM_NAME              "QOEMHOOK"


namespace oemhook {
namespace utils {

uint8_t* constructOemHookRaw
(
  uint32_t* length,
  int unsol_event,
  uint8_t* data,
  uint32_t data_len
);

bool dataLengthCheck(uint32_t data_index, uint32_t data_len);
}
}
