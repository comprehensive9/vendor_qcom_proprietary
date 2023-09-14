/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/ims/QcRilRequestImsGeoLocationInfoMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

static void freeGeolocationInfoBuffer(RIL_IMS_GeolocationInfo& geolocationInfo) {
    delete[] geolocationInfo.countryCode;
    delete[] geolocationInfo.country;
    delete[] geolocationInfo.state;
    delete[] geolocationInfo.city;
    delete[] geolocationInfo.postalCode;
    delete[] geolocationInfo.street;
    delete[] geolocationInfo.houseNumber;
    geolocationInfo.countryCode = nullptr;
    geolocationInfo.country = nullptr;
    geolocationInfo.state = nullptr;
    geolocationInfo.city = nullptr;
    geolocationInfo.postalCode = nullptr;
    geolocationInfo.street = nullptr;
    geolocationInfo.houseNumber = nullptr;
}

void dispatchImsSendGeolocationInfo(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_FUNC_ENTRY();
    RIL_IMS_GeolocationInfo geolocationInfo{};

    if (p.read(geolocationInfo) != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Ims Geolocation Info is empty");
        sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    auto msg = std::make_shared<QcRilRequestImsGeoLocationInfoMessage>(context);
    if(msg == nullptr){
        QCRIL_LOG_ERROR("msg is nullptr");
        sendResponse(context, RIL_E_NO_MEMORY, nullptr);
        freeGeolocationInfoBuffer(geolocationInfo);
        QCRIL_LOG_FUNC_RETURN();
        return;
    }

    msg->setLatitude(geolocationInfo.latitude);
    msg->setLongitude(geolocationInfo.longitude);
    msg->setCountryCode(geolocationInfo.countryCode);
    msg->setCountry(geolocationInfo.country);
    msg->setState(geolocationInfo.state);
    msg->setCity(geolocationInfo.city);
    msg->setPostalCode(geolocationInfo.postalCode);
    msg->setStreet(geolocationInfo.street);
    msg->setHouseNumber(geolocationInfo.houseNumber);

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              sendResponse(context, resp->errorCode, nullptr);
          } else {
              sendResponse(context, RIL_E_INTERNAL_ERR, nullptr);
          }
    });

    msg->setCallback(&cb);
    msg->dispatch();
    freeGeolocationInfoBuffer(geolocationInfo);
    QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
