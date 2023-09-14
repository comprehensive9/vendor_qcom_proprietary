/*
 * Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/qmi/ModemEndPointFactory.h"

#include "DSDModemEndPoint.h"
#include "PreferredDataStates.h"
#include "UnSolMessages/DDSSwitchIPCMessage.h"
#include "UnSolMessages/IAInfoIPCMessage.h"
#include "sync/TriggerDDSSwitchSyncMessage.h"
#include "request/SetPreferredDataModemRequestMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "sync/SetInitialAttachCompletedMessage.h"
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/VoiceIndMessage.h"
#include "legacy/qcril_data.h"
#include "UnSolMessages/DDSSwitchTimeoutMessage.h"
#include "DataCommon.h"

#include "SMLog.h"

using namespace SM;
using namespace rildata;
using std::placeholders::_1;

extern pthread_mutex_t ddsSubMutex;
extern DDSSubIdInfo currentDDSSUB;

PreferredDataState::PreferredDataState(
    string name,
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> stateInfo
): IState(name, fn), mStateInfo(stateInfo), ddsSwitchRequestTimer(TimeKeeper::no_timer)
{
  mStateInfo->iaPending = 0;
}

InitialState::InitialState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Initial", fn, info)
{}

InitializedState::InitializedState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Initialized", fn, info)
{}

SwitchingState::SwitchingState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Switching", fn, info)
{}

DefaultDataState::DefaultDataState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("DefaultData", fn, info)
{}

NonDefaultDataState::NonDefaultDataState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("NonDefaultData", fn, info)
{}

ApTriggeredState::ApTriggeredState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("ApTriggered", fn, info)
{}

MpTriggeredState::MpTriggeredState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("MpTriggered", fn, info)
{}

string PreferredDataEventType::getEventName() const {
    switch (event) {
        case PreferredDataEventEnum::SetPreferredDataModem:
            return "SetPreferredDataModem";
        case PreferredDataEventEnum::InitializeSM:
            return "InitializeSM";
        case PreferredDataEventEnum::DDSSwitchInd:
            return "DDSSwitchInd";
        case PreferredDataEventEnum::CurrentDDSInd:
            return "CurrentDDSInd";
        case PreferredDataEventEnum::DDSSwitchTimerExpired:
            return "DDSSwitchTimerExpired";
        case PreferredDataEventEnum::DDSSwitchApStarted:
            return "DDSSwitchApStarted";
        case PreferredDataEventEnum::DDSSwitchCompleted:
            return "DDSSwitchCompleted";
        case PreferredDataEventEnum::DDSSwitchMpStarted:
            return "DDSSwitchMpStarted";
        case PreferredDataEventEnum::TempDDSSwitchTimerExpired:
            return "TempDDSSwitchTimerExpired";
        case PreferredDataEventEnum::IAStarted:
            return "IAStarted";
        case PreferredDataEventEnum::IACompleted:
            return "IACompleted";
        case PreferredDataEventEnum::IAStartRequest:
            return "IAStartRequest";
        case PreferredDataEventEnum::IAStartResponse:
            return "IAStartResponse";
        case PreferredDataEventEnum::VoiceServiceDown:
            return "VoiceServiceDown";
        default:
            return "UNKNOWN";
    }
}

void PreferredDataState::ddsTransition() {
    int nextState = (mStateInfo->dds == mStateInfo->sub) ? DefaultData : NonDefaultData;
    transitionTo(nextState);
}

void PreferredDataState::sendResponse(std::shared_ptr<Message> msg, SetPreferredDataModemResult_t error) {
    SetPreferredDataModemResponse_t response;
    auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(msg);
    response.errCode = error;
    if (m != nullptr) {
        SM_LOGD("responding to SetPreferredDataModem with error=" +
                             std::to_string(static_cast<int>(error)));
        auto r = make_shared<SetPreferredDataModemResponse_t>(response);
        if (error == SetPreferredDataModemResult_t::NO_ERROR) {
            m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
        } else {
            m->sendResponse(m, Message::Callback::Status::FAILURE, r);
        }
    } else {
        SM_LOGE("Could not send response. Message is null");
    }
}

static void handleLockMutex( pthread_mutex_t &ddsSubMutex )
{
    LOCK_MUTEX(ddsSubMutex);
}

static void handleUnlockMutex( pthread_mutex_t &ddsSubMutex )
{
    UNLOCK_MUTEX(ddsSubMutex);
}

void PreferredDataState::onDDSSwitchRequestExpired(void *)
{
    Log::getInstance().d("PreferredDataState::onDDSSwitchRequestExpired ENTRY");
    DDSTimeOutSwitchType type = DDSTimeOutSwitchType::DDSTimeOutSwitch;
    auto msg = std::make_shared<DDSSwitchTimeoutMessage>(type);
    msg->broadcast();
}

int InitialState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    switch (e.event) {
        case InitializeSM:
        {
            DDSSubIdInfo ddsInfo;
            ddsInfo.dds_sub_id = QCRIL_INVALID_MODEM_STACK_ID;
            ddsInfo.switch_type = DSD_DDS_DURATION_ENUM_MIN_ENUM_VAL_V01;
            // get current dds, switch to DefaultData or NonDefaultData
            auto status = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->getCurrentDDSSync(ddsInfo);
            if (status == Message::Callback::Status::SUCCESS) {

                handleLockMutex(ddsSubMutex);
                currentDDSSUB.dds_sub_id = ddsInfo.dds_sub_id;
                currentDDSSUB.switch_type = ddsInfo.switch_type;
                handleUnlockMutex(ddsSubMutex);
                Log::getInstance().d("Current DDS is on Sub ="+std::to_string(currentDDSSUB.dds_sub_id)+
                                     " and Switch type = "+std::to_string(currentDDSSUB.switch_type));

                //Post DDS SUB Info to RIL Common
                auto msg = std::make_shared<rildata::NewDDSInfoMessage>(global_instance_id, ddsInfo);
                msg->broadcast();

                status = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->registerForCurrentDDSInd();
                if (status == Message::Callback::Status::SUCCESS) {
                    mStateInfo->dds = ddsInfo.dds_sub_id;
                    ddsTransition();
                } else {
                    SM_LOGE(getName() + ": Failed to register for current DDS ind");
                }
            } else {
                SM_LOGE(getName() + ": Failed to get current DDS ");
            }
            return HANDLED;
        }
        case SetPreferredDataModem:
        {
            DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
            SetPreferredDataModemResponse_t resp;
            resp.errCode = SetPreferredDataModemResult_t::INVALID_OPERATION;
            auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
            if (r != nullptr) {
                SM_LOGE("responding to SetPreferredDataModem with error=" +
                                    std::to_string(static_cast<int>(r->errCode)));
                m->sendResponse(m, Message::Callback::Status::FAILURE, r);
            }
            return HANDLED;
        }
        case IAStarted:
        {
            DDSSwitchInfo_t *iaStarted = static_cast<DDSSwitchInfo_t *>(e.data);
            if(mStateInfo->isRilIpcNotifier)
            {
              mStateInfo->iaPending++;
              auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(iaStarted->msg);
              auto cb = *((mStateInfo->pendingIACb).get());
              if(cb != nullptr)
              {
                cb(m);
              }
            }
            else
            {
               if(mStateInfo->iaPending && mStateInfo->iaMessage) {
                 /* If there is already a pending IA request, send failure response for that and store this new request*/
                 auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mStateInfo->iaMessage);
                 auto attachCompletedMsg = std::make_shared<SetInitialAttachCompletedMessage>();
                 attachCompletedMsg->setMsg(m);
                 attachCompletedMsg->setProcessIACompleted(false);
                 attachCompletedMsg->setStatus(false);
                 auto responseCb = *((mStateInfo->iaSendResponse).get());
                 if(responseCb != nullptr)
                 {
                    responseCb(attachCompletedMsg);
                 }
                 mStateInfo->iaMessage = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(iaStarted->msg);
                 if (!mStateInfo->isIPCprocessingOngoing) {
                    auto cb = *((mStateInfo->pendingIACb).get());
                    if(cb != nullptr && mStateInfo->iaMessage)
                    {
                        cb(mStateInfo->iaMessage);
                    }
                 }
               } else {
                 mStateInfo->iaPending++;
                 mStateInfo->isIPCprocessingOngoing = true;
                 auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
                 #ifdef QMI_RIL_UTF
                 usleep(3000);
                 #endif
                 msg->broadcast();
                 mStateInfo->iaMessage = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(iaStarted->msg);
               }
            }
            return HANDLED;
        }
        case IACompleted:
        {
            mStateInfo->iaPending = (mStateInfo->iaPending > 0) ? (mStateInfo->iaPending - 1) : 0;
            Log::getInstance().d("IAPending is " + std::to_string(mStateInfo->iaPending));
            //Inform master RIL about IA Complete on another RIL
            if(!mStateInfo->isRilIpcNotifier)
            {
              mStateInfo->iaMessage = nullptr;
              auto iaCompleted = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IACompleted);
              iaCompleted->broadcast();
            }
            return HANDLED;
        }
        case IAStartRequest:
        {
            //handled by master ril
            mStateInfo->iaPending++;
            auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
            #ifdef QMI_RIL_UTF
            usleep(3000);
            #endif
            msg->broadcast();
            return HANDLED;
        }
        case IAStartResponse:
        {
            //handled by slave ril
            if(mStateInfo->iaPending && mStateInfo->iaMessage)
            {
              mStateInfo->isIPCprocessingOngoing = false;
              auto cb = *((mStateInfo->pendingIACb).get());
              auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mStateInfo->iaMessage);
              if(cb != nullptr)
              {
                cb(m);
              }
            }
            return HANDLED;
        }
        default:
            break;
    }
    return HANDLED;
}

int InitializedState::handleEvent(const PreferredDataEventType& e) {
    DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    SM_LOGD(getName() + ": handling " + e.getEventName());
    switch (e.event) {
        case SetPreferredDataModem:
        {
            // save request msg so we can respond after CURRENT_DDS_IND arrives
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
            if (switchInfo->subId == mStateInfo->dds) {
                // respond with success
                SM_LOGD("[InitializedState]: responding to SetPreferredDataModem with success");
                SetPreferredDataModemResponse_t resp;
                resp.errCode = SetPreferredDataModemResult_t::NO_ERROR;
                auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
            } else {
                mStateInfo->mPendingMessage =
                    std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
                if(!mStateInfo->iaPending)
                {
                  transitionTo(ApTriggered);
                } else {
                  Log::getInstance().d("[InitializedState]: no of pending IA requests are " + std::to_string(mStateInfo->iaPending));
                  PendingEvents_t pendingEvent = {SetPreferredDataModem, m};
                  deferList.insertElement(pendingEvent);
                }
            }
            return HANDLED;
        }
        case DDSSwitchApStarted:
        {
            transitionTo(ApTriggered);
            return HANDLED;
            break;
        }
        case CurrentDDSInd:
        case DDSSwitchMpStarted:
        {
            if (switchInfo->subId == mStateInfo->dds) {
                SM_LOGD(getName() + ": already on the triggered dds sub. Ignoring " + e.getEventName());
            } else {
                // cache current dds
                mStateInfo->dds = switchInfo->subId;
                transitionTo(MpTriggered);
                SM_LOGD(getName() + " switch dds to sub " + std::to_string(mStateInfo->dds));
            }
            return HANDLED;
        }
        case TempDDSSwitchTimerExpired:
        {
            SM_LOGD("Temp DDS timer Expired");
            int error;
            mStateInfo->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
            SM_LOGD("Initiating Permanent DDS switch with sub" + std::to_string(mStateInfo->dds));
            ModemEndPointFactory<DSDModemEndPoint>::getInstance()
                         .buildEndPoint()->triggerDDSSwitchSync(mStateInfo->dds, error, mStateInfo->switch_type);
            return HANDLED;
        }
        case IAStarted:
        {
            if(mStateInfo->isRilIpcNotifier)
            {
              mStateInfo->iaPending++;
              auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(switchInfo->msg);
              auto cb = *((mStateInfo->pendingIACb).get());
              if(cb != nullptr)
              {
                cb(m);
              }
            }
            else
            {
               if(mStateInfo->iaPending && mStateInfo->iaMessage) {
                 /* If there is already a pending IA request, send failure response for that and store this new request*/
                 auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mStateInfo->iaMessage);
                 auto attachCompletedMsg = std::make_shared<SetInitialAttachCompletedMessage>();
                 attachCompletedMsg->setMsg(m);
                 attachCompletedMsg->setProcessIACompleted(false);
                 attachCompletedMsg->setStatus(false);
                 auto responseCb = *((mStateInfo->iaSendResponse).get());
                 if(responseCb != nullptr)
                 {
                    responseCb(attachCompletedMsg);
                 }
                 mStateInfo->iaMessage = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(switchInfo->msg);
                 if (!mStateInfo->isIPCprocessingOngoing) {
                    auto cb = *((mStateInfo->pendingIACb).get());
                    if(cb != nullptr && mStateInfo->iaMessage)
                    {
                        cb(mStateInfo->iaMessage);
                    }
                 }
               } else {
                 mStateInfo->iaPending++;
                 mStateInfo->isIPCprocessingOngoing = true;
                 auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
                 #ifdef QMI_RIL_UTF
                 usleep(3000);
                 #endif
                 msg->broadcast();
                 mStateInfo->iaMessage = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(switchInfo->msg);
               }
            }
            return HANDLED;
        }
        case IACompleted:
        {
            mStateInfo->iaPending = (mStateInfo->iaPending > 0) ? (mStateInfo->iaPending - 1) : 0;
            Log::getInstance().d("IAPending is " + std::to_string(mStateInfo->iaPending));
            //Inform master RIL about IA Complete on another RIL
            if(!mStateInfo->isRilIpcNotifier)
            {
              mStateInfo->iaMessage = nullptr;
              auto iaCompleted = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IACompleted);
              iaCompleted->broadcast();
            }

            #ifdef QMI_RIL_UTF
            if(mStateInfo->iaPending == 0)
            #else
            if(mStateInfo->iaPending == 0 && mStateInfo->isRilIpcNotifier)
            #endif
            {
              if(deferList.removeElement(SetPreferredDataModem))
              {
                #ifdef QMI_RIL_UTF
                mStateInfo->isRilIpcNotifier = TRUE;
                #endif
                transitionTo(ApTriggered);
              }
            }
            return HANDLED;
        }
        case IAStartRequest:
        {
            //handled by master ril
            #ifndef QMI_RIL_UTF
            mStateInfo->iaPending++;
            #endif
            auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
            msg->broadcast();
            return HANDLED;
        }
        case IAStartResponse:
        {
            //handled by slave ril
            if(mStateInfo->iaPending && mStateInfo->iaMessage)
            {
              mStateInfo->isIPCprocessingOngoing = false;
              auto cb = *((mStateInfo->pendingIACb).get());
              auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mStateInfo->iaMessage);
              if(cb != nullptr)
              {
                cb(m);
              }
            }
            return HANDLED;
        }
        default:
            break;
    }
    return UNHANDLED;
}

int DefaultDataState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

int NonDefaultDataState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

int SwitchingState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

void ApTriggeredState::enter() {
    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[ApTriggeredState]: broadcasting IPC switch started");
        // Send ipc message
        auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::ApStarted,
                                                        mStateInfo->dds);
        ipcStarted->broadcast();
        // send dds switch request to modem
        int error;
        Message::Callback::Status status = Message::Callback::Status::FAILURE;

        auto pendingMessage = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(mStateInfo->mPendingMessage);
        if (pendingMessage != nullptr) {
            status = ModemEndPointFactory<DSDModemEndPoint>::getInstance()
                        .buildEndPoint()->triggerDDSSwitchSync(pendingMessage->getModemId(), error, mStateInfo->switch_type);
            ddsSwitchRequestTimer = TimeKeeper::getInstance().set_timer(
                std::bind(&PreferredDataState::onDDSSwitchRequestExpired, this, std::placeholders::_1),
                nullptr,
                DDS_SWITCH_REQUEST_TIMEOUT);
            Log::getInstance().d("Started timer for dds switch. Return Success");
        } else {
            SM_LOGE("[ApTriggeredState]: pending SetPreferredDataModem message is null");
            error = static_cast<int>(SetPreferredDataModemResult_t::INVALID_ARG);
        }
        if (status != Message::Callback::Status::SUCCESS) {
            sendResponse(pendingMessage, static_cast<SetPreferredDataModemResult_t>(error));
            pendingMessage = nullptr;
        }
    }
}

void ApTriggeredState::exit() {

    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[ApTriggeredState]: broadcasting IPC switch Completed");
        // Send ipc message
        auto ipcCompleted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::Completed,
                                                            mStateInfo->dds);
        ipcCompleted->broadcast();
        TimeKeeper::getInstance().clear_timer(ddsSwitchRequestTimer);
    }

    if(!deferList.isEmptyList())
    {
      if(deferList.findElement(IAStarted))
      {
        auto msg = deferList.findMessageForEvent(IAStarted);
        auto cb = *((mStateInfo->pendingIACb).get());
        if(cb != nullptr)
        {
          cb(msg);
        }
        deferList.removeElement(IAStarted);
      }

      if(mStateInfo->iaPending)
      {
        if(deferList.removeElement(IAStartRequest))
        {
          auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
          msg->broadcast();
        }
      }
    }
}

int ApTriggeredState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    DDSSwitchInfo_t *switchInfo = nullptr;
    if(e.data != nullptr)
    {
      switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    }
    switch (e.event) {
        case CurrentDDSInd:
        {
            #ifdef QMI_RIL_UTF
              if (switchInfo != nullptr) {
            #else
              if (mStateInfo->isRilIpcNotifier && switchInfo != nullptr) {
            #endif
                mStateInfo->dds = switchInfo->subId;
                ddsTransition();
                sendResponse(mStateInfo->mPendingMessage, SetPreferredDataModemResult_t::NO_ERROR);
                mStateInfo->mPendingMessage = nullptr;
            }
            return HANDLED;
        }
        case DDSSwitchInd:
        {
            if (switchInfo != nullptr && switchInfo->switchAllowed == DDSSwitchRes::ALLOWED) {
                return HANDLED;
            }
            ddsTransition();

            if (switchInfo != nullptr && switchInfo->switchAllowed == DDSSwitchRes::FAIL) {
                SM_LOGD("[ApTriggeredState]: DDSSwitch failed");
                sendResponse(mStateInfo->mPendingMessage, SetPreferredDataModemResult_t::DDS_SWITCH_NOT_ALLOWED);
                mStateInfo->mPendingMessage = nullptr;
            }

            return HANDLED;
        }
        case DDSSwitchTimerExpired:
        {
            ddsTransition();
            sendResponse(mStateInfo->mPendingMessage, SetPreferredDataModemResult_t::DDS_SWITCH_TIMEOUT);
            mStateInfo->mPendingMessage = nullptr;
            return HANDLED;
        }
        case SetPreferredDataModem:
        {
            /* If SetPreferredDataModem is triggered while request is still pending,
               respond with error */
            if(switchInfo != nullptr)
            {
              std::shared_ptr<SetPreferredDataModemRequestMessage> m =
                      std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
              if (m != nullptr) {
                  SetPreferredDataModemResponse_t resp;
                  resp.errCode = SetPreferredDataModemResult_t::REQUEST_ALREADY_PENDING;
                  auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                  if (r != nullptr) {
                      SM_LOGE("responding to SetPreferredDataModem with error=" +
                                           std::to_string(static_cast<int>(r->errCode)));
                      m->sendResponse(m, Message::Callback::Status::FAILURE, r);
                  }
              }
            }
            return HANDLED;
        }
        case DDSSwitchCompleted:
        {
            if(switchInfo != nullptr)
            {
              mStateInfo->dds = switchInfo->subId;
              ddsTransition();
            }
            return HANDLED;
        }
        case IAStarted:
        {
            if(switchInfo != nullptr)
            {
              if(mStateInfo->isRilIpcNotifier)
              {
                std::shared_ptr<SetInitialAttachApnRequestMessage> m =
                        std::static_pointer_cast<SetInitialAttachApnRequestMessage>(switchInfo->msg);
                //If there is any pending IA request, send failure response for that before storing this new IA request.
                if(!deferList.isEmptyList() && deferList.findElement(IAStarted))
                {
                    SM_LOGD("Receive another IA request when older IA request is pending");
                    std::shared_ptr<SetInitialAttachApnRequestMessage> msg =
                            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(deferList.findMessageForEvent(IAStarted));
                    if(msg != nullptr)
                    {
                       RIL_Errno result = RIL_E_INTERNAL_ERR;
                       auto resp = std::make_shared<RIL_Errno>(result);
                       msg->sendResponse(msg, Message::Callback::Status::FAILURE, resp);
                    }
                    deferList.removeElement(IAStarted);
                }
                SM_LOGE("Storing the IA request in deferal List");
                PendingEvents_t pendingEvent = {IAStarted, m};
                deferList.insertElement(pendingEvent);
              }
              else
              {
                if(mStateInfo->iaPending && mStateInfo->iaMessage) {
                  /* If there is already a pending IA request, send failure response for that and store this new request*/
                  auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(mStateInfo->iaMessage);
                  auto attachCompletedMsg = std::make_shared<SetInitialAttachCompletedMessage>();
                  attachCompletedMsg->setMsg(m);
                  attachCompletedMsg->setProcessIACompleted(false);
                  attachCompletedMsg->setStatus(false);
                  auto responseCb = *((mStateInfo->iaSendResponse).get());
                  if(responseCb != nullptr)
                  {
                    responseCb(attachCompletedMsg);
                  }
                } else {
                  mStateInfo->iaPending++;
                  auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
                  #ifdef QMI_RIL_UTF
                  usleep(3000);
                  #endif
                  msg->broadcast();
                }
                mStateInfo->iaMessage = std::static_pointer_cast<SetInitialAttachApnRequestMessage>(switchInfo->msg);
              }
            }
            return HANDLED;
        }
        case IAStartRequest:
        {
            mStateInfo->iaPending++;
            PendingEvents_t pendingEvent = {IAStartRequest, nullptr};
            deferList.insertElement(pendingEvent);
            return HANDLED;
        }
        case VoiceServiceDown:
        {
            ddsTransition();
            sendResponse(mStateInfo->mPendingMessage, SetPreferredDataModemResult_t::DDS_SWITCH_FAILED);
            mStateInfo->mPendingMessage = nullptr;
            return HANDLED;
        }
        case IACompleted:
        {
            mStateInfo->iaPending = (mStateInfo->iaPending > 0) ? (mStateInfo->iaPending - 1) : 0;
            Log::getInstance().d("IAPending is " + std::to_string(mStateInfo->iaPending));
            //Inform master RIL about IA Complete on another RIL
            if(!mStateInfo->isRilIpcNotifier)
            {
              //This should never be the case
              mStateInfo->iaMessage = nullptr;
              auto iaCompleted = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IACompleted);
              iaCompleted->broadcast();
            }
            return HANDLED;
        }

        default:
            break;
    }
    SM_LOGD(getName() + ": ignored " + e.getEventName());
    return UNHANDLED;
}

void MpTriggeredState::enter() {
    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[ApTriggeredState]: broadcasting MP IPC switch started");
        // Send ipc message
        auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::MpStarted,
                                                             mStateInfo->dds);
        ipcStarted->broadcast();
    }
}

int MpTriggeredState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    switch (e.event) {
        case SetPreferredDataModem:
        {
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
            if (switchInfo->subId == mStateInfo->dds) {
                // Send ipc message
                auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::ApStarted,
                                                                   switchInfo->subId);
                ipcStarted->broadcast();
                // respond with success
                SM_LOGD(getName() + " responding to SetPreferredDataModem with success");
                SetPreferredDataModemResponse_t resp;
                resp.errCode = SetPreferredDataModemResult_t::NO_ERROR;
                auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
                ddsTransition();
            } else {
                // save request msg so we can respond after CURRENT_DDS_IND arrives
                mStateInfo->mPendingMessage = m;
                mStateInfo->dds = switchInfo->subId;
                transitionTo(ApTriggered);
            }
            return HANDLED;
        }
        case DDSSwitchApStarted:
            if (switchInfo->subId == mStateInfo->dds) {
                ddsTransition();
            } else {
                mStateInfo->dds = switchInfo->subId;
                transitionTo(ApTriggered);
            }
            return HANDLED;
        case CurrentDDSInd:
        case DDSSwitchMpStarted:
        {
            if (switchInfo->subId == mStateInfo->dds) {
                SM_LOGD(getName() + ": already on the triggered dds sub. Ignoring " + e.getEventName());
            } else {
                // cache current dds
                mStateInfo->dds = switchInfo->subId;
                if (mStateInfo->isRilIpcNotifier) {
                    SM_LOGD("[MpTriggeredState]: broadcasting MP IPC switch started");
                    // Send ipc message
                    auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::MpStarted,
                                                                       mStateInfo->dds);
                    ipcStarted->broadcast();
                }
                SM_LOGD(getName() + " switch dds to sub " + std::to_string(mStateInfo->dds));
            }
            return HANDLED;
        }
        default:
            break;
    }
    SM_LOGD(getName() + ": ignored " + e.getEventName());
    return UNHANDLED;
}
