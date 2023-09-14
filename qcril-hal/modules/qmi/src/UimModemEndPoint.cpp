/******************************************************************************
#  @file    UimModemEndPoint.cpp
#  @brief   Base class source file for QMI UIM module
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "modules/uim/UimModemEndPoint.h"
#include "QmiUimSetupRequestSync.h"
#include "UimModemEndPointModule.h"

#define TAG "UimModemEndPoint"

/*=========================================================================

  Function:  UimModemEndPoint::UimModemEndPoint

===========================================================================*/
UimModemEndPoint::UimModemEndPoint() : ModemEndPoint("Uim")
{
  mModule = new UimModemEndPointModule("UimModemEndPointModule", this);

  if (mModule != nullptr)
  {
    mModule->init();
  }
} /* UimModemEndPoint::UimModemEndPoint */


/*=========================================================================

  Function:  UimModemEndPoint::requestSetupSync

===========================================================================*/
Message::Callback::Status UimModemEndPoint::requestSetupSync
(
  std::shared_ptr<string>& sharedResponse
)
{
  Message::Callback::Status ret = Message::Callback::Status::SUCCESS;
  QCRIL_LOG_INFO( "%s - %d \n", __FUNCTION__, getState());
  if (getState() != ModemEndPoint::State::OPERATIONAL)
  {
    auto setupMsg = std::make_shared<QmiUimSetupRequestSync>("QMI_UIM-Module");
    if(setupMsg != nullptr) {
        return (setupMsg->dispatchSync(sharedResponse));
    }
    else {
        return Message::Callback::Status::FAILURE;
    }
  }
  else
  {
    sharedResponse = std::make_shared<string>("Service already operational");
  }
  return ret;
} /* UimModemEndPoint::requestSetupSync */


/*=========================================================================

  Function:  UimModemEndPoint::cleanUp

===========================================================================*/
void UimModemEndPoint::cleanUp()
{
  if (mModule != NULL)
  {
    ((UimModemEndPointModule*)mModule)->cleanUpQmiSvcClient();    
  }
  setState(ModemEndPoint::State::NON_OPERATIONAL);
} /* UimModemEndPoint::cleanUp */
