/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef NASRFBANDINFOINDMESSAGE
#define NASRFBANDINFOINDMESSAGE

#include "network_access_service_v01.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class NasRfBandInfoIndMessage: public UnSolicitedMessage,
                         public add_message_id<NasRfBandInfoIndMessage> {

private:
    nas_rf_band_info_ind_msg_v01 nasRfBandInfo;

public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_NAS_RF_BAND_INFO_INDICATION";

    NasRfBandInfoIndMessage() = delete;
    ~NasRfBandInfoIndMessage() {}

    NasRfBandInfoIndMessage(nas_rf_band_info_ind_msg_v01 const &rfBandInfo) : UnSolicitedMessage(get_class_message_id())
    {
        mName = MESSAGE_NAME;
        nasRfBandInfo = rfBandInfo;
    }

    std::shared_ptr<UnSolicitedMessage> clone()
    {
        return std::make_shared<NasRfBandInfoIndMessage>(nasRfBandInfo);
    }

    int32_t getNasRFActiveBand()
    {
        if (nasRfBandInfo.rf_band_info_list_ext_valid && nasRfBandInfo.rf_band_info_list_ext.radio_if == NAS_RADIO_IF_NR5G_V01)
        {
            return (int32_t)toRilNgranBand(nasRfBandInfo.rf_band_info_list_ext.active_band);
        }
        else
        {
            return (int32_t)(NasActiveBand_t::NGRAN_BAND_MIN_ENUM_VAL);
        }
    }

    string dump()
    {
        std::stringstream ss;
        ss << MESSAGE_NAME;

        ss << " : rf_band_info.rf_band_info.radio_if = " <<nasRfBandInfo.rf_band_info.radio_if;
        ss << " : rf_band_info.rf_band_info.active_band = " <<nasRfBandInfo.rf_band_info.active_band;
        ss << " : rf_band_info.rf_band_info.active_channel = " <<nasRfBandInfo.rf_band_info.active_channel;

        if( nasRfBandInfo.rf_dedicated_band_info_valid ) {
           ss << " : nasRfBandInfo.rf_dedicated_band_info.radio_if = " <<nasRfBandInfo.rf_dedicated_band_info.radio_if;
           ss << " : nasRfBandInfo.rf_dedicated_band_info.dedicated_band = " <<nasRfBandInfo.rf_dedicated_band_info.dedicated_band;
        }

        if( nasRfBandInfo.rf_band_info_list_ext_valid ) {
          ss << " : rf_band_info.active_band = " << nasRfBandInfo.rf_band_info_list_ext.active_band;
          ss << " : rf_band_info.active_channel = " << nasRfBandInfo.rf_band_info_list_ext.active_channel;
          ss << " : rf_band_info.radio_if = " << nasRfBandInfo.rf_band_info_list_ext.radio_if;
        }

        if( nasRfBandInfo.nas_rf_bandwidth_info_valid ) {
          ss << " : rf_band_info.nas_rf_bandwidth_info.bandwidth = " << nasRfBandInfo.nas_rf_bandwidth_info.bandwidth;
          ss << " : rf_band_info.nas_rf_bandwidth_info.radio_if = " << nasRfBandInfo.nas_rf_bandwidth_info.radio_if;
        }

        if (nasRfBandInfo.ciot_lte_op_mode_valid)
          ss << " : ciot_lte_op_mode = " << nasRfBandInfo.ciot_lte_op_mode;

        return ss.str();
    }
};

/** NasRfBandInfoMessage.
 * This message is sent by the Nas module to notify of changes in the
 * NAS layer RF BAND INFO
 */
class NasRfBandInfoMessage : public UnSolicitedMessage,
                                 public add_message_id<NasRfBandInfoMessage>
{
private:
    NasActiveBand_t bandInfo;

public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_RF_BAND_INFO";

    NasRfBandInfoMessage() = delete;

    NasRfBandInfoMessage(NasActiveBand_t rfBandInfo) : UnSolicitedMessage(get_class_message_id()), bandInfo(rfBandInfo)
    {
        mName = MESSAGE_NAME;
    }
    ~NasRfBandInfoMessage() {}

    NasActiveBand_t getRfBandInfo()
    {
        return bandInfo;
    }
    std::shared_ptr<UnSolicitedMessage> clone()
    {
        return nullptr;
    }

    string dump()
    {
        return mName;
    }
};
} //namespace
#endif