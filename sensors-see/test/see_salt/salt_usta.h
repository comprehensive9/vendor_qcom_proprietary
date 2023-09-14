/* ===================================================================
** Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: salt_usta.h
** DESC: interface from salt to usta
** ================================================================ */
#pragma once

#include "see_salt.h"
#include "sensor.h"     // sensors-see/usta/native/sensor.h

salt_rc usta_get_sensor_list(see_salt *psalt,
                             std::vector<sensor_info>& sensor_list);
salt_rc usta_get_request_list(see_salt *psalt,
                              unsigned int handle);
salt_rc usta_get_sttributes(see_salt *psalt,
                            unsigned int handle,
                            std::string& attribute_list);
salt_rc usta_send_request(see_salt *psalt,
                          unsigned int handle,
                          see_client_request_message &client_req);
salt_rc usta_stop_request(see_salt *psalt,
                          unsigned int handle,
                          bool delete_or_disconnect);

salt_rc usta_register_event_cb(see_salt *psalt, unsigned int handle,
                               event_cb_func event_cb);

void usta_destroy_instance( see_salt *psalt);


// Direct Channel Interface
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
salt_rc usta_create_dc_channel( see_salt *psalt, see_create_dc_channel_info &dc_channel_info, unsigned long &channel_handle);

salt_rc usta_close_dc_channel(see_salt *psalt, unsigned long &channel_handle);

salt_rc usta_send_request_dc_channel(see_salt *psalt, unsigned long &channel_handle, see_dc_std_req_info &req_info, bool is_delete_request_info);

salt_rc usta_update_offset_ts_dc_channel(see_salt *psalt, unsigned long &channel_handle, int64_t offset);
#endif

void usta_get_direct_channel_attributes(see_salt *psalt, direct_channel_attributes &dc_attributes);
