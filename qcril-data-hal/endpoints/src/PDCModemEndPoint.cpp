/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "PDCModemEndPoint.h"

using rildata::PDCModemEndPoint;

constexpr const char *PDCModemEndPoint::NAME;

#ifdef RIL_FOR_DYNAMIC_LOADING
void PDCModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}
#else
void PDCModemEndPoint::requestSetup(string clientToken,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, callback);
  mModule->dispatch(shared_setupMsg);
}
#endif

void PDCModemEndPoint::registerForPDCIndication()
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[PDCModemEndPoint]: registerForPDCIndication");
    pdc_indication_register_req_msg_v01 req;
    pdc_indication_register_resp_msg_v01 resp;
    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.reg_client_refresh_valid = TRUE;
    req.reg_client_refresh = 1;

    auto rc = sendRawSync(QMI_PDC_INDICATION_REGISTER_REQ_V01,
                          (void *)&req, sizeof(req),
                          (void *)&resp, sizeof(resp),
                          QCRIL_DATA_QMI_TIMEOUT);

    if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      Log::getInstance().d("handleRegisterForPDCIndication(): failed with rc="+ std::to_string(rc) +
                              ", qmi_err=" + std::to_string(resp.resp.error));
    }
  }
}