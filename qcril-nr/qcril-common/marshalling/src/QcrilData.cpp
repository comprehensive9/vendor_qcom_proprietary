/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
#include <telephony/ril.h>
#include <marshal/Dial.h>
 template <>
Marshal::Result Marshal::write<RIL_RadioAccessNetworks>(const RIL_RadioAccessNetworks &arg){
    switch(arg) {
        case RIL_RadioAccessNetworks::UNKNOWN:
            write(RIL_RadioAccessNetworks::UNKNOWN);
            break;
        case RIL_RadioAccessNetworks::GERAN:
            write(RIL_RadioAccessNetworks::GERAN);
            break;
        case RIL_RadioAccessNetworks::UTRAN:
            write(RIL_RadioAccessNetworks::UTRAN);
            break;
        case RIL_RadioAccessNetworks::EUTRAN:
            write(RIL_RadioAccessNetworks::EUTRAN);
            break;
        case RIL_RadioAccessNetworks::CDMA:
            write(RIL_RadioAccessNetworks::CDMA);
            break;
        case RIL_RadioAccessNetworks::IWLAN:
            write(RIL_RadioAccessNetworks::IWLAN);
            break;
    }
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_RadioAccessNetworks>(RIL_RadioAccessNetworks &arg) const{
    switch(arg) {
        case RIL_RadioAccessNetworks::UNKNOWN:
            read(RIL_RadioAccessNetworks::UNKNOWN);
            break;
        case RIL_RadioAccessNetworks::GERAN:
            read(RIL_RadioAccessNetworks::GERAN);
            break;
        case RIL_RadioAccessNetworks::UTRAN:
            read(RIL_RadioAccessNetworks::UTRAN);
            break;
        case RIL_RadioAccessNetworks::EUTRAN:
            read(RIL_RadioAccessNetworks::EUTRAN);
            break;
        case RIL_RadioAccessNetworks::CDMA:
            read(RIL_RadioAccessNetworks::CDMA);
            break;
        case RIL_RadioAccessNetworks::IWLAN:
            read(RIL_RadioAccessNetworks::IWLAN);
            break;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_RadioDataRequestReasons>(const RIL_RadioDataRequestReasons &arg){
    switch(arg) {
        case RIL_RadioDataRequestReasons::NORMAL:
            write(RIL_RadioDataRequestReasons::NORMAL);
            break;
        case RIL_RadioDataRequestReasons::SHUTDOWN:
            write(RIL_RadioDataRequestReasons::SHUTDOWN);
            break;
        case RIL_RadioDataRequestReasons::HANDOVER:
            write(RIL_RadioDataRequestReasons::HANDOVER);
            break;
    }
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_RadioDataRequestReasons>(RIL_RadioDataRequestReasons &arg) const{
    switch(arg) {
        case RIL_RadioDataRequestReasons::NORMAL:
            read(RIL_RadioDataRequestReasons::NORMAL);
            break;
        case RIL_RadioDataRequestReasons::SHUTDOWN:
            read(RIL_RadioDataRequestReasons::SHUTDOWN);
            break;
        case RIL_RadioDataRequestReasons::HANDOVER:
            read(RIL_RadioDataRequestReasons::HANDOVER);
            break;
    }
    return Result::SUCCESS;
}
*/

