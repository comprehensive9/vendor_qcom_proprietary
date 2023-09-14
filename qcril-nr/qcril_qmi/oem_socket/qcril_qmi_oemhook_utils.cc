/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qcril_qmi_oemhook_utils.h"
#include <string.h>

namespace oemhook {
namespace utils {

uint8_t* constructOemHookRaw
(
  uint32_t* length,
  int unsol_event,
  uint8_t* data,
  uint32_t data_len
)
{
    size_t payload_len = (QCRIL_QMI_OEM_INT_VAL_LEN + QCRIL_OTHER_OEM_NAME_LENGTH +
                          sizeof(unsol_event) + sizeof(data_len) + data_len);
    uint8_t *payload = new uint8_t[payload_len];
    uint32_t index = 0;
    if ( NULL != payload )
    {
        /* Unsolicited oemhook message has following message format
          [RIL_UNSOL_OEM_HOOK_RAW (4)]
          [OEM_NAME(8)] [Message Id (4) ] [Payload Length (4) ] [Payload]
        */
        int unsol_hook_raw = RIL_UNSOL_OEM_HOOK_RAW;
        memcpy( payload, &(unsol_hook_raw), QCRIL_QMI_OEM_INT_VAL_LEN );
        index += QCRIL_QMI_OEM_INT_VAL_LEN;

        memcpy( &payload[index], QCRIL_HOOK_OEM_NAME, QCRIL_OTHER_OEM_NAME_LENGTH );
        index += QCRIL_OTHER_OEM_NAME_LENGTH;

        memcpy( &payload[index], &unsol_event, sizeof(unsol_event) );
        index += sizeof(unsol_event);

        memcpy( &payload[index], &data_len, sizeof(data_len) );
        index += sizeof(data_len);

        if( data!= NULL && data_len > 0 ) {
            memcpy( &payload[index], data, data_len );
        }
        index += data_len;
    }

    *length = index;
    return payload;
}

bool dataLengthCheck(uint32_t data_index, uint32_t data_len) {
  if( data_index >= data_len) {
    return false;
  }
  return true;
}

} // namespace utils
} // namespace oemhook
