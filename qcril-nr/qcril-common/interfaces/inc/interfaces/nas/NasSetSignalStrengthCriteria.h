/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <framework/SolicitedMessage.h>
#include <framework/Message.h>
#include <framework/add_message_id.h>
#include <framework/legacy.h>
#include <telephony/ril.h>

/******************************************************************************
#   Message Class: NasSetSignalStrengthCriteria
#   Sender: ConfigurationClients
#   Receiver: NasModule
#   Data transferred: A list of SignalStrengthCriteriaEntry
#   Usage: Client sends this message when wanting to set the reporting criteria
           for the signal strength reporting.
#******************************************************************************/

static inline std::string toString(RIL_RadioAccessNetworks ran) {
    std::string ret = "<Unknown>";
    switch(ran) {
        case GERAN:
            ret = "GERAN";
            break;
        case UTRAN:
            ret = "UTRAN";
            break;
        case EUTRAN:
            ret = "EUTRAN";
            break;
        case CDMA:
            ret = "CDMA";
            break;
        case IWLAN:
            ret = "IWLAN";
            break;
        case NGRAN:
            ret = "NGRAN";
            break;
        default:
            break;
    }
    return ret;
}

static inline std::string toString(RIL_SignalMeasureType type) {
    std::string ret = "<Unknown>";
    switch (type) {
    case MT_RSSI:
        ret = "RSSI";
        break;
    case MT_RSCP:
        ret = "RSCP";
        break;
    case MT_RSRP:
        ret = "RSRP";
        break;
    case MT_RSRQ:
        ret = "RSRQ";
        break;
    case MT_RSSNR:
        ret = "RSSNR";
        break;
    case MT_SSRSRP:
        ret = "SSRSRP";
        break;
    case MT_SSRSRQ:
        ret = "SSRSRQ";
        break;
    case MT_SSSINR:
        ret = "SSSINR";
        break;
    default:
        break;
    }

    return ret;
}

struct SignalStrengthCriteriaEntry
{
    RIL_RadioAccessNetworks ran;
    RIL_SignalMeasureType type;
    int32_t hysteresisMs;
    int32_t hysteresisDb; // hysteresis values around thresholds
    std::vector<int32_t> thresholds;
    bool isEnabled;
};

class NasSetSignalStrengthCriteria : public SolicitedMessage<RIL_Errno>,
    public add_message_id<NasSetSignalStrengthCriteria>
{
private:
    std::vector<SignalStrengthCriteriaEntry> mCriteria;
    bool disableAllTypes{false};

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_signal_strength_criteria";
    static constexpr int RSSI_DELTA = 50;
    static constexpr int RSCP_DELTA = 40;
    static constexpr int RSRP_DELTA = 60;
    static constexpr int RSRQ_DELTA = 20;
    static constexpr int RSSNR_DELTA = 40;

    NasSetSignalStrengthCriteria() = delete;
    inline ~NasSetSignalStrengthCriteria() {};

    inline NasSetSignalStrengthCriteria(std::weak_ptr<MessageContext> ctx, std::vector<SignalStrengthCriteriaEntry> criteria):
        SolicitedMessage<RIL_Errno>(MESSAGE_NAME, ctx, get_class_message_id()),
        mCriteria(criteria)
    {
    }

    std::vector<SignalStrengthCriteriaEntry> &getCriteria() { return mCriteria;}
    bool getDisableAllTypes() { return disableAllTypes;}
    void setDisableAllTypes() { disableAllTypes = true;}

    string dump()
    {
      string dump = mName + "{";
      for (const SignalStrengthCriteriaEntry &entry: mCriteria) {
          dump += "{ ran: " + toString(entry.ran);
          dump += " hysteresis db: " + std::to_string(entry.hysteresisDb);
          dump += " thresholds: { ";
          for (int32_t th: entry.thresholds) {
              dump += std::to_string(th) + ", ";
          }
          dump += "}";
      }
      return dump;
    }
};

