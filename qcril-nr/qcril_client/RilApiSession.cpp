/******************************************************************************
#  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <limits>
#include <framework/Log.h>
#include <RilApiSession.hpp>
#include <marshal/CdmaSubscription.h>
#include <marshal/IMSConference.h>
#include <marshal/GsmBroadcastSms.h>

#define TAG "[RilApiSession]"

RilApiSession::RilApiSession(const std::string& socketPath)
    : socketPath(socketPath) {}

RilApiSession::RilApiSession(const std::string& ipAddress,
        const unsigned short tcpPort)
    : ipAddress(ipAddress), tcpPort(tcpPort) {}

RilApiSession::~RilApiSession() {
    deinitialize();
}

// TODO: return fd instead of setting socketFd directly
Status RilApiSession::createUnixSocket() {
    int sd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sd < 0) {
        return Status::FAILURE;
    }

    Log::getInstance().d("[RilApiSession]: Socket FD: " + std::to_string(sd));

    struct sockaddr_un socketAddress = {};
    socketAddress.sun_family = AF_LOCAL;

    if (strlcpy(socketAddress.sun_path, socketPath.c_str(),
            sizeof(socketAddress.sun_path)) >= sizeof(socketAddress.sun_path))
    {
        return Status::FAILURE;
    }

    if (connect(sd, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0) {
        Log::getInstance().d("[RilApiSession]: Failed to connect to the server. Error: " + std::string(strerror(errno)));
        return Status::FAILURE;
    }

    socketFd = sd;

    return Status::SUCCESS;
}

// TODO: return fd instead of setting socketFd directly
Status RilApiSession::createTcpIpSocket() {
    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        return Status::FAILURE;
    }

    struct sockaddr_in socketAddress {};
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    socketAddress.sin_port = htons(tcpPort);

    int err = connect(sd, (struct sockaddr*) &socketAddress,
            sizeof(socketAddress));
    if (err) {
        return Status::FAILURE;
    }

    socketFd = sd;

    return Status::SUCCESS;
}

Status RilApiSession::initialize(const RequestManager::ErrorCallback& errorCallback) {
    Status s = Status::FAILURE;

    Log::getInstance().d("[RilApiSession]: Initializing RIL API session.");

    {
        std::scoped_lock lock(socketFdMutex, reqMgrMutex);

        if (socketFd > -1) {
            return s;
        }

        if (ipAddress.empty()) {
            s = createUnixSocket();
        } else {
            s = createTcpIpSocket();
        }

        if (s != Status::SUCCESS) {
            return s;
        }

        reqMgr = std::make_unique<RequestManager>(socketFd, errorCallback);
    }

    return s;
}

Status RilApiSession::deinitialize() {
    {
        std::scoped_lock lock(socketFdMutex, reqMgrMutex);

        reqMgr.reset(nullptr);
        if (socketFd > -1) {
            close(socketFd);
            socketFd = -1;
        }
    }

    return Status::SUCCESS;
}

Status RilApiSession::reinitialize(const RequestManager::ErrorCallback& errorCallback) {
    deinitialize();
    return initialize(errorCallback);
}

Status RilApiSession::registerIndicationHandler(int32_t indicationId, const RequestManager::GenericIndicationHandler& indicationHandler) {
    if (reqMgr) {
        return reqMgr->registerIndicationHandler(indicationId, indicationHandler);
    }
    return Status::FAILURE;
}

// TODO: Have a common function to check if there's an active and valid
// TODO: RequestManager before issuing it the request.
Status RilApiSession::dial(const RIL_Dial& dialParams, const DialCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(dialParams);
            return reqMgr->issueRequest(RIL_REQUEST_DIAL, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::CarrierInfoForImsiEncryption(const RIL_CarrierInfoForImsiEncryption carrier,
    const RIL_PublicKeyType type, const CarrierInfoForImsiEncryptionCallback &cb) {
    Log::getInstance().d("[RilApiSession]: CarrierInfoForImsiEncryption enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if ((parcel->write(carrier) == Marshal::Result::FAILURE) || (parcel->write(type) == Marshal::Result::FAILURE)) {
                Log::getInstance().d("CarrierInfoForImsiEncryption: Failed to send . Marshal Write Failed.");
                return Status::FAILURE;
            }
            return reqMgr->issueRequest(RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: CarrierInfoForImsiEncryption before Read");
                    int res;
                    Marshal::Result status1 = p->read(res);
                    Status status = Status::FAILURE;
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    else {
                        cb(e, Status::FAILURE, nullptr);
                    }
                    RIL_Errno result = static_cast<RIL_Errno>(res);
                    Log::getInstance().d("[RilApiSession]: CarrierInfoForImsiEncryption before callback");
                    cb(e, status, &result);
                }
                else {
                    cb(e, Status::FAILURE, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: CarrierInfoForImsiEncryption exit");
    return Status::FAILURE;
}

Status RilApiSession::getCellInfo(const CellInfoCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(
                RIL_REQUEST_GET_CELL_INFO_LIST,
                std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    if (!p || p->dataAvail() == 0) {
                        cb(e, nullptr, 0);
                        return;
                    }

                    uint64_t _cellInfoListLen = 0;
                    if (p->read(_cellInfoListLen) != Marshal::Result::SUCCESS) {
                        cb(RIL_E_INVALID_RESPONSE, nullptr, 0);
                        return;
                    }

                    size_t cellInfoListLen = static_cast<size_t>(_cellInfoListLen);
                    RIL_CellInfo_v12* cellInfoList = new RIL_CellInfo_v12[cellInfoListLen];

                    if (cellInfoList == nullptr) {
                        cb(RIL_E_NO_MEMORY, nullptr, 0);
                        return;
                    }

                    if (p->read(cellInfoList, cellInfoListLen) == Marshal::Result::SUCCESS) {
                        cb(e, cellInfoList, cellInfoListLen);
                    } else {
                        cb(RIL_E_INVALID_RESPONSE, nullptr, 0);
                    }

                    delete[] cellInfoList;
                }
            );
        }
    }

    return Status::FAILURE;
}

Status RilApiSession::registerCarrierInfoForImsiHandler(const CarrierInfoForImsiHandler &indicationHandler) {
    return registerIndicationHandler(
    RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION,
    [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
        indicationHandler();
    });
}

Status RilApiSession::registerSignalStrengthIndicationHandler(const SignalStrengthIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_SIGNAL_STRENGTH,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            RIL_SignalStrength signalStrength;
            if (p->read(signalStrength) == Marshal::Result::SUCCESS) {
                indicationHandler(signalStrength);
            }
        }
    );
}

Status RilApiSession::registerCellInfoIndicationHandler(const CellInfoIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_CELL_INFO_LIST,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            uint64_t _cellInfoListLen = 0;
            if (p->read(_cellInfoListLen) != Marshal::Result::SUCCESS) {
                return;
            }

            size_t cellInfoListLen = static_cast<size_t>(_cellInfoListLen);
            RIL_CellInfo_v12* cellInfoList = new RIL_CellInfo_v12[cellInfoListLen];

            if (cellInfoList == nullptr) {
                return;
            }

            if (p->read(cellInfoList, cellInfoListLen) == Marshal::Result::SUCCESS) {
                indicationHandler(cellInfoList, cellInfoListLen);
            }

            delete[] cellInfoList;
        }
    );
}

Status RilApiSession::getVoiceRegStatus(const GetVoiceRegCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_VOICE_REGISTRATION_STATE, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_VoiceRegistrationStateResponse voiceReg = {};
                    if (p && p->dataAvail()) {
                        p->read(voiceReg);
                    } else {
                        // overwrite the ril error if no data
                        e = RIL_E_NO_MEMORY;
                    }
                    cb(e, voiceReg);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::getDataRegStatus(const GetDataRegCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_DATA_REGISTRATION_STATE, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_DataRegistrationStateResponse dataReg = {};
                    if (p && p->dataAvail()) {
                        p->read(dataReg);
                    } else {
                        // overwrite the ril error if no data
                        e = RIL_E_NO_MEMORY;
                    }
                    cb(e, dataReg);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::setIndicationFilter(int32_t filter, const VoidResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(filter);
            return reqMgr->issueRequest(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setPreferredNetworkType(int nwType, const SetNetworkTypeCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(nwType);
            return reqMgr->issueRequest(RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getPreferredNetworkType(const GetNetworkTypeCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    int nwType;
                    p->read(nwType);
                    cb(e, nwType);
                }
             });
    }
    return Status::FAILURE;
}


Status RilApiSession::getCurrentCalls(const GetCurrentCallsCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_GET_CURRENT_CALLS, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          int32_t numCalls = 0;
          RIL_Call **calls = nullptr;
          if (p && p->dataAvail()) {
            p->read(numCalls);
            if (numCalls) {
              calls = new RIL_Call*[numCalls]();
            }
            if(calls) {
              for (int i = 0; (i < numCalls) && p->dataAvail(); i++) {
                calls[i] = new RIL_Call;
                if(calls[i]) {
                  p->read(*calls[i]);
                }
              }
            }
          }
          cb(e, numCalls, (const RIL_Call **)calls);

          // Free the calls objects
          if (calls) {
            for (int32_t i = 0; i < numCalls; i++) {
              if (calls[i] && p) {
                p->release(*calls[i]);
              }
              delete calls[i];
            }
            delete calls;
          }
        });
  }
  return Status::FAILURE;
}
Status RilApiSession::answer(const AnswerCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_ANSWER, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::conference(const ConferenceCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_CONFERENCE, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::switchWaitingOrHoldingAndActive(const SwitchWaitingOrHoldingAndActiveCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::udub(const UdubCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_UDUB, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::hangupWaitingOrBackground(const HangupWaitingOrBackgroundCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::hangupForegroundResumeBackground(const HangupForegroundResumeBackgroundCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
  }
  return Status::FAILURE;
}
Status RilApiSession::separateConnection(int callId, const SeparateConnectionCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> p = std::make_unique<Marshal>();
    if (p != nullptr) {
      p->write(callId);
      return reqMgr->issueRequest(RIL_REQUEST_SEPARATE_CONNECTION, std::move(p),
          [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::shutDown(const ShutDownCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_SHUTDOWN, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setRadioCapability(const RIL_RadioCapability &radioCap,
    const SetRadioCapabilityCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr &&
            parcel->write(radioCap) == Marshal::Result::SUCCESS) {
            return reqMgr->issueRequest(RIL_REQUEST_SET_RADIO_CAPABILITY, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getRadioCapability(const GetRadioCapabilityCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_GET_RADIO_CAPABILITY,
                std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          if (p && p->dataAvail()) {
            RIL_RadioCapability radioCap;
            p->read(radioCap);
            cb(e, radioCap);
          }
        });
      }
  }
  return Status::FAILURE;
}

Status RilApiSession::registerRadioCapabilityChangedIndHandler(
    const RadioCapabilityIndHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RADIO_CAPABILITY,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            RIL_RadioCapability radioCap;
            if (p->read(radioCap) == Marshal::Result::SUCCESS) {
                indicationHandler(radioCap);
            }
        }
    );
}


Status RilApiSession::exitEmergencyCbMode(const ExitEmergencyCbModeCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setBandMode(const int& bandMode, const SetBandModeCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr &&
            parcel->write(bandMode) == Marshal::Result::SUCCESS) {
            return reqMgr->issueRequest(RIL_REQUEST_SET_BAND_MODE, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::queryAvailableBandMode(const QueryAvailableBandModeCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          int *bandMode = nullptr;
          if (p && p->dataAvail()) {
            int size = 0;
            p->read(size);
            if (size) {
              bandMode = new int[size+1]();
            }
            if (bandMode) {
              bandMode[0] = size;
            }
            for (int i = 1; (i < size+1) && p->dataAvail(); i++) {
              p->read(bandMode[i]);
            }
          }
          cb(e, bandMode);
          // free bandMode
          delete []bandMode;
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::captureRilDataDump(const GetRilDataDumpCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_DATA_DUMP, std::move(parcel),[cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                cb(e);
            });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getSignalStrength(const GetSignalStrengthCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_SIGNAL_STRENGTH, std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          if (p && p->dataAvail()) {
            RIL_SignalStrength signalStrength;
            p->read(signalStrength);
            cb(e, &signalStrength);
          }
        });
      }
  }
  return Status::FAILURE;
}

Status RilApiSession::radioPower(const bool& on, const RadioPowerCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write<int>(on);
            return reqMgr->issueRequest(RIL_REQUEST_RADIO_POWER, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::allowData(const bool& on, const AllowDataCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(on);
            return reqMgr->issueRequest(RIL_REQUEST_ALLOW_DATA, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getEnableSimStatus(const GetEnableSimStatusCallback& cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_REQUEST_GET_UICC_ENABLEMENT_STATUS, nullptr,
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    bool status = false;
                    p->read(status);
                    cb(e, status);
                }
             });
    }
    return Status::FAILURE;
}

Status RilApiSession::enableSim(const bool& on, const EnableSimCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(on);
            return reqMgr->issueRequest(RIL_REQUEST_ENABLE_UICC_APPLICATIONS, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setLocationUpdates(const bool& on, const SetLocationUpdatesCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(on);
            return reqMgr->issueRequest(RIL_REQUEST_SET_LOCATION_UPDATES, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getActivityInfo(const GetActivityInfoCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_GET_ACTIVITY_INFO, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_ActivityStatsInfo activityInfo {};
                    if (p){
                        p->read(activityInfo);
                    }
                    cb(e, activityInfo);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::VoiceRadioTech(const VoiceRadioTechCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_VOICE_RADIO_TECH, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    int voiceRadioTech;
                    p->read(voiceRadioTech);
                    cb(e, voiceRadioTech);
                }
             });
    }
    return Status::FAILURE;
}

Status RilApiSession::deviceIdentity(const DeviceIdentityCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_DEVICE_IDENTITY, std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          auto deviceIdentity = new char*[4]();
          if (p && p->dataAvail() && deviceIdentity) {
              p->read(deviceIdentity[0]);
              p->read(deviceIdentity[1]);
              p->read(deviceIdentity[2]);
              p->read(deviceIdentity[3]);
              cb(e, (const char **)deviceIdentity);

              if(deviceIdentity[0] != nullptr)
              {
                delete[] deviceIdentity[0];
              }
              if(deviceIdentity[1] != nullptr)
              {
                delete[] deviceIdentity[1];
              }
              if(deviceIdentity[2] != nullptr)
              {
                delete[] deviceIdentity[2];
              }
              if(deviceIdentity[3] != nullptr)
              {
                delete[] deviceIdentity[3];
              }
          }
          if(deviceIdentity)
          {
            delete[] deviceIdentity;
          }
      });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::operatorName(const OperatorNameCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_OPERATOR, std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          auto operName = new char*[3]();
          if (p && p->dataAvail() && operName) {
              p->read(operName[0]);
              p->read(operName[1]);
              p->read(operName[2]);
              cb(e, (const char **)operName);

              if(operName[0] != nullptr)
              {
                delete[] operName[0];
              }
              if(operName[1] != nullptr)
              {
                delete[] operName[1];
              }
              if(operName[2] != nullptr)
              {
                delete[] operName[2];
              }
          }
          if(operName)
          {
            delete[] operName;
          }
      });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::QueryNetworkSelectionMode(const QueryNetworkSelectionModeCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    int mode;
                    p->read(mode);
                    cb(e, mode);
                }
             });
    }
    return Status::FAILURE;
}

Status RilApiSession::setNetworkSelectionAutomatic(const SetNetworkSelectionAutomaticCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setNetworkSelectionManual(std::string mcc, std::string mnc,
        const SetNetworkSelectionAutomaticCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            std::string plmn = mcc + mnc;
            parcel->write(plmn.c_str());
            return reqMgr->issueRequest(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::basebandVersion(const BasebandVersionCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_BASEBAND_VERSION, std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
          char *version = nullptr;
          if (p && p->dataAvail()) {
              p->read(version);
              cb(e, version);
              p->release(version);
          }
      });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::hangup(const int& connid, const HangupCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write<int>(connid);
            return reqMgr->issueRequest(RIL_REQUEST_HANGUP, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::sendUssd(const std::string& ussd, const SendUssdCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(ussd.c_str());
            return reqMgr->issueRequest(RIL_REQUEST_SEND_USSD, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::cancelUssd(const SendUssdCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_CANCEL_USSD, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::dtmf(const char& dtmf, const DtmfCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(dtmf);
            return reqMgr->issueRequest(RIL_REQUEST_DTMF, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::dtmfStart(const char& dtmf, const DtmfStartCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(dtmf);
            return reqMgr->issueRequest(RIL_REQUEST_DTMF_START, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::dtmfStop(const DtmfStopCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_DTMF_STOP, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::lastCallFailCause(
    const LastCallFailCauseCallback& cb)
{
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_LAST_CALL_FAIL_CAUSE, std::move(parcel),
                [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
                    if (p && p->dataAvail()) {
                        RIL_LastCallFailCauseInfo lastCallFailCauseInfo{};
                        p->read(lastCallFailCauseInfo);
                        cb(e, lastCallFailCauseInfo);
                        p->release(lastCallFailCauseInfo);
                    }
                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::registerEnableSimStatusIndicationHandler(
        const EnableSimStatusIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                return;
            }
            bool status;
            if (p->read(status) == Marshal::Result::SUCCESS &&
                indicationHandler) {
                indicationHandler(status ? true : false);
            }
        }
    );
}

Status RilApiSession::registerCdmaSubscriptionSourceChangedIndicationHandler(
        const CdmaSubscriptionSourceChangedIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                return;
            }
            RIL_CdmaSubscriptionSource source = CDMA_SUBSCRIPTION_SOURCE_INVALID;
            if (p->read(source) == Marshal::Result::SUCCESS &&
                indicationHandler) {
                indicationHandler(source);
            }
        }
    );
}

Status RilApiSession::registerEmergencyCbModeIndHandler(
    const EmergencyCbModeIndHandler& indicationHandler) {
    Status res = Status::FAILURE;
    res = registerIndicationHandler(
        RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            if (indicationHandler != nullptr) {
                indicationHandler(RIL_EMERGENCY_CALLBACK_MODE_ENTER);
            }
        }
    );

    if (res == Status::SUCCESS) {
        res = registerIndicationHandler(
            RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,
            [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
                if (indicationHandler != nullptr) {
                    indicationHandler(RIL_EMERGENCY_CALLBACK_MODE_EXIT);
                }
            }
        );
    }
    return res;
}

Status RilApiSession::registerCdmaPrlChangedIndHandler(
    const CdmaPrlChangedHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_CDMA_PRL_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr || p->dataAvail() == 0) {
                return;
            }

            int prl;
            if (p->read(prl) == Marshal::Result::SUCCESS &&
                indicationHandler != nullptr) {
                indicationHandler(prl);
            }
        }
    );
}

Status RilApiSession::registerUiccSubStatusIndHandler(
    const UiccSubStatusChangedHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr || p->dataAvail() == 0) {
                return;
            }

            int status;
            if (p->read(status) == Marshal::Result::SUCCESS &&
                indicationHandler != nullptr) {
                indicationHandler(status);
            }
        }
    );
}

Status RilApiSession::registerRingBackToneIndicationHandler(
    const RingBackToneIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RINGBACK_TONE,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr || p->dataAvail() == 0) {
                return;
            }

            int operation;
            if (p->read(operation) == Marshal::Result::SUCCESS) {
                indicationHandler(operation);
            }
        }
    );
}

Status RilApiSession::registerImsNetworkStateChangeIndicationHandler(
    const ImsNetworkStateChangeIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            indicationHandler();
        }
    );
}

Status RilApiSession::explicitCallTransfer(const ExplicitCallTransferCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_EXPLICIT_CALL_TRANSFER, std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
      });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::setMute(const bool& mute, const SetMuteCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write<int>(mute);
            return reqMgr->issueRequest(RIL_REQUEST_SET_MUTE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getMute(const GetMuteCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_GET_MUTE, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    int mute;
                    p->read(mute);
                    cb(e, mute);
                }
             });
    }
    return Status::FAILURE;
}

Status RilApiSession::setClir(const int* clir, const SetClirCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write<int>(clir[0]) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_SET_CLIR, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getClir(const GetClirCallback &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_GET_CLIR, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p != nullptr) {
                    int m = -1 ,n = -1;
                    if(p->read(m) == Marshal::Result::SUCCESS &&
                       p->read(n) == Marshal::Result::SUCCESS) {
                        cb(e, m, n);
                    }
                }
            });
    }
    return Status::FAILURE;
}

Status RilApiSession::imsSendRttMessage(const char* msg, const size_t len,
            const SendRttCallback& cb) {
    if(!reqMgr) {
        return Status::FAILURE;
    }
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if(parcel != nullptr) {
        if(parcel->write(msg, len) == Marshal::Result::SUCCESS) {
            return reqMgr->issueRequest(RIL_IMS_REQUEST_SEND_RTT_MSG, std::move(parcel),
            [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::imsSendGeolocationInfo(const RIL_IMS_GeolocationInfo & geolocationInfo,
            const GeolocationInfoCallback& cb) {
    if(!reqMgr) {
        return Status::FAILURE;
    }
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if(parcel != nullptr) {
        if(parcel->write(geolocationInfo) == Marshal::Result::SUCCESS) {
            return reqMgr->issueRequest(RIL_IMS_REQUEST_SEND_GEOLOCATION_INFO,
            std::move(parcel), [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::registerImsRttMessageIndicationHandler(
    const ImsRttMessageIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_IMS_UNSOL_RTT_MSG_RECEIVED,
    [indicationHandler] (std::shared_ptr<Marshal> p) {
        if (p == nullptr) {
            return;
        }
        std::string rttMessage;
        if (p->read(rttMessage) == Marshal::Result::SUCCESS) {
            indicationHandler(rttMessage);
        }
    });
}

Status RilApiSession::registerImsGeolocationIndicationHandler(
    const ImsGeolocationIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_IMS_UNSOL_REQUEST_GEOLOCATION,
    [indicationHandler] (std::shared_ptr<Marshal> p) {
        if (p == nullptr) {
            return;
        }
        //Initialize with invalid values in case parcel does not get de-serialized.
        double latitude = -999;
        double longitude = -999;
        if (p->read(latitude) == Marshal::Result::SUCCESS &&
                p->read(longitude) == Marshal::Result::SUCCESS) {
            indicationHandler(latitude, longitude);
        }
    });
}

Status RilApiSession::registerImsUnsolRetrievingGeoLocationDataStatus(
    const RetrievingGeoLocationDataStatusHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        if (p == nullptr) {
          return;
        }
        RIL_IMS_GeoLocationDataStatus status = RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN;
        if (p->read(status) == Marshal::Result::SUCCESS &&
            status != RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN) {
          Log::getInstance().d("RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS");
          indicationHandler(status);
        }
      });
}

Status RilApiSession::registerImsVowifiQualityIndicationHandler(
    const ImsVowifiQualityIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_IMS_UNSOL_VOWIFI_CALL_QUALITY,
    [indicationHandler] (std::shared_ptr<Marshal> p) {
        if (p == nullptr) {
            return;
        }
        RIL_IMS_VowifiCallQuality vowifiCallQuality;
        if (p->read(vowifiCallQuality) == Marshal::Result::SUCCESS) {
            indicationHandler(vowifiCallQuality);
        }
    });
}

Status RilApiSession::queryCallForwardStatus(const RIL_CallForwardInfo& callFwdInfo,
                                             const QueryCallForwardStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(callFwdInfo);
      return reqMgr->issueRequest(
          RIL_REQUEST_QUERY_CALL_FORWARD_STATUS, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
            int32_t numCallFwdInfos = 0;
            RIL_CallForwardInfo** callFwdInfos = nullptr;
            if (p && p->dataAvail()) {
              p->read(numCallFwdInfos);
              if (numCallFwdInfos) {
                callFwdInfos = new RIL_CallForwardInfo*[numCallFwdInfos]();
              }
              if (callFwdInfos) {
                for (int i = 0; (i < numCallFwdInfos) && p->dataAvail(); i++) {
                  callFwdInfos[i] = new RIL_CallForwardInfo;
                  if (callFwdInfos[i]) {
                    p->read(*callFwdInfos[i]);
                  }
                }
              }
            }
            Log::getInstance().d("queryCallForwardStatus:resp: e=" + std::to_string(e));
            cb(e, numCallFwdInfos, (const RIL_CallForwardInfo**)callFwdInfos);
            // free callFwdInfos
            if (callFwdInfos) {
              for (int i = 0; i < numCallFwdInfos; i++) {
                if (callFwdInfos[i] && p) {
                  p->release(*callFwdInfos[i]);
                }
                delete callFwdInfos[i];
              }
            }
            delete []callFwdInfos;
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::setCallForward(const RIL_CallForwardInfo& callFwdInfo,
                                     const SetCallForwardStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(callFwdInfo);
      return reqMgr->issueRequest(
          RIL_REQUEST_SET_CALL_FORWARD, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("setCallForward:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::queryCallWaiting(const int& serviceClass, const QueryCallWaitingCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(serviceClass);
      return reqMgr->issueRequest(
          RIL_REQUEST_QUERY_CALL_WAITING, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
            int enabled = 0;
            int serviceClass = 0;
            if (p && p->dataAvail()) {
              p->read(enabled);
              p->read(serviceClass);
            }
            Log::getInstance().d("queryCallWaiting:resp: e=" + std::to_string(e));
            cb(e, enabled, serviceClass);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::setCallWaiting(const int& enabled, const int& serviceClass,
                                     const SetCallCallWaitingCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(enabled);
      parcel->write(serviceClass);
      return reqMgr->issueRequest(
          RIL_REQUEST_SET_CALL_WAITING, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("setCallWaiting:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::changeBarringPassword(const std::string& facility,
                                            const std::string& oldPassword,
                                            const std::string& newPassword,
                                            const ChangeBarringPasswordCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      if (facility.size()) {
        parcel->write(facility);
      } else {
        parcel->write((int32_t)-1);
      }
      if (oldPassword.size()) {
        parcel->write(oldPassword);
      } else {
        parcel->write((int32_t)-1);
      }
      if (newPassword.size()) {
        parcel->write(newPassword);
      } else {
        parcel->write((int32_t)-1);
      }
      return reqMgr->issueRequest(
          RIL_REQUEST_CHANGE_BARRING_PASSWORD, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("changeBarringPassword:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::queryClip(const QueryClipCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(RIL_REQUEST_QUERY_CLIP, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    int status = 0;
                                    if (p && p->dataAvail()) {
                                      p->read(status);
                                    }
                                    Log::getInstance().d("queryClip:resp: e=" + std::to_string(e));
                                    cb(e, status);
                                  });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::setSuppSvcNotification(const int& enabled,
                                             const SetSuppSvcNotificationCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(enabled);
      return reqMgr->issueRequest(
          RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("setSuppSvcNotification:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::registerUnsolCallStateChangeIndicationHandler(
    const CallStateChangeIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            indicationHandler();
        }
    );
}

Status RilApiSession::registerVoiceNetworkStateIndicationHandler(
    const VoiceNetworkStateIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            indicationHandler();
        }
    );
}

Status RilApiSession::registerVoiceRadioTechIndicationHandler(
    const VoiceRadioTechIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_VOICE_RADIO_TECH_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            int newRadioTech;
            if (p->read(newRadioTech) == Marshal::Result::SUCCESS) {
                indicationHandler(newRadioTech);
            }
        }
    );
}

Status RilApiSession::registerNitzTimeIndicationHandler(
    const NitzTimeIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_NITZ_TIME_RECEIVED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            char *nitzTime = nullptr;
            if (p->read(nitzTime) == Marshal::Result::SUCCESS) {
                indicationHandler(nitzTime);
                p->release(nitzTime);
            }
        }
    );
}

Status RilApiSession::registerRestrictedStateChangedIndicationHandler(
    const RestrictedStateChangedIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESTRICTED_STATE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr || p->dataAvail() == 0) {
                return;
            }
            int state;
            if (p->read(state) == Marshal::Result::SUCCESS) {
                indicationHandler(state);
            }
        }
    );
}

Status RilApiSession::registerRadioStateIndicationHandler(
    const RadioStateIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                return;
            }

            int radioState;
            if (p->read(radioState) == Marshal::Result::SUCCESS) {
                //RIL_RadioState state = static_cast<int>(radioState);
                indicationHandler(static_cast<RIL_RadioState>(radioState));
            }
        }
    );
}

Status RilApiSession::registerCallRingIndicationHandler(
    const CallRingIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_CALL_RING,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            RIL_CDMA_SignalInfoRecord sigInfoRecord;
            RIL_CDMA_SignalInfoRecord *pSigInfoRecord = nullptr;
            if (p && p->dataAvail()) {
              if (p->read(sigInfoRecord) == Marshal::Result::SUCCESS) {
                pSigInfoRecord = &sigInfoRecord;
              }
            }
            indicationHandler(pSigInfoRecord);
        }
    );
}

Status RilApiSession::registerOnUssdIndicationHandler(
    const OnUssdIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_ON_USSD,
                                   [indicationHandler](std::shared_ptr<Marshal> p) {
                                     char mode = 0;
                                     std::string message;
                                     if (p && p->dataAvail()) {
                                       p->read(mode);
                                       p->read(message);
                                     }
                                     indicationHandler(mode, message);
                                   });
}

Status RilApiSession::registerSuppSvcNotificationIndicationHandler(
    const SuppSvcNotificationIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_SUPP_SVC_NOTIFICATION,
                                   [indicationHandler](std::shared_ptr<Marshal> p) {
                                     RIL_SuppSvcNotification suppSvc = {};
                                     if (p && p->dataAvail()) {
                                       p->read(suppSvc);
                                       indicationHandler(suppSvc);
                                       p->release(suppSvc);
                                     }
                                   });
}

Status RilApiSession::registerCdmaCallWaitingIndicationHandler(
    const CdmaCallWaitingIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_CDMA_CALL_WAITING,
                                   [indicationHandler](std::shared_ptr<Marshal> p) {
                                     RIL_CDMA_CallWaiting_v6 callWaitingRecord = {};
                                     if (p && p->dataAvail()) {
                                       p->read(callWaitingRecord);
                                       indicationHandler(callWaitingRecord);
                                       p->release(callWaitingRecord);
                                     }
                                   });
}

Status RilApiSession::registerCdmaOtaProvisionStatusIndicationHandler(
    const CdmaOtaProvisionStatusIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_UNSOL_CDMA_OTA_PROVISION_STATUS, [indicationHandler](std::shared_ptr<Marshal> p) {
        int status = 0;
        if (p && p->dataAvail()) {
          p->read(status);
        }
        RIL_CDMA_OTA_ProvisionStatus provStatus = (RIL_CDMA_OTA_ProvisionStatus)status;
        indicationHandler(provStatus);
      });
}

Status RilApiSession::registerCdmaInfoRecIndicationHandler(
    const CdmaInfoRecIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_CDMA_INFO_REC,
                                   [indicationHandler](std::shared_ptr<Marshal> p) {
                                     RIL_CDMA_InformationRecords records = {};
                                     if (p && p->dataAvail()) {
                                       p->read(records);
                                     }
                                     indicationHandler(records);
                                   });
}
Status RilApiSession::registerSrvccStatusIndicationHandler(
    const SrvccStatusIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_SRVCC_STATE_NOTIFY,
                                   [indicationHandler](std::shared_ptr<Marshal> p) {
                                     RIL_SrvccState status = HANDOVER_STARTED;
                                     if (p && p->dataAvail()) {
                                       p->read(status);
                                     }
                                     indicationHandler(status);
                                   });
}

Status RilApiSession::registerOnSupplementaryServiceIndicationHandler(
    const OnSupplementaryServiceIndicationHandler& indicationHandler) {
  return registerIndicationHandler(RIL_UNSOL_ON_SS,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_StkCcUnsolSsResponse ss = {};
        if (p && p->dataAvail()) {
          p->read(ss);
          indicationHandler(ss);
          p->release(ss);
        }
      });
}

Status RilApiSession::setUnsolCellInfoListRate(const int32_t& mRate, const SetUnsolCellInfoListRateCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(mRate);
            return reqMgr->issueRequest(RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setupDataCall(const RIL_RadioAccessNetworks accessNetwork, const RIL_DataProfileInfo_IRadio_1_4& dataProfile,
                                    const bool roamingAllowed, RIL_RadioDataRequestReasons reason,const SetupDataCallCallback& cb) {
    RIL_SetUpDataCallParams callparams = {};
    callparams.accessNetwork = accessNetwork;
    callparams.profileInfo = dataProfile;
    callparams.roamingAllowed = roamingAllowed;
    callparams.reason = reason;
    callparams.addresses = (char*)"";
    callparams.dnses = (char*)"";
    return RilApiSession::setupDataCall(callparams, cb);
}

Status RilApiSession::setupDataCall(const RIL_SetUpDataCallParams &callParams, const SetupDataCallCallback& cb) {
    Log::getInstance().d("[RilApiSession]: setupDataCall enter.");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write(callParams) == Marshal::Result::FAILURE) {
                Log::getInstance().d("SetupDataCallRequestMessage Data Could not be written on socket. Marshal Write Failed");
                return Status::FAILURE;
            }
            return reqMgr->issueRequest(RIL_REQUEST_SETUP_DATA_CALL, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            if (p && p->dataAvail()) {
                Log::getInstance().d("[RilApiSession]: setupDataCall before Read");
                RIL_Data_Call_Response_v11 dataCallResponse = {};
                Status status = Status::SUCCESS;
                if (p->read(dataCallResponse) == Marshal::Result::FAILURE) {
                    Log::getInstance().d("SetupDataCall Response Could not be read from Socket. Marshal Write Failed");
                    status = Status::FAILURE;
                }
                Log::getInstance().d("[RilApiSession]: setupDataCall before callback");
                cb(e, status, &dataCallResponse);
                p->release(dataCallResponse);
            } else {
                cb(e, Status::FAILURE, nullptr);
            }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: setupDataCall exit");
    return Status::FAILURE;
}

Status RilApiSession::deactivateDataCall(const int32_t cid, const bool reason, const DeactivateDataCallCallback& cb) {
    Log::getInstance().d("[RilApiSession]: deactivateDataCall enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(cid);
            parcel->writeBool(reason);
            return reqMgr->issueRequest(RIL_REQUEST_DEACTIVATE_DATA_CALL, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    Log::getInstance().d("[RilApiSession]: deactivateDataCall exit");
    return Status::FAILURE;
}

Status RilApiSession::StartLceData(const int32_t interval, const int32_t mode, const StartLceDataCallback &cb) {
    Log::getInstance().d("[RilApiSession]: StartLceData enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(interval);
            parcel->write(mode);
            return reqMgr->issueRequest(RIL_REQUEST_START_LCE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: startLceData before Read");
                    RIL_LceStatusInfo LCECallResponse;
                    p->read(LCECallResponse);
                    Log::getInstance().d("[RilApiSession]: startLceData before callback");
                    cb(e, &LCECallResponse);
                } else {
                    cb(e, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: StartLceData exit");
    return Status::FAILURE;
}

Status RilApiSession::SetLinkCapFilter(const RIL_ReportFilter enable_bit, const SetLinkCapFilterCallback &cb) {
    Log::getInstance().d("[RilApiSession]: SetLinkCapFilter enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(static_cast<int>(enable_bit));
            return reqMgr->issueRequest(RIL_REQUEST_SET_LINK_CAP_FILTER, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: SetLinkCapFilter before Read");
                    int res;
                    res = p->readInt32();
                    Log::getInstance().d("[RilApiSession]: SetLinkCapFilter before callback");
                    cb(e, &res);
                } else {
                    cb(e, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: SetLinkCapFilter exit");
    return Status::FAILURE;
}

Status RilApiSession::SetLinkCapRptCriteria(const RIL_LinkCapCriteria &criteria, const SetLinkCapRptCriteriaCallback &cb) {
    Log::getInstance().d("[RilApiSession]: SetLinkCapRptCriteria enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(criteria);
            return reqMgr->issueRequest(RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: SetLinkCapRptCriteria before Read");
                    int res;
                    res = p->readInt32();
                    RIL_LinkCapCriteriaResult result = static_cast<RIL_LinkCapCriteriaResult>(res);
                    Log::getInstance().d("[RilApiSession]: SetLinkCapRptCriteria before callback");
                    cb(e, &result);
                } else {
                    cb(e, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: SetLinkCapRptCriteria exit");
    return Status::FAILURE;
}

Status RilApiSession::GetDataNrIconType(const GetDataNrIconTypeCallback &cb) {
    Log::getInstance().d("[RilApiSession]: GetDataNrIconType enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_GET_DATA_NR_ICON_TYPE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: GetDataNrIconType before Read");
                    int res;
                    Status status = Status::FAILURE;
                    Marshal::Result status1 = p->read(res);
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    five_g_icon_type result = static_cast<five_g_icon_type>(res);
                    Log::getInstance().d("[RilApiSession]: GetDataNrIconType before callback");
                    cb(e, status, &result);
                }
                else {
                    cb(e, Status::FAILURE, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: GetDataNrIconType exit");
    return Status::FAILURE;
}

Status RilApiSession::StartKeepAlive(const RIL_KeepaliveRequest request, const StartKeepAliveCallback &cb) {
    Log::getInstance().d("[RilApiSession]: StartKeepAlive enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(request);
            return reqMgr->issueRequest(RIL_REQUEST_START_KEEPALIVE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: StartKeepAlive before Read");
                    int res;
                    Marshal::Result status1 = p->read(res);
                    RIL_ResponseError result = static_cast<RIL_ResponseError>(res);
                    int res2;
                    Marshal::Result status2 = p->read(res2);
                    uint32_t handle = static_cast<uint32_t>(res2);
                    int res3;
                    Marshal::Result status3 = p->read(res3);
                    Status status = Status::FAILURE;
                    RIL_KeepaliveStatus_t statuscode = static_cast<RIL_KeepaliveStatus_t>(res3);
                    if (status1 == Marshal::Result::SUCCESS && status2 == Marshal::Result::SUCCESS
                        && status3 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    Log::getInstance().d("[RilApiSession]: StartKeepAlive before callback");
                    cb(e, status, &result, &handle, &statuscode);
                }
                else {
                    cb(e, Status::FAILURE, nullptr, nullptr, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: GetDataNrIconType exit");
    return Status::FAILURE;
}
Status RilApiSession::SetPreferredDataModem(const int32_t modemId, const SetPreferredDataModemCallback &cb) {
    Log::getInstance().d("[RilApiSession]: SetPreferredDataModem enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(modemId);
            return reqMgr->issueRequest(RIL_REQUEST_SET_PREFERRED_DATA_MODEM, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: SetPreferredDataModem before Read");
                    int res;
                    Marshal::Result status1 = p->read(res);
                    Status status = Status::FAILURE;
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    RIL_ResponseError result = static_cast<RIL_ResponseError>(res);
                    Log::getInstance().d("[RilApiSession]: SetPreferredDataModem before callback");
                    cb(e, status, &result);
                }
                else {
                    cb(e, Status::FAILURE, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: SetPreferredDataModem exit");
    return Status::FAILURE;
}

Status RilApiSession::registerLCEDataChangeHandler(const RILLCEDataChangeIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_LCEDATA_RECV,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            Log::getInstance().d("[RilApiSession]: LCEDataChangeInd received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: LCEDataChangeInd is NULL. Returning!!!");
                return;
            }
            RIL_LinkCapacityEstimate LCECallResponse;
            Log::getInstance().d("[RilApiSession]: LCEDataChangeInd before Read");
            p->read(LCECallResponse);
            Log::getInstance().d("[RilApiSession]: LCEDataChangeInd before indication handling");
            indicationHandler(LCECallResponse);
            p->release(LCECallResponse);
        }
    );
}

Status RilApiSession::registerRadioKeepAliveHandler(const RILDataRadioKeepAliveHandler &indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_KEEPALIVE_STATUS,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            Log::getInstance().d("[RilApiSession]: RadioKeepAliveInd received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: RadioKeepAliveInd is NULL. Returning!!!");
                return;
            }
            Log::getInstance().d("[RilApiSession]: RadioKeepAlive before Read");
            int handle = -1;
            RIL_KeepaliveStatusCode statuscode = RIL_KeepaliveStatusCode::KEEPALIVE_INACTIVE;
            Marshal::Result status1 = p->read(handle);
            int code;
            Marshal::Result status2 = p->read(code);
            Status status = Status::FAILURE;
            if (status1 == Marshal::Result::SUCCESS && status2 == Marshal::Result::SUCCESS) {
                status = Status::SUCCESS;
            }
            statuscode = static_cast<RIL_KeepaliveStatusCode>(code);
            Log::getInstance().d("[RilApiSession]: RadioKeepAlive before indication handling");
            indicationHandler(status, handle, statuscode);
        }
    );
}

Status RilApiSession::StopKeepAlive(const int32_t handle, const StopKeepAliveCallback &cb) {
    Log::getInstance().d("[RilApiSession]: StopKeepAlive enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(static_cast<int>(handle));
            return reqMgr->issueRequest(RIL_REQUEST_STOP_KEEPALIVE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: StopKeepAlive before Read");
                    int res;
                    Marshal::Result status1 = p->read(res);
                    Status status = Status::FAILURE;
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    RIL_ResponseError result = static_cast<RIL_ResponseError>(res);
                    Log::getInstance().d("[RilApiSession]: StopKeepAlive before callback");
                    cb(e, status, &result);
                }
                else {
                    cb(e, Status::FAILURE, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: StopKeepAlive exit");
    return Status::FAILURE;
}

Status RilApiSession::registerDataUnsolIndication
    (const int32_t filter, const RegisterDataUnsolIndicationCallback &cb) {
    Log::getInstance().d("[RilApiSession]: RegisterDataUnsolIndication enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(static_cast<int>(filter));
            return reqMgr->issueRequest(RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: RegisterDataUnsolIndication before Read");
                    int res = -1;
                    Marshal::Result status1 = p->read(res);
                    Status status = Status::FAILURE;
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    Log::getInstance().d("[RilApiSession]: RegisterDataUnsolIndication before callback");
                    cb(e, status, &res);
                } else {
                    cb(e, Status::FAILURE, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: RegisterDataUnsolIndication exit");
    return Status::FAILURE;
}

Status RilApiSession::registerDataNrIconChangeHandler(const RILDataNrIconChangeIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_DATA_NR_ICON_TYPE,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            Log::getInstance().d("[RilApiSession]: DataNrIconChangeIndication received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: DataNrIconChangeIndication is NULL. Returning!!!");
                return;
            }
            int val;
            five_g_icon_type data;
            Log::getInstance().d("[RilApiSession]: DataNrIconChangeIndication before Read");
            Marshal::Result status1 = p->read(val);
            Status status = Status::FAILURE;
            if (status1 == Marshal::Result::SUCCESS) {
                status = Status::SUCCESS;
            }
            data = static_cast<five_g_icon_type>(val);
            Log::getInstance().d("[RilApiSession]: DataNrIconChangeIndication before indication handling");
            indicationHandler(status, data);
        }
    );
}

Status RilApiSession::registerPhysicalConfigStructHandler(const PhysicalConfigStructHandlerHandler &indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            Log::getInstance().d("[RilApiSession]: PhysicalConfigStructUpdate received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: PhysicalConfigStructUpdate is NULL. Returning!!!");
                return;
            }
            RIL_PhysicalChannelConfig phyconfig = {};
            Log::getInstance().d("[RilApiSession]: PhysicalConfigStructUpdate before Read");
            int val = -1;
            std::vector<RIL_PhysicalChannelConfig> data;
            Marshal::Result status2 = p->read(val);
            Status status = Status::FAILURE;
            if (status2 == Marshal::Result::FAILURE) {
                indicationHandler(Status::FAILURE, data);
            }
            else {
                for (int i = 0; i < val; i++) {
                    phyconfig = {};
                    Marshal::Result status1 = p->read(phyconfig);
                    if (status1 == Marshal::Result::SUCCESS) {
                        status = Status::SUCCESS;
                    }
                    else {
                        indicationHandler(Status::FAILURE, data);
                        return;
                    }
                    data.push_back(phyconfig);
                    p->release(phyconfig);
                }
                Log::getInstance().d("[RilApiSession]: PhysicalConfigStructUpdate before indication handling");
                indicationHandler(status, data);
            }
        }
    );
}

Status RilApiSession::registerPcoDataChangeHandler(const RILPCODataChangeIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_PCO_DATA,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            Log::getInstance().d("[RilApiSession]: PCODataChangeInd received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: PCODataChangeInd is NULL. Returning!!!");
                return;
            }
            RIL_PCO_Data PCOCallResponse;
            Log::getInstance().d("[RilApiSession]: PCODataChangeInd before Read");
            p->read(PCOCallResponse);
            Log::getInstance().d("[RilApiSession]: PCODataChangeInd before indication handling");
            indicationHandler(PCOCallResponse);
            p->release(PCOCallResponse);
        }
    );
}

Status RilApiSession::StopLceData(const StopLceDataCallback &cb) {
    Log::getInstance().d("[RilApiSession]: StopLceData enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_STOP_LCE, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: stopLceData before Read");
                    RIL_LceStatusInfo LCECallResponse;
                    p->read(LCECallResponse);
                    Log::getInstance().d("[RilApiSession]: stopLceData before callback");
                    cb(e, &LCECallResponse);
                } else {
                    cb(e, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: StopLceData exit");
    return Status::FAILURE;
}

Status RilApiSession::PullLceData(const PullLceDataCallback &cb) {
    Log::getInstance().d("[RilApiSession]: PullLceData enter");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_PULL_LCEDATA, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    Log::getInstance().d("[RilApiSession]: pullLceData before Read");
                    RIL_LceDataInfo LCECallResponse;
                    p->read(LCECallResponse);
                    Log::getInstance().d("[RilApiSession]: pullLceData before callback");
                    cb(e, &LCECallResponse);
                } else {
                    cb(e, nullptr);
                }
            });
        }
    }
    Log::getInstance().d("[RilApiSession]: PullLceData exit");
    return Status::FAILURE;
}

Status RilApiSession::registerDataCallListChangedIndicationHandler(const DataCallListChangedIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_DATA_CALL_LIST_CHANGED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
          Log::getInstance().d("[RilApiSession]: RadioDataCallListChangeInd received");
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: RadioDataCallListChangeInd p is NULL. Returning!!!");
                return;
            }
            uint64_t _dataCallListLen = 0;
            if (p->read(_dataCallListLen) != Marshal::Result::SUCCESS) {
                Log::getInstance().d("[RilApiSession]: RadioDataCallListChangeInd read length is zero. Returning!!!");
                return;
            }
            size_t dataCallListLen = static_cast<size_t>(_dataCallListLen);
            RIL_Data_Call_Response_v11* dataCallList = new RIL_Data_Call_Response_v11[dataCallListLen];
            /* This memory is passed to client. Dynamic fields on this memory needs to be cleared by client*/
            if (dataCallList == nullptr) {
                Log::getInstance().d("[RilApiSession]: Device has no Memory. Returning!!!");
                return;
            }
            for (int i = 0; (i < dataCallListLen) && p->dataAvail(); i++) {
              Log::getInstance().d("[RilApiSession]: RadioDataCallListChangeInd Data present!!!");
              p->read(dataCallList[i]);
            }
            indicationHandler(dataCallList, dataCallListLen);
            Log::getInstance().d("[RilApiSession]: RadioDataCallListChangeInd done!!");
            for (int i = 0; i < dataCallListLen; i++) {
                p->release(dataCallList[i]);
            }
            if (dataCallList) {
                if( dataCallListLen > 0) {
                  delete[] dataCallList;
                }
                dataCallList = NULL;
            }
        }
    );
}

Status RilApiSession::registerCdmaNewSmsIndicationHandler(const CdmaNewSmsIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_CDMA_NEW_SMS,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p == nullptr || p->dataAvail() == 0) {
                return;
            }

            RIL_CDMA_SMS_Message sms;
            if (p->read(sms) == Marshal::Result::SUCCESS) {
                indicationHandler(sms);
            }
        }
    );
}

Status RilApiSession::setInitialAttachApn(const RIL_DataProfileInfo_IRadio_1_4& dataProfile, const SetInitialAttachApnCallback &cb) {
    Log::getInstance().d("[RilApiSession]: setInitialAttachApn enter.");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(dataProfile);
            return reqMgr->issueRequest(RIL_REQUEST_SET_INITIAL_ATTACH_APN, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setDataProfile(const RIL_DataProfileInfo_IRadio_1_4* dataProfile,const uint32_t size, const SetDataProfileCallback &cb) {
    Log::getInstance().d("[RilApiSession]: setDataProfile enter.");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if ((parcel != nullptr) && (dataProfile != nullptr)) {
            parcel->write(size);
            for (int i =0; i< size; i++) {
                parcel->write(dataProfile[i]);
            }
            return reqMgr->issueRequest(RIL_REQUEST_SET_DATA_PROFILE, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
        else {
           Log::getInstance().d("[RilApiSession]: setDataProfile parcel or dataProfile is NULL"
                                "Failed to send SetDataProfile request");
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::sendSms(const std::string& message, const std::string& smscAddress,
        bool expectMore, const SendSmsCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    RIL_GsmSmsMessage gsmSms {};

    if (!smscAddress.empty()) {
        gsmSms.smscPdu = const_cast<char*>(smscAddress.c_str());
    }

    gsmSms.pdu = const_cast<char*>(message.c_str());

    if (parcel->write(gsmSms) != Marshal::Result::SUCCESS
            || parcel->write(expectMore) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_SEND_SMS,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_SMS_Response smsResponse {};

            if (!p || p->read(smsResponse) != Marshal::Result::SUCCESS) {
                if (e == RIL_Errno::RIL_E_SUCCESS) {
                    e = RIL_Errno::RIL_E_INTERNAL_ERR;
                }
                smsResponse.messageRef = -1;
                smsResponse.ackPDU = nullptr;
                smsResponse.errorCode = -1;
            }

            cb(e, smsResponse);

            if (p) {
                p->release(smsResponse);
            }
        }
    );
}

Status RilApiSession::ackSms(const RIL_GsmSmsAck ack, const AckSmsCallback& cb) {
    if (!reqMgr) return Status::FAILURE;
    auto parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(ack) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }
    return reqMgr->issueRequest(RIL_REQUEST_SMS_ACKNOWLEDGE, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
}

Status RilApiSession::getSmscAddress(const GetSmscAddressCallback& cb) {
    if (!reqMgr) return Status::FAILURE;
    auto parcel = std::make_unique<Marshal>();
    if (!parcel) return Status::FAILURE;
    return reqMgr->issueRequest(RIL_REQUEST_GET_SMSC_ADDRESS, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            std::string addr;
                            if (!p || !p->dataAvail() ||
                                    p->read(addr) != Marshal::Result::SUCCESS) {
                                e = RIL_E_INTERNAL_ERR;
                            }
                            cb(e, addr.c_str());
                        });
}

Status RilApiSession::setSmscAddress(
        const std::string& addr, const SetSmscAddressCallback& cb) {
    if (!reqMgr) return Status::FAILURE;
    auto parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(addr) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }
    return reqMgr->issueRequest(RIL_REQUEST_SET_SMSC_ADDRESS, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
}

Status RilApiSession::setTtyMode(const int mode, const SetTtyModeCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      parcel->write(mode);
      return reqMgr->issueRequest(RIL_REQUEST_SET_TTY_MODE, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::getTtyMode(const GetTtyModeCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      return reqMgr->issueRequest(RIL_REQUEST_QUERY_TTY_MODE, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    if (p && p->dataAvail()) {
                                      int mode;
                                      p->read(mode);
                                      cb(e, mode);
                                    }
                                  });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::setCdmaVoicePrefMode(const int mode, const SetCdmaVoicePrefModeCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      parcel->write(mode);
      return reqMgr->issueRequest(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
                                  std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::getCdmaVoicePrefMode(const GetCdmaVoicePrefModeCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      return reqMgr->issueRequest(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
                                  std::move(parcel), [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    if (p && p->dataAvail()) {
                                      int mode;
                                      p->read(mode);
                                      cb(e, mode);
                                    }
                                  });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::sendCdmaFlash(const std::string& flash, const SendCdmaFlashCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      parcel->write(flash);
      return reqMgr->issueRequest(RIL_REQUEST_CDMA_FLASH, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::sendCdmaBurstDtmf(const std::string& dtmf, int32_t on, int32_t off,
                                        const SendCdmaBurstDtmfCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      parcel->write(dtmf);
      parcel->write(on);
      parcel->write(off);
      return reqMgr->issueRequest(RIL_REQUEST_CDMA_BURST_DTMF, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::getImsRegState(const GetImsRegStateCallBack& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel) {
      return reqMgr->issueRequest(RIL_REQUEST_IMS_REGISTRATION_STATE, std::move(parcel),
                                  [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    if (p && p->dataAvail()) {
                                      int registered, tech;
                                      RIL_RadioTechnologyFamily family{ RADIO_TECH_FAMILY_UNKNOWN };
                                      p->read(registered);
                                      p->read(tech);
                                      if (registered) {
                                        family = (tech == 1) ? RADIO_TECH_3GPP : RADIO_TECH_3GPP2;
                                      }
                                      cb(e, !!registered, family);
                                    }
                                  });
    }
  }

  return Status::FAILURE;
}

Status RilApiSession::getDataCallList(const GetDataCallListCallback &cb) {
    Log::getInstance().d("[RilApiSession]: getDataCallList enter.");
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_DATA_CALL_LIST, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            if (p == nullptr) {
                Log::getInstance().d("[RilApiSession]: getDataCallList p is NULL. Returning!!!");
                return;
            }
            size_t dataCallListLen = 0;
            if (p->read(dataCallListLen) != Marshal::Result::SUCCESS) {
                Log::getInstance().d("[RilApiSession]: getDataCallList read length is zero. Returning!!!");
                return;
            }
            RIL_Data_Call_Response_v11* dataCallList = nullptr;
            if(dataCallListLen) {
                dataCallList = new RIL_Data_Call_Response_v11[dataCallListLen];
                /* This memory is passed to client. Dynamic fields on this memory needs to be cleared by client*/
                if(dataCallList) {
                    for (int i = 0; (i < dataCallListLen) && p->dataAvail(); i++) {
                        Log::getInstance().d("[RilApiSession]: getDataCallList Data present!!!");
                        p->read(dataCallList[i]);
                    }
                }
            }
            cb(e, dataCallList, dataCallListLen);
            Log::getInstance().d("[RilApiSession]: getDataCallList done!!");
            for (int i = 0; i < dataCallListLen; i++) {
                p->release(dataCallList[i]);
            }
            if (dataCallList) {
                delete[] dataCallList;
                dataCallList = NULL;
            }
       });
    }
    return Status::FAILURE;
}
Status RilApiSession::registerNewSmsOnSimIndicationHandler(
    const NewSmsOnSimIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            int index = -1;
            if(p && p->dataAvail()) {
                p->read(index);
            }
            indicationHandler(index);
        }
    );
}
Status RilApiSession::registerSimSmsStorageFullIndicationHandler(
    const SimSmsStorageFullIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_UNSOL_SIM_SMS_STORAGE_FULL,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            indicationHandler();
        }
    );
}
Status RilApiSession::registerNewSmsStatusReportIndicationHandler(
    const NewSmsStatusReportIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            char* payload = nullptr;
            if(p && p->dataAvail()) {
                p->read(payload);
            }
            indicationHandler(payload);
            delete payload;
        }
    );
}
Status RilApiSession::registerCdmaSmsRuimStorageFullIndicationHandler(
    const CdmaSmsRuimStorageFullIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
    RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL,
        [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
            indicationHandler();
        }
    );
}

Status RilApiSession::queryAvailableNetworks(const QueryAvailableNetworksCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        return reqMgr->issueRequest(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    std::vector<RIL_OperatorInfo> networks{};
                    if (p && p->dataAvail()) {
                        int count;
                        p->read(count);
                        while(count-- && p->dataAvail()) {
                            RIL_OperatorInfo nw {};
                            p->read(nw);
                            networks.emplace_back(nw);
                        }
                    } else {
                        e = RIL_E_NO_MEMORY;
                    }
                    cb(e, networks);
                    // Free RIL_OperatorInfo's
                    if (p) {
                      for (auto &nw : networks) {
                        p->release(nw);
                      }
                    }
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::startNetworkScan(const RIL_NetworkScanRequest& req,
    const VoidResponseHandler& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel) {
            parcel->write(req);
        }
        return reqMgr->issueRequest(RIL_REQUEST_START_NETWORK_SCAN , std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                    cb(e);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::stopNetworkScan(const VoidResponseHandler& cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_REQUEST_STOP_NETWORK_SCAN , nullptr,
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                    cb(e);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::setSystemSelectionChannels(const RIL_SysSelChannels& req,
    const VoidResponseHandler& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel) {
            parcel->write(req);
        }
        return reqMgr->issueRequest(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                    cb(e);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::registerNetworksScanIndicationHandler(
    const NetworkScanIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_NETWORK_SCAN_RESULT,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    RIL_NetworkScanResult  arg {};
                    if (p->read(arg) == Marshal::Result::SUCCESS) {
                        indicationHandler(arg);
                        p->release(arg);
                    }
                }
            }
    );
}

Status RilApiSession::registerEmergencyListIndicationHandler(
        const EmergencyListIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_EMERGENCY_NUMBERS_LIST,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    RIL_EmergencyList arg {};
                    if (p->read(arg) == Marshal::Result::SUCCESS) {
                        indicationHandler(arg);
                    }
                }
            }
    );
}

/*****************
## UIM Messages ##
*****************/

Status RilApiSession::simGetImsiReq(const char *aid, const GetImsiReqCallback& cb) {
    if (reqMgr) {
        if (aid != nullptr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if(parcel != nullptr) {
                const char *params[1] = {aid};
                parcel->write(params);
                return reqMgr->issueRequest(RIL_REQUEST_GET_IMSI, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            const char *IMSI = nullptr;
                            std::string tmp;
                            if (p) {
                                if (p->read(tmp) == Marshal::Result::SUCCESS) {
                                    IMSI = tmp.c_str();
                                }
                            }
                            cb(e, IMSI);
                        });
            }
        }
        else {
           Log::getInstance().d("[RilApiSession::simGetImsiReq]: Invalid Input Arguments.");
           RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
           cb(errorCode,nullptr);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simIOReq(const RIL_SIM_IO_v6& reqParams, const IOReqCallback& cb) {
    int valid_arg = 0;
    if (reqMgr) {
            /* Check for update binary or update record command
             * aid and data ptr is mandatory must be non null
             */
            if((reqParams.command == RIL_UIM_CMD_UPDATE_BINARY  ||
                    reqParams.command == RIL_UIM_CMD_UPDATE_RECORD) &&
                    (reqParams.path && reqParams.data && reqParams.aidPtr)) {
                valid_arg = 1;
            }
            /* check for read binary or read record or get response
             *  aidPtr is mandatory and it must be non null
             */
            else if((reqParams.command == RIL_UIM_CMD_READ_BINARY ||
                reqParams.command == RIL_UIM_CMD_READ_RECORD ||
                   reqParams.command == RIL_UIM_CMD_GET_RESPONSE) &&
                      (reqParams.path && reqParams.aidPtr)) {
                valid_arg = 1;
            }
            /* check for sim_cmd_status command
             *  file_path mandatory and it must ne non null
            */
            else if((reqParams.command == RIL_UIM_CMD_STATUS) && (reqParams.path)) {
                valid_arg = 1;
            }
            if(valid_arg) {
                std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
                if (parcel != nullptr) {
                    parcel->write(reqParams);
                    return reqMgr->issueRequest(RIL_REQUEST_SIM_IO, std::move(parcel),
                            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                RIL_SIM_IO_Response *resp = nullptr;
                                RIL_SIM_IO_Response tmp{};
                                if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                    resp = &tmp;
                                }
                                cb(e, resp);
                                if(tmp.simResponse != nullptr){
                                p->release(tmp);
                                }
                            });
                }
            }
            else {
                Log::getInstance().d("[RilApiSessioni::simIOReq]: Invalid Input Arguments.");
                RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
                cb(errorCode,nullptr);
            }
    }
    return Status::FAILURE;
}

Status RilApiSession::simGetSimStatusReq(const GetSimStatusReqCallback& cb) {
    if (reqMgr) {
       return reqMgr->issueRequest(RIL_REQUEST_GET_SIM_STATUS, nullptr,
               [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                   RIL_CardStatus_v6 *resp = nullptr;
                   RIL_CardStatus_v6 tmp{};
                   if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                       resp = &tmp;
                   }
                   cb(e, resp);
                   p->release(tmp);
               });
    }
    return Status::FAILURE;
}

Status RilApiSession::simOpenChannelReq(const RIL_OpenChannelParams& reqParams, const OpenChannelReqCallback& cb) {
    if (reqMgr) {
            if(reqParams.aidPtr != nullptr) {
                std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
                if (parcel != nullptr) {
                    parcel->write(reqParams);
                    return reqMgr->issueRequest(RIL_REQUEST_SIM_OPEN_CHANNEL, std::move(parcel),
                            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                const int *resp = nullptr;
                                std::vector<int> tmp{};
                                size_t len = 0;
                                if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                    resp = tmp.data();
                                    len = tmp.size();
                                }
                                cb(e, resp, len);
                            });
                    }
            }
            else {
                Log::getInstance().d("[RilApiSessioni::simOpenChannelReq]: Invalid Input Arguments.");
                RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
                cb(errorCode,nullptr,0);
            }
    }
    return Status::FAILURE;
}

Status RilApiSession::simCloseChannelReq(const int sessionId, const CloseChannelReqCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            int reqParams[] = { sessionId };
            parcel->write(reqParams);
            return reqMgr->issueRequest(RIL_REQUEST_SIM_CLOSE_CHANNEL, std::move(parcel),
                                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) { cb(e); });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simTransmitApduBasicReq(const RIL_SIM_APDU& reqParams, const TransmitApduBasicReqCallback& cb) {
    if (reqMgr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write(reqParams);
                return reqMgr->issueRequest(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            RIL_SIM_IO_Response *resp = nullptr;
                            RIL_SIM_IO_Response tmp{};
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                            if(tmp.simResponse !=nullptr) {
                                p->release(tmp);
                            }
                        });
            }
        }
    return Status::FAILURE;
}

Status RilApiSession::simTransmitApduChannelReq(const RIL_SIM_APDU& reqParams, const TransmitApduChannelReqCallback& cb) {
    if (reqMgr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write(reqParams);
                return reqMgr->issueRequest(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            RIL_SIM_IO_Response *resp = nullptr;
                            RIL_SIM_IO_Response tmp{};
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                            if(tmp.simResponse != nullptr) {
                                p->release(tmp);
                            }
                        });
            }
        }
    return Status::FAILURE;
}

Status RilApiSession::simChangePinReq(const char ** reqParams, const ChangePinReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1] && reqParams[2]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 3);
                return reqMgr->issueRequest(RIL_REQUEST_CHANGE_SIM_PIN, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *resp = nullptr;
                            int tmp = 0;
                            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                                errorCode = e;
                            }
                            cb(errorCode, resp);
                        });
            }
        }
        else {
            Log::getInstance().d("[RilApiSession::simChangePinReq]: Invalid Input Arguments.");
            RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
            cb(errorCode,nullptr);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simChangePin2Req(const char ** reqParams, const ChangePin2ReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1] && reqParams[2]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 3);
                return reqMgr->issueRequest(RIL_REQUEST_CHANGE_SIM_PIN2, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *resp = nullptr;
                            int tmp = 0;
                            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                                errorCode = e;
                            }
                            cb(errorCode, resp);
                        });
            }
       }
       else {
           Log::getInstance().d("[RilApiSessioni::simChangePin2Req]: Invalid Input Arguments.");
           RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
           cb(errorCode,nullptr);
       }
    }
    return Status::FAILURE;
}

Status RilApiSession::simEnterPukReq(const char ** reqParams, const EnterPukReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1] && reqParams[2]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 3);
                return reqMgr->issueRequest(RIL_REQUEST_ENTER_SIM_PUK, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *resp = nullptr;
                            int tmp;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                        });
            }
        }
        else {
            Log::getInstance().d("[RilApiSession::simEnterPukReq]: Invalid Input Arguments.");
            RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
            cb(errorCode,nullptr);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simEnterPuk2Req(const char ** reqParams, const EnterPuk2ReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1] && reqParams[2]) {
           std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
           if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 3);
                return reqMgr->issueRequest(RIL_REQUEST_ENTER_SIM_PUK2, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *resp = nullptr;
                            int tmp;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                        });
            }
        }
        else {
           Log::getInstance().d("[RilApiSession::simEnterPuk2Req]: Invalid Input Arguments.");
           RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
           cb(errorCode,nullptr);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simEnterPinReq(const char ** reqParams, const EnterPinReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 2);
                return reqMgr->issueRequest(RIL_REQUEST_ENTER_SIM_PIN, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *retries = nullptr;
                            int tmp = 0;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                retries = &tmp;
                            }
                            cb(e, retries);
                        });
            }
         }
         else {
             Log::getInstance().d("[RilApiSession::simEnterPinReq]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}


Status RilApiSession::simEnterPin2Req(const char ** reqParams, const EnterPin2ReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[1]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 2);
                return reqMgr->issueRequest(RIL_REQUEST_ENTER_SIM_PIN2, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *resp = nullptr;
                            int tmp = 0;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                        });
            }
         }
         else {
             Log::getInstance().d("[RilApiSession::simEnterPin2Req]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}

Status RilApiSession::simQueryFacilityLockReq(const char ** reqParams, const QueryFacilityLockReqCallback& cb) {
    if (reqMgr) {
        if(reqParams[0] && reqParams[3]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write<const char *>(reqParams, 4);
                return reqMgr->issueRequest(RIL_REQUEST_QUERY_FACILITY_LOCK, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int32_t *rsp = nullptr;
                            int32_t tmp = 0;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                rsp = &tmp;
                            }
                            cb(e, rsp);
                        });
             }
         }
         else {
             Log::getInstance().d("[RilApiSession::simQueryFacilityLockReq]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}

Status RilApiSession::simSetFacilityLockReq(const char ** reqParams, const SetFacilityLockReqCallback& cb) {
    int valid_arg = 0;
    if (reqMgr) {
        /* check for null check for set pin status*/
        if((strcmp(reqParams[0],"SC")==0) && reqParams[2] && reqParams[4]) {
            valid_arg =1;
        }
        /*Null check for set fdn status*/
        else if((strcmp(reqParams[0],"SC")!=0) && reqParams[4]) {
            valid_arg = 1;
        }
        if(valid_arg) {
            if(reqParams[0] && reqParams[4]) {
                std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
                if (parcel != nullptr) {
                    parcel->write<const char *>(reqParams, 5);
                    return reqMgr->issueRequest(RIL_REQUEST_SET_FACILITY_LOCK, std::move(parcel),
                            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                int *resp = nullptr;
                                int tmp = 0;
                                if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                    resp = &tmp;
                                }
                                cb(e, resp);
                            });
                }
             }
         }
         else {
             Log::getInstance().d("[RilApiSession::simSetFacilityLockReq]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}

Status RilApiSession::simIsimAuthenticationReq(const char * reqParams, const IsimAuthenticationReqCallback& cb) {
    if (reqMgr) {
        if(reqParams != nullptr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write(reqParams);
                return reqMgr->issueRequest(RIL_REQUEST_ISIM_AUTHENTICATION, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            const char *response = nullptr;
                            std::string tmp;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                response = tmp.c_str();
                            }
                            cb(e, response);
                        });
            }
         }
         else {
             Log::getInstance().d("[RilApiSessioni::simIsimAuthenticationReq]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}

Status RilApiSession::simStkSendEnvelopeWithStatusReq(const char * reqParams, const StkSendEnvelopeWithStatusReqCallback& cb) {
    if (reqMgr) {
        if(reqParams != nullptr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write(reqParams);
                return reqMgr->issueRequest(RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            RIL_SIM_IO_Response *resp = nullptr;
                            RIL_SIM_IO_Response tmp{};
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                            if(tmp.simResponse != nullptr) {
                               p->release(tmp);
                            }
                        });
            }
        }
        else {
            Log::getInstance().d("[RilApiSession::simStkSendEnvelopeWithStatusReq]: Invalid Input Arguments.");
            RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
            cb(errorCode,nullptr);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::simAuthenticationReq(const RIL_SimAuthentication &reqParams, const AuthenticationReqCallback& cb) {
    if (reqMgr) {
        if(reqParams.authData && reqParams.aid) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                parcel->write(reqParams);
                return reqMgr->issueRequest(RIL_REQUEST_SIM_AUTHENTICATION, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            RIL_SIM_IO_Response *resp = nullptr;
                            RIL_SIM_IO_Response tmp;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                resp = &tmp;
                            }
                            cb(e, resp);
                            if(tmp.simResponse != nullptr) {
                               p->release(tmp);
                            }
                        });
            }
        }
        else {
             Log::getInstance().d("[RilApiSession::simAuthenticationReq]: Invalid Input Arguments.");
             RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
             cb(errorCode,nullptr);
         }
    }
    return Status::FAILURE;
}

Status RilApiSession::sendCdmaSms(const RIL_CDMA_SMS_Message& smsParams, const SendSmsCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(smsParams);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_SEND_SMS, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    RIL_SMS_Response result {-1, nullptr, -1};
                                    if (p && p->dataAvail()) {
                                        p->read<RIL_SMS_Response>(result);
                                    }
                                    cb(e, result);
                                    if (p) {
                                        p->release(result);
                                    }
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::ackCdmaSms(const RIL_CDMA_SMS_Ack& ackParams, const AckCdmaSmsCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(ackParams);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p */) {
                                    cb(e);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::getCdmaSmsBroadcastConfig(const CdmaGetBroadcastSmsConfigCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    RIL_CDMA_BroadcastSmsConfigInfo*  configInfo = nullptr;
                                    size_t num = 0;
                                    if (p && p->dataAvail()) {
                                        if (p->readAndAlloc<RIL_CDMA_BroadcastSmsConfigInfo>(
                                                configInfo, num) != Marshal::Result::SUCCESS) {
                                            e = RIL_E_INTERNAL_ERR;
                                        }
                                    }
                                    cb(e, configInfo, num);
                                    if (configInfo != nullptr) {
                                        delete []configInfo;
                                    }
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setCdmaSmsBroadcastConfig(const RIL_CDMA_BroadcastSmsConfigInfo cfgParams[],
        size_t size, const CdmaSetBroadcastSmsConfigCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write<RIL_CDMA_BroadcastSmsConfigInfo>(cfgParams, size);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /* p */) {
                                    cb(e);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::setCdmaSmsBroadcastActivation(const int32_t value,
        const CdmaSmsBroadcastActivationCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(value);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /* p */) {
                                    cb(e);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::writeCdmaSmsToRuim(const RIL_CDMA_SMS_WriteArgs arg,
        const WriteCdmaSmsToSimCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(arg);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    int32_t recordNumber = -1;
                                    if (p && p->dataAvail()) {
                                        p->read(recordNumber);
                                    }
                                    cb(e, recordNumber);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::deleteCdmaSmsOnRuim(int32_t index, const DeleteCdmaSmsOnRuimCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(index);
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /* p */) {
                                    cb(e);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::EnterNetworkDepersonalization(const char **reqParams, const EnterPinReqCallback &cb) {
    if (reqMgr) {
        if(reqParams && reqParams[0]) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr && reqParams) {
                parcel->write<const char *>(reqParams, 1);
                return reqMgr->issueRequest(RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            int *retries = nullptr;
                            int tmp = 0;
                            if (p && p->read(tmp) == Marshal::Result::SUCCESS) {
                                retries = &tmp;
                            }
                            cb(e, retries);
                        });
            }
       }
       else {
           RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
           cb(errorCode,nullptr);
       }
    }
    return Status::FAILURE;
}

Status RilApiSession::GsmGetBroadcastSmsConfig(const GsmGetBroadcastSmsConfigCallback& cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG, nullptr,
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    std::vector<RIL_GSM_BroadcastSmsConfigInfo> tmp;
                    const RIL_GSM_BroadcastSmsConfigInfo *response = nullptr;
                    size_t respLen = 0;
                    if (p) {
                        if (p->read(tmp) == Marshal::Result::SUCCESS) {
                            response = tmp.data();
                            respLen = tmp.size();
                        }
                    }
                    cb(e, response, respLen);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::GsmSetBroadcastSMSConfig(RIL_GSM_BroadcastSmsConfigInfo *config, size_t len, const VoidResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write(config, len) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::GsmSmsBroadcastActivation(uint32_t disable, const VoidResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write(disable) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::ReportStkServiceIsRunning(const VoidResponseHandler &cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, nullptr,
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                    cb(e);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::GetAtr(uint8_t slot, const CharStarSizeResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write(slot) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_SIM_GET_ATR, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                            if (p) {
                                std::string atr{};
                                if (p->read(atr) == Marshal::Result::SUCCESS) {
                                    cb(e,atr.c_str(), atr.size());
                                }
                            }
                        });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::StkHandleCallSetupRequestedFromSim(int32_t accept, const VoidResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            if (parcel->write(accept) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::StkSendEnvelopeCommand(const char *command, size_t size, const CharStarSizeResponseHandler &cb) {
    if (reqMgr) {
        if(command != nullptr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                if (parcel->write(command, size) == Marshal::Result::SUCCESS) {
                    return reqMgr->issueRequest(RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND, std::move(parcel),
                            [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                if (p) {
                                    std::string resp{};
                                    if (p->read(resp) == Marshal::Result::SUCCESS) {
                                        cb(e, resp.c_str(), resp.size());
                                    }
                                }
                            });
                }
            }
          }
          else {
               Log::getInstance().d("[RilApiSession::StkSendEnvelopeCommand]: Invalid Input Arguments.");
               RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
               cb(errorCode,nullptr,0);
          }
    }
    return Status::FAILURE;
}

Status RilApiSession::StkSendTerminalResponse(const char *terminalResponse, size_t size, const VoidResponseHandler &cb) {
    if (reqMgr) {
        if(terminalResponse != nullptr) {
            std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
            if (parcel != nullptr) {
                if (parcel->write(terminalResponse, size) == Marshal::Result::SUCCESS) {
                    return reqMgr->issueRequest(RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE, std::move(parcel),
                            [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                                cb(e);
                            });
                }
            }
        }
        else {
            Log::getInstance().d("[RilApiSession::StkSendTerminalResponse]: Invalid Input Arguments.");
            RIL_Errno errorCode = RIL_E_INVALID_ARGUMENTS;
            cb(errorCode);
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::GetAdnRecord(const GetAdnRecordCallback &cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_REQUEST_GET_ADN_RECORD, nullptr,
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    if (p) {
                        RIL_AdnCountInfo tmp{};
                        RIL_AdnCountInfo *resp = nullptr;
                        if (p->read(tmp) == Marshal::Result::SUCCESS) {
                            resp = &tmp;
                        }
                        cb(e, resp);
                    }
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::UpdateAdnRecord(const RIL_AdnRecordInfo *record, const VoidResponseHandler &cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr && record) {
            if (parcel->write(*record) == Marshal::Result::SUCCESS) {
                return reqMgr->issueRequest(RIL_REQUEST_UPDATE_ADN_RECORD, std::move(parcel),
                        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                            cb(e);
                        });
            }
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::registerStkUnsolSessionEnd(
        const VoidIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_STK_SESSION_END,
            [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
                indicationHandler();
            }
    );
}

Status RilApiSession::registerStkUnsolProactiveCommand(
        const CharStarIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_STK_PROACTIVE_COMMAND,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    char *arg = nullptr;
                    if (p->read(arg) == Marshal::Result::SUCCESS) {
                        indicationHandler(arg);
                        delete[] arg;
                        arg = nullptr;
                    }
                }
            }
    );
}

Status RilApiSession::registerStkEventNotify(
        const CharStarIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_STK_EVENT_NOTIFY,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    char *arg = nullptr;
                    if (p->read(arg) == Marshal::Result::SUCCESS) {
                        indicationHandler(arg);
                        delete[] arg;
                        arg = nullptr;
                    }
                }
            }
    );
}

Status RilApiSession::registerStkCallSetup(
        const IntIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_STK_CALL_SETUP,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    int arg = 0;
                    if (p->read(arg) == Marshal::Result::SUCCESS) {
                        indicationHandler(arg);
                    }
                }
            }
    );
}

Status RilApiSession::imsGetRegistrationState(const ImsGetRegistrationStateCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_GET_REGISTRATION_STATE, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_Registration regInfo{};
          if (p && p->dataAvail()) {
            p->read(regInfo);
          }
          Log::getInstance().d("imsGetRegistrationState:resp: e=" + std::to_string(e));
          cb(e, regInfo);
          if (p) {
            p->release(regInfo);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsGetImsSubConfig(const ImsGetImsSubConfigCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_GET_IMS_SUB_CONFIG, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SubConfigInfo subConfigInfo{};
          if (p && p->dataAvail()) {
            p->read(subConfigInfo);
          }
          Log::getInstance().d("imsGetImsSubConfig:resp: e=" + std::to_string(e));
          cb(e, subConfigInfo);
          // Free
          if (p) {
            p->release(subConfigInfo);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsRegistrationChange(const RIL_IMS_RegistrationState& state,
                                            const ImsRegistrationChangeCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    parcel->write(state);
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_REGISTRATION_CHANGE, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsRegistrationChange:resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsQueryServiceStatus(const ImsQueryServiceStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_QUERY_SERVICE_STATUS, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          std::vector<RIL_IMS_ServiceStatusInfo> ssInfoVector;
          int32_t numSsInfos = 0;
          if (p && p->dataAvail()) {
            p->read(numSsInfos);
            for (int i = 0; (i < numSsInfos) && p->dataAvail(); i++) {
              RIL_IMS_ServiceStatusInfo ssInfo{};
              p->read(ssInfo);
              ssInfoVector.push_back(ssInfo);
            }
          }
          RIL_IMS_ServiceStatusInfo **ssInfos = nullptr;
          if (numSsInfos) {
            ssInfos = new RIL_IMS_ServiceStatusInfo*[numSsInfos]();
          }
          if(ssInfos) {
            for (int i = 0; (i < numSsInfos); i++) {
              ssInfos[i] = &ssInfoVector[i];
            }
          }
          Log::getInstance().d("imsQueryServiceStatus:resp: e=" + std::to_string(e));
          cb(e, numSsInfos, (const RIL_IMS_ServiceStatusInfo **)ssInfos);
          // Free
          if (p) {
            for (auto &e : ssInfoVector) {
              p->release(e);
            }
          }
          delete[] ssInfos;
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetServiceStatus(const size_t numSsInfos, const RIL_IMS_ServiceStatusInfo **ssInfos,
                                          const ImsSetServiceStatusCallback& cb) {
  if (reqMgr) {
    if (!numSsInfos || !ssInfos) {
      Log::getInstance().d("imsSetServiceStatus: invalid parameters");
      return Status::FAILURE;
    }
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    int32_t count = numSsInfos;
    parcel->write(count);
    for (size_t i = 0; i < numSsInfos; i++) {
      const auto ss = ssInfos[i];
      parcel->write(*ss);
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SET_SERVICE_STATUS, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsSetServiceStatus:resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsDial(const RIL_IMS_Dial& dial, const ImsDialCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(dial);
            return reqMgr->issueRequest(RIL_IMS_REQUEST_DIAL, std::move(parcel),
                    [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                        cb(e);
                    });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::imsAnswer(const RIL_IMS_Answer& answer, const ImsAnswerCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(answer);
            return reqMgr->issueRequest(RIL_IMS_REQUEST_ANSWER, std::move(parcel),
                    [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                        cb(e);
                    });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::imsHangup(const RIL_IMS_Hangup& hangup, const ImsHangupCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->write(hangup);
            return reqMgr->issueRequest(RIL_IMS_REQUEST_HANGUP, std::move(parcel),
                    [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                        cb(e);
                    });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::imsModifyCallInitiate(const RIL_IMS_CallModifyInfo& modifyInfo,
                                            const ImsModifyCallInitiateCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_MODIFY_CALL_INITIATE, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsModifyCallInitiate:resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsModifyCallConfirm(const RIL_IMS_CallModifyInfo& modifyInfo,
                                           const ImsModifyCallConfirmCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(modifyInfo);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_MODIFY_CALL_CONFIRM, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsModifyCallConfirm:resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsCancelModifyCall(const uint32_t& callId,
                                          const ImsCancelModifyCallCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(callId);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_CANCEL_MODIFY_CALL, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsCancelModifyCall:resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsAddParticipant(const char* address, const ImsAddParticipantCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(address);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_ADD_PARTICIPANT, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SipErrorInfo errorDetails{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(errorDetails);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsAddParticipant:resp: e=" + std::to_string(e));
          cb(e, errorDetails);
          if (p) {
            p->release(errorDetails);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetImsConfig(const RIL_IMS_ConfigInfo& config,
                                      const ImsSetImsConfigCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(config);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SET_IMS_CONFIG, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_ConfigInfo resultConfig{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(resultConfig);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsSetImsConfig:resp: e=" + std::to_string(e));
          cb(e, resultConfig);
          if (p) {
            p->release(resultConfig);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsGetImsConfig(const RIL_IMS_ConfigInfo& config,
                                      const ImsGetImsConfigCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(config);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_GET_IMS_CONFIG, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_ConfigInfo resultConfig{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(resultConfig);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsGetImsConfig:resp: e=" + std::to_string(e));
          cb(e, resultConfig);
          if (p) {
            p->release(resultConfig);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsQueryCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                                const ImsQueryCallForwardStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(cfInfo);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_QUERY_CALL_FORWARD_STATUS, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_QueryCallForwardStatusInfo resultCfStatus{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(resultCfStatus);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsQueryCallForwardStatus:resp: e=" + std::to_string(e));
          cb(e, resultCfStatus);
          if (p) {
            p->release(resultCfStatus);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetCallForwardStatus(const RIL_IMS_CallForwardInfo& cfInfo,
                                              const ImsSetCallForwardStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(cfInfo);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SET_CALL_FORWARD_STATUS, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SetCallForwardStatusInfo resultCfStatus{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(resultCfStatus);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsSetCallForwardStatus:resp: e=" + std::to_string(e));
          cb(e, resultCfStatus);
          if (p) {
            p->release(resultCfStatus);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSuppSvcStatus(const RIL_IMS_SuppSvcRequest& suppSvc,
                                          const ImsSuppSvcStatusCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
      return Status::FAILURE;
    }
    auto ret = parcel->write(suppSvc);
    if (ret != Marshal::Result::SUCCESS) {
      return Status::FAILURE;
    }
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SUPP_SVC_STATUS, std::move(parcel),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SuppSvcResponse suppSvcResp{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(suppSvcResp);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsSuppSvcStatus:resp: e=" + std::to_string(e));
          cb(e, suppSvcResp);
          if (p) {
            p->release(suppSvcResp);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsExplicitCallTransfer(const RIL_IMS_ExplicitCallTransfer& explicitCall,
                                              const ImsExplicitCallTransferCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(explicitCall);
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_EXPLICIT_CALL_TRANSFER, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("imsExplicitCallTransfer:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsGetClir(const ImsGetClirCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_GET_CLIR, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
            if (p != nullptr) {
              RIL_IMS_ClirInfo clirInfo{};
              p->read(clirInfo);
              Log::getInstance().d("imsGetClir:resp: e=" + std::to_string(e));
              cb(e, clirInfo);
            }
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetClir(const RIL_IMS_SetClirInfo& clirInfo,
                                              const ImsSetClirCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(clirInfo);
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_SET_CLIR, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("imsSetClir:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSendUiTtyMode(const RIL_IMS_TtyNotifyInfo& ttyInfo,
                                              const ImsSendUiTtyModeCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(ttyInfo);
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_SEND_UI_TTY_MODE, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("imsSendUiTtyMode:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsDeflectCall(const RIL_IMS_DeflectCallInfo& deflectCall,
                                              const ImsDeflectCallCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(deflectCall);
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_DEFLECT_CALL, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            Log::getInstance().d("imsDeflectCall:resp: e=" + std::to_string(e));
            cb(e);
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsQueryClip(const ImsQueryClipCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_QUERY_CLIP, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
            if (p != nullptr) {
              RIL_IMS_ClipInfo clipInfo{};
              p->read(clipInfo);
              Log::getInstance().d("imsQueryClip:resp: e=" + std::to_string(e));
              cb(e, clipInfo);
              if (p) {
                p->release(clipInfo);
              }
            }
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetSuppSvcNotification(const RIL_IMS_ServiceClassStatus& serviceStatus,
                                                const ImsSetSuppSvcNotificationCallback& cb) {
  if (reqMgr) {
    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (parcel != nullptr) {
      parcel->write(serviceStatus);
      return reqMgr->issueRequest(
          RIL_IMS_REQUEST_SET_SUPP_SVC_NOTIFICATION, std::move(parcel),
          [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
            if (p != nullptr) {
              RIL_IMS_ServiceClassStatus status{};
              p->read(status);
              Log::getInstance().d("imsSetSuppSvcNotification:resp: e=" + std::to_string(e));
              cb(e, status);
            }
          });
    }
  }
  return Status::FAILURE;
}

Status RilApiSession::imsSetColr(const RIL_IMS_ColrInfo& colr,
        const ImsSetColrCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (!parcel) return Status::FAILURE;
        auto ret = parcel->write(colr);
        if (ret != Marshal::Result::SUCCESS) {
            return Status::FAILURE;
        }
        return reqMgr->issueRequest(RIL_IMS_REQUEST_SET_COLR, std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_IMS_SipErrorInfo errInfo{};
                    if (p && p->dataAvail()) {
                        p->read(errInfo);
                    }
                    cb(e, errInfo);
                    if (p) {
                      p->release(errInfo);
                    }
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::imsGetColr(const ImsGetColrCallback& cb) {
    if (reqMgr) {
        return reqMgr->issueRequest(RIL_IMS_REQUEST_GET_COLR, nullptr,
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_IMS_ColrInfo rilColr{};
                    if (p && p->dataAvail()) {
                        p->read(rilColr);
                    }
                    cb(e, rilColr);
                    if (p) {
                      p->release(rilColr);
                    }
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::imsRegisterMultiIdentityLines(const RIL_IMS_MultiIdentityLineInfo
        lineInfo[], size_t size, const ImsRegisterMultiIdentityLinesCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (!parcel) return Status::FAILURE;
        auto ret = parcel->write(lineInfo, size);
        if (ret != Marshal::Result::SUCCESS) {
            return Status::FAILURE;
        }
        return reqMgr->issueRequest(RIL_IMS_REQUEST_REGISTER_MULTI_IDENTITY_LINES,
                std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
                    cb(e);
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::imsQueryVirtualLineInfo(const char* msisdn,
        const ImsQueryVirtualLineInfoCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (!parcel) return Status::FAILURE;
        auto ret = parcel->write(msisdn);
        if (ret != Marshal::Result::SUCCESS) {
            return Status::FAILURE;
        }
        return reqMgr->issueRequest(RIL_IMS_REQUEST_QUERY_VIRTUAL_LINE_INFO,
                std::move(parcel),
                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    RIL_IMS_QueryVirtualLineInfoResponse info{};
                    if (p && p->dataAvail()) {
                        p->read(info);
                    }
                    cb(e, info);
                    if (p) {
                      p->release(info);
                    }
                });
    }
    return Status::FAILURE;
}

Status RilApiSession::imsSendSms(const RIL_IMS_SmsMessage& smsMessage, const ImsSendSmsCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(smsMessage) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SEND_SMS,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_IMS_SendSmsResponse resp {};
            resp.status = RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_ERROR;
            resp.messageRef = 0;

            if (p->read(resp) != Marshal::Result::SUCCESS) {
                resp.status = RIL_IMS_SmsSendStatus::RIL_IMS_SMS_SEND_STATUS_ERROR;
                resp.messageRef = 0;
                if (e == RIL_Errno::RIL_E_SUCCESS) {
                    e = RIL_Errno::RIL_E_INTERNAL_ERR;
                }
            }

            cb(e, resp);
        }
    );
}

Status RilApiSession::imsAckSms(const RIL_IMS_SmsAck& smsAck, const ImsAckSmsCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(smsAck) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_ACK_SMS,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::imsGetSmsFormat(const ImsGetSmsFormatCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_GET_SMS_FORMAT,
        nullptr,
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_IMS_SmsFormat smsFormat = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_UNKNOWN;
            if (p->read(smsFormat) != Marshal::Result::SUCCESS) {
                smsFormat = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_UNKNOWN;
                if (e == RIL_Errno::RIL_E_SUCCESS) {
                    e = RIL_Errno::RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, smsFormat);
        }
    );
}

Status RilApiSession::imsSetCallWaiting(const RIL_IMS_CallWaitingSettings& callWaitingSettings,
        const ImsSetCallWaitingCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(callWaitingSettings) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_SET_CALL_WAITING,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_IMS_SipErrorInfo sipError {};
            if (p->read(sipError) != Marshal::Result::SUCCESS) {
                sipError.errorCode = std::numeric_limits<uint32_t>::max();
                sipError.errorString = nullptr;
                if (e == RIL_Errno::RIL_E_SUCCESS) {
                    e = RIL_Errno::RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, sipError);
            if (p) {
              p->release(sipError);
            }
        }
    );
}

Status RilApiSession::imsQueryCallWaiting(uint32_t serviceClass, const ImsQueryCallWaitingCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(serviceClass) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_QUERY_CALL_WAITING,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_IMS_QueryCallWaitingResponse response {};
            if (p->read(response) != Marshal::Result::SUCCESS) {
                response.callWaitingSettings.enabled = false;
                response.callWaitingSettings.serviceClass = std::numeric_limits<uint32_t>::max();
                response.sipError.errorCode = std::numeric_limits<uint32_t>::max();
                response.sipError.errorString = nullptr;
                if (e == RIL_Errno::RIL_E_SUCCESS) {
                    e = RIL_Errno::RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, response);
            if (p) {
              p->release(response);
            }
        }
    );
}

Status RilApiSession::registerImsUnsolRegistrationStateIndicationHandler(
    const ImsRegistrationStateIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_Registration regInfo{};
        if (p && p->dataAvail()) {
          p->read(regInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED");
          indicationHandler(regInfo);
          p->release(regInfo);
        }
      });
}

Status RilApiSession::registerImsUnsolServiceStatusIndicationHandler(
    const ImsServiceStatusIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        std::vector<RIL_IMS_ServiceStatusInfo> ssInfoVector;
        int32_t numSsInfos = 0;
        if (p && p->dataAvail()) {
          p->read(numSsInfos);
          for (int i = 0; (i < numSsInfos) && p->dataAvail(); i++) {
            RIL_IMS_ServiceStatusInfo ss{};
            p->read(ss);
            ssInfoVector.push_back(ss);
          }
          RIL_IMS_ServiceStatusInfo **ssInfos = nullptr;
          if (numSsInfos) {
            ssInfos = new RIL_IMS_ServiceStatusInfo*[numSsInfos]();
          }
          if(ssInfos) {
            for (int i = 0; (i < numSsInfos); i++) {
              ssInfos[i] = &ssInfoVector[i];
            }
          }
          Log::getInstance().d("RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED");
          indicationHandler(numSsInfos, (const RIL_IMS_ServiceStatusInfo **)ssInfos);
          // Free
          if (p) {
            for (auto &e : ssInfoVector) {
              p->release(e);
            }
          }
          delete[] ssInfos;
        }
      });
}

Status RilApiSession::registerImsUnsolSubConfigChangeIndicationHandler(
    const ImsSubConfigChangeIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_IMS_SUB_CONFIG_CHANGED,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_SubConfigInfo subConfigInfo{};
        if (p && p->dataAvail()) {
          p->read(subConfigInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_IMS_SUB_CONFIG_CHANGED");
          indicationHandler(subConfigInfo);
          // Free
          p->release(subConfigInfo);
        }
      });
}

Status RilApiSession::registerImsUnsolCallRingIndicationHandler(
    const ImsCallRingIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_CALL_RING,
      [indicationHandler](std::shared_ptr<Marshal> /*p*/) {
        Log::getInstance().d("RIL_IMS_UNSOL_CALL_RING");
        indicationHandler();
      });
}

Status RilApiSession::registerImsUnsolRingBackToneIndicationHandler(
    const ImsRingBackToneIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_RINGBACK_TONE,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_ToneOperation toneOp{};
        if (p && p->dataAvail()) {
          p->read(toneOp);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_RINGBACK_TONE");
        indicationHandler(toneOp);
      });
}

Status RilApiSession::registerImsUnsolEnterEcbmIndicationHandler(
    const ImsEnterEcbmIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE,
      [indicationHandler](std::shared_ptr<Marshal> /*p*/) {
        Log::getInstance().d("RIL_IMS_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE");
        indicationHandler();
      });
}

Status RilApiSession::registerImsUnsolExitEcbmIndicationHandler(
    const ImsExitEcbmIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,
      [indicationHandler](std::shared_ptr<Marshal> /*p*/) {
        Log::getInstance().d("RIL_IMS_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE");
        indicationHandler();
      });
}

Status RilApiSession::registerImsIncomingSmsHandler(const ImsIncomingSmsHandler& handler) {
    return registerIndicationHandler(
        RIL_IMS_UNSOL_INCOMING_IMS_SMS,
        [handler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                Log::getInstance().d("No parcel received.");
                return;
            }

            RIL_IMS_IncomingSms incomingSms {};
            if (p->read(incomingSms) == Marshal::Result::SUCCESS) {
                Log::getInstance().d("Calling handler for RIL_IMS_UNSOL_INCOMING_IMS_SMS.");
                handler(incomingSms);
                if (incomingSms.pdu) {
                    delete[] incomingSms.pdu;
                    incomingSms.pdu = nullptr;
                    incomingSms.pduLength = 0;
                }
            } else {
                Log::getInstance().d("Failed to read parcel.");
            }
        }
    );
}

Status RilApiSession::registerImsSmsStatusReportHandler(const ImsSmsStatusReportHandler& handler) {
    return registerIndicationHandler(
        RIL_IMS_UNSOL_SMS_STATUS_REPORT,
        [handler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                Log::getInstance().d("No parcel received.");
                return;
            }

            RIL_IMS_SmsStatusReport smsStatusReport {};
            if (p->read(smsStatusReport) == Marshal::Result::SUCCESS) {
                Log::getInstance().d("Calling handler for RIL_IMS_UNSOL_SMS_STATUS_REPORT.");
                handler(smsStatusReport);
                if (smsStatusReport.pdu) {
                    delete[] smsStatusReport.pdu;
                    smsStatusReport.pdu = nullptr;
                    smsStatusReport.pduLength = 0;
                }
            } else {
                Log::getInstance().d("Failed to read parcel.");
            }
        }
    );
}

Status RilApiSession::registerImsViceInfoHandler(const ImsViceInfoHandler& handler) {
    return registerIndicationHandler(
        RIL_IMS_UNSOL_REFRESH_VICE_INFO,
        [handler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                Log::getInstance().d("No parcel received.");
                return;
            }

            std::vector<uint8_t> viceInfoUri;
            if (p->read(viceInfoUri) == Marshal::Result::SUCCESS) {
                Log::getInstance().d("Calling handler for RIL_IMS_UNSOL_REFRESH_VICE_INFO.");
                handler(viceInfoUri);
            } else {
                Log::getInstance().d("Failed to read parcel.");
            }
        }
    );
}

Status RilApiSession::registerImsTtyNotificationHandler(const ImsTtyNotificationHandler& handler) {
    return registerIndicationHandler(
        RIL_IMS_UNSOL_TTY_NOTIFICATION,
        [handler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                Log::getInstance().d("No parcel received.");
                return;
            }

            RIL_IMS_TtyModeType ttyMode = RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_INVALID;
            if (p->read(ttyMode) == Marshal::Result::SUCCESS) {
                Log::getInstance().d("Calling handler for RIL_IMS_UNSOL_TTY_NOTIFICATION.");
                handler(ttyMode);
            } else {
                Log::getInstance().d("Failed to read parcel.");
            }
        }
    );
}

Status RilApiSession::getCdmaSubscription(const GetCdmaSubInfoCallback& cb) {
    if (reqMgr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            return reqMgr->issueRequest(RIL_REQUEST_CDMA_SUBSCRIPTION, std::move(parcel),
                                [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                                    RIL_CDMASubInfo info{};
                                    const char* out[5] = {nullptr, nullptr, nullptr,
                                        nullptr, nullptr};
                                    if (p && p->dataAvail()) {
                                        if (p->read<RIL_CDMASubInfo>(info) !=
                                                Marshal::Result::SUCCESS) {
                                            e = RIL_E_INTERNAL_ERR;
                                        }
                                        out[0] = info.mdn.c_str();
                                        out[1] = info.hSid.c_str();
                                        out[2] = info.hNid.c_str();
                                        out[3] = info.min.c_str();
                                        out[4] = info.prl.c_str();
                                    }
                                    cb(e, out);
                                });
        }
    }
    return Status::FAILURE;
}

Status RilApiSession::registerIncoming3GppSmsIndicationHandler(
        const Incoming3GppSmsIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_NEW_SMS,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (!p) {
                return;
            }

            uint8_t* pdu = nullptr;
            size_t pduLength = 0;
            if (p->readAndAlloc(pdu, pduLength) == Marshal::Result::SUCCESS
                    && pduLength != 0 && pdu != nullptr) {
                indicationHandler(pdu, pduLength);
            }

            if (pdu != nullptr) {
                delete[] pdu;
            }
        }
    );
}

Status RilApiSession::registerBroadcastSmsIndicationHandler(
        const BroadcastSmsIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p) {
                std::vector<uint8_t> pdu;
                if (p->read(pdu) == Marshal::Result::SUCCESS) {
                    indicationHandler(pdu);
                }
            }
        }
    );
}

Status RilApiSession::registerModemRestartIndicationHandler(
        const ModemRestartIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_MODEM_RESTART,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p) {
                std::string restartReason;
                if (p->read(restartReason) == Marshal::Result::SUCCESS) {
                    indicationHandler(restartReason);
                }
            }
        }
    );
}

Status RilApiSession::registerRilServerReadyIndicationHandler(
        const RilServerReadyIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
        RIL_UNSOL_RIL_CONNECTED,
        [indicationHandler] (std::shared_ptr<Marshal> p) {
            if (p) {
                int rilVersion;
                if (p->read(rilVersion) == Marshal::Result::SUCCESS) {
                    indicationHandler(rilVersion);
                }
            }
        }
    );
}

Status RilApiSession::registerOemhookIndicationHandler(
    const OemhookIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_UNSOL_OEM_HOOK_RAW,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        if (p && p->dataAvail()) {
          char* data = nullptr;
          size_t dataLen = 0;
          if (p->readAndAlloc(data, dataLen) == Marshal::Result::SUCCESS) {
            if (dataLen != 0 && data != nullptr) {
              Log::getInstance().d("RIL_UNSOL_OEM_HOOK_RAW");
              indicationHandler(data, dataLen);
            }
          }
          if (data != nullptr) {
            delete[] data;
          }
        }
      });
}

Status RilApiSession::writeSmsToSim(const std::string& pdu, const std::string& smscAddress,
                                    const int status, const WriteSmsToSimCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    RIL_SMS_WriteArgs smsWriteArgs {};
    smsWriteArgs.pdu = const_cast<char*>(pdu.c_str());
    smsWriteArgs.smsc = const_cast<char*>(smscAddress.c_str());
    smsWriteArgs.status = status;

    if (parcel->write(smsWriteArgs) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_WRITE_SMS_TO_SIM,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            int32_t recordIndex = -1;
            if (!p || p->read(recordIndex) != Marshal::Result::SUCCESS) {
                recordIndex = -1;
                if (e == RIL_E_SUCCESS) {
                    e = RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, recordIndex);
        }
    );
}

Status RilApiSession::deleteSmsOnSim(int32_t recordIndex, const DeleteSmsOnSimCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    if (parcel->write(recordIndex) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_DELETE_SMS_ON_SIM,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::reportSmsMemoryStatus(bool storageSpaceAvailable, const ReportSmsMemoryStatusCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(storageSpaceAvailable) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_REPORT_SMS_MEMORY_STATUS,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::setCdmaSubscriptionSource(RIL_CdmaSubscriptionSource subscriptionSource,
                                                const SetCdmaSubscriptionSourceCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    if (parcel->write(subscriptionSource) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::getCdmaSubscriptionSource(const GetCdmaSubscriptionSourceCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_CdmaSubscriptionSource subSource = CDMA_SUBSCRIPTION_SOURCE_INVALID;
            if (!p || p->read(subSource) != Marshal::Result::SUCCESS) {
                subSource = CDMA_SUBSCRIPTION_SOURCE_INVALID;
                if (e == RIL_E_SUCCESS) {
                    e = RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, subSource);
        }
    );
}

Status RilApiSession::setCdmaRoamingPreference(RIL_CdmaRoamingPreference roamingPref, const SetCdmaRoamingPreferenceCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    if (parcel->write(roamingPref) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::getCdmaRoamingPreference(const GetCdmaRoamingPreferenceCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> p) {
            RIL_CdmaRoamingPreference roamingPref = CDMA_ROAMING_PREFERENCE_INVALID;
            if (!p || p->read(roamingPref) != Marshal::Result::SUCCESS) {
                roamingPref = CDMA_ROAMING_PREFERENCE_INVALID;
                if (e == RIL_E_SUCCESS) {
                    e = RIL_E_INTERNAL_ERR;
                }
            }
            cb(e, roamingPref);
        }
    );
}

Status RilApiSession::setSignalStrengthReportingCriteria(
        const RIL_SignalStrengthReportingCriteria& criteria,
        const SetSignalStrengthReportingCriteriaCallback& cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
    if (!parcel || parcel->write(criteria) != Marshal::Result::SUCCESS) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA,
        std::move(parcel),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::oemhookRaw(const char *data, size_t dataLen, const OemhookRawCallback& cb) {
  if (!reqMgr) {
    return Status::FAILURE;
  }
  std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
  if (!parcel) {
    return Status::FAILURE;
  }
  if (parcel->write(data, dataLen) != Marshal::Result::SUCCESS) {
    return Status::FAILURE;
  }
  return reqMgr->issueRequest(
      RIL_REQUEST_OEM_HOOK_RAW, std::move(parcel),
      [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
        size_t dataLen = 0;
        char* data = nullptr;
        if (p && p->dataAvail()) {
          if ((p->readAndAlloc(data, dataLen) != Marshal::Result::SUCCESS) ||
              (dataLen == 0) || (data == nullptr)) {
            e = RIL_E_INTERNAL_ERR;
          }
        }
        Log::getInstance().d("oemhookRaw resp: e=" + std::to_string(e) +
                             ", dataLen=" + std::to_string(dataLen));
        cb(e, data, dataLen);
        if (data) {
          delete []data;
        }
      });
}

Status RilApiSession::imsDtmf(char digit, const VoidResponseHandler &cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> p = std::make_unique<Marshal>();
    if (!p) {
        return Status::FAILURE;
    }
    p->write(digit);
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_DTMF,
        std::move(p),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::imsDtmfStart(char digit, const VoidResponseHandler &cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    std::unique_ptr<Marshal> p = std::make_unique<Marshal>();
    if (!p) {
        return Status::FAILURE;
    }
    p->write(digit);
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_DTMF_START,
        std::move(p),
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}


Status RilApiSession::imsDtmfStop(const VoidResponseHandler &cb) {
    if (!reqMgr) {
        return Status::FAILURE;
    }

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_DTMF_STOP,
        nullptr,
        [cb] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            cb(e);
        }
    );
}

Status RilApiSession::registerImsUnsolCallStateChangedIndicationHandler(
        const ImsCallStateChangedIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_IMS_UNSOL_CALL_STATE_CHANGED,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    size_t sz = 0;
                    RIL_IMS_CallInfo *calls = nullptr;
                    if (p->readAndAlloc(calls, sz) == Marshal::Result::SUCCESS) {
                        indicationHandler(calls, sz);
                        p->release(calls, sz);
                    }
                }
            }
    );
}

Status RilApiSession::registerImsUnsolModifyCallIndicationHandler(
        const ImsModifyCallIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_MODIFY_CALL,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_CallModifyInfo modifyInfo{};
        if (p && p->dataAvail()) {
          p->read(modifyInfo);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_MODIFY_CALL");
        indicationHandler(modifyInfo);
      });
}

Status RilApiSession::registerImsUnsolMessageWaitingIndicationHandler(
        const ImsMessageWaitingIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_MESSAGE_WAITING,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_MessageWaitingInfo mwi{};
        if (p && p->dataAvail()) {
          p->read(mwi);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_MESSAGE_WAITING");
        indicationHandler(mwi);
      });
}

Status RilApiSession::registerImsUnsolSuppSvcNotificationIndicationHandler(
    const ImsSuppSvcNotificationIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_SuppSvcNotification ssInfo{};
        if (p && p->dataAvail()) {
          p->read(ssInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION");
          indicationHandler(ssInfo);
          p->release(ssInfo);
        }
      });
}

Status RilApiSession::registerImsUnsolOnSsIndicationHandler(
    const ImsOnSsIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_ON_SS,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_StkCcUnsolSsResponse ssInfo{};
        if (p && p->dataAvail()) {
          p->read(ssInfo);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_ON_SS");
        indicationHandler(ssInfo);
      });
}

Status RilApiSession::registerImsUnsolVopsChangedIndicationHandler(
        const ImsVopsChangedIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_VOPS_CHANGED,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        uint8_t vopsStatus;
        if (p && p->dataAvail()) {
          p->read(vopsStatus);
          Log::getInstance().d("RIL_IMS_UNSOL_VOPS_CHANGED");
          indicationHandler(vopsStatus);
        }
      });
}

Status RilApiSession::registerImsUnsolParticipantStatusInfoIndicationHandler(
    const ImsParticipantStatusInfoIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_ParticipantStatusInfo partipantStatusInfo{};
        if (p && p->dataAvail()) {
          p->read(partipantStatusInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO");
          indicationHandler(partipantStatusInfo);
          p->release(partipantStatusInfo);
        }
      });
}

Status RilApiSession::registerImsUnsolRegistrationBlockStatusIndicationHandler(
    const ImsRegistrationBlockStatusIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_RegistrationBlockStatus regBlockStatus{};
        if (p && p->dataAvail()) {
          p->read(regBlockStatus);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS");
        indicationHandler(regBlockStatus);
      });
}

Status RilApiSession::registerImsUnsolWfcRoamingModeConfigSupportIndicationHandler(
            const ImsWfcRoamingModeConfigSupportIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        uint8_t wfcRoamingSupported = 1;
        if (p && p->dataAvail()) {
          p->read(wfcRoamingSupported);
        }
        Log::getInstance().d("RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT");
        indicationHandler(wfcRoamingSupported);
      });
}

Status RilApiSession::registerImsUnsolUssdFailedIndicationHandler(
            const ImsUssdFailedIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_USSD_FAILED,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_UssdInfo ussdInfo{};
        if (p && p->dataAvail()) {
          p->read(ussdInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_USSD_FAILED");
          indicationHandler(ussdInfo);
          p->release(ussdInfo);
        }
      });
}

Status RilApiSession::registerImsUnsolVoiceInfoIndicationHandler(
            const ImsVoiceInfoIndicationHandler& indicationHandler) {
  return registerIndicationHandler(
      RIL_IMS_UNSOL_VOICE_INFO,
      [indicationHandler](std::shared_ptr<Marshal> p) {
        RIL_IMS_VoiceInfoType voiceInfo;
        if (p && p->dataAvail()) {
          p->read(voiceInfo);
          Log::getInstance().d("RIL_IMS_UNSOL_VOICE_INFO");
          indicationHandler(voiceInfo);
        }
      });
}

Status RilApiSession::imsRequestConference(const ImsRequestConferenceCallback& cb) {
  if (reqMgr) {
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_CONFERENCE,
        nullptr,
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SipErrorInfo errorDetails{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(errorDetails);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsRequestConference resp: e=" + std::to_string(e));
          cb(e, errorDetails);
          if (p) {
            p->release(errorDetails);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsRequestHoldCall(uint32_t callId, const ImsRequestHoldCallback& cb) {
  if (reqMgr) {

    std::unique_ptr<Marshal> p = std::make_unique<Marshal>();
    if (!p) {
        return Status::FAILURE;
    }
    p->write(callId);

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_HOLD,
        std::move(p),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SipErrorInfo errorDetails{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(errorDetails);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsRequestHoldCall resp: e=" + std::to_string(e));
          cb(e, errorDetails);
          if (p) {
            p->release(errorDetails);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsRequestResumeCall(uint32_t callId, const ImsRequestResumeCallback& cb) {
  if (reqMgr) {

    std::unique_ptr<Marshal> p = std::make_unique<Marshal>();
    if (!p) {
        return Status::FAILURE;
    }
    p->write(callId);

    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_RESUME,
        std::move(p),
        [cb](RIL_Errno e, std::shared_ptr<Marshal> p) {
          RIL_IMS_SipErrorInfo errorDetails{};
          if (p && p->dataAvail() != 0) {
            auto ret = p->read(errorDetails);
            if (ret != Marshal::Result::SUCCESS) {
              e = RIL_E_GENERIC_FAILURE;
            }
          }
          Log::getInstance().d("imsRequestResumeCall resp: e=" + std::to_string(e));
          cb(e, errorDetails);
          if (p) {
            p->release(errorDetails);
          }
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::imsRequestExitEmergencyCallbackMode(const ImsExitECBMCallback& cb) {
  if (reqMgr) {
    return reqMgr->issueRequest(
        RIL_IMS_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE,
        nullptr,
        [cb](RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
          Log::getInstance().d("imsRequestExitEmergencyCallbackMode resp: e=" + std::to_string(e));
          cb(e);
        });
  }
  return Status::FAILURE;
}

Status RilApiSession::registerImsUnsolHandoverIndicationHandler(const ImsHandoverIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
            RIL_IMS_UNSOL_HANDOVER,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    RIL_IMS_HandoverInfo handoverInfo{};
                    if (p->read(handoverInfo) == Marshal::Result::SUCCESS) {
                        indicationHandler(handoverInfo);
                        p->release(handoverInfo);
                    }
                }
            }
    );
}

Status RilApiSession::registerImsUnsolRefreshConferenceInfo(const ImsRefreshConferenceInfoIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
            RIL_IMS_UNSOL_REFRESH_CONF_INFO,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if( p && p->dataAvail()) {
                    RIL_IMS_RefreshConferenceInfo confInfo {};
                    if (p->read(confInfo) == Marshal::Result::SUCCESS) {
                        indicationHandler(confInfo);
                        p->release(confInfo);
                    }
                }
            }
    );
}
Status RilApiSession::registerImsUnsolMultiIdentityPendingInfo(const VoidIndicationHandler& indicationHandler) {
    return registerIndicationHandler(
            RIL_IMS_UNSOL_MULTI_IDENTITY_PENDING_INFO,
            [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
                indicationHandler();
            }
    );
}

Status RilApiSession::registerImsUnsolMultiIdentityRegistrationStatus(
        const MultiIdentityRegistrationStatusHandler indicationHandler) {
    return registerIndicationHandler(
            RIL_IMS_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p && p->dataAvail()) {
                    std::vector<RIL_IMS_MultiIdentityLineInfo> lines;
                    if (p->read(lines) == Marshal::Result::SUCCESS) {
                        indicationHandler(lines.data(), lines.size());
                        for (auto &e : lines) {
                          p->release(e);
                        }
                    }
                }
            }
    );
}

Status RilApiSession::registerAdnInitDone(
        const VoidIndicationHandler indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_RESPONSE_ADN_INIT_DONE,
            [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
                indicationHandler();
            }
    );
}

Status RilApiSession::registerSimStatusChanged(
        const VoidIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
            [indicationHandler] (std::shared_ptr<Marshal> /*p*/) {
                indicationHandler();
            }
    );
}

Status RilApiSession::registerStkCcAlphaNotify(
        const CharStarIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_STK_CC_ALPHA_NOTIFY,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    std::string data{};
                    if (p->read(data) == Marshal::Result::SUCCESS) {
                        indicationHandler(data.c_str());
                    }
                }
            }
    );
}

Status RilApiSession::registerSimRefresh(
        const SimRefreshIndicationHandler &indicationHandler) {
    return registerIndicationHandler(
            RIL_UNSOL_SIM_REFRESH,
            [indicationHandler] (std::shared_ptr<Marshal> p) {
                if (p) {
                    RIL_SimRefreshResponse_v7 payload{};
                    if (p->read(payload) == Marshal::Result::SUCCESS) {
                        indicationHandler(&payload);
                    }
                }
            }
    );
}

