/******************************************************************************
#  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <unistd.h>
#include <algorithm>
#include <framework/Log.h>
#include <RilApiSession.hpp>
#include <iostream>
#include <stdlib.h>


void testDataCall(RilApiSession* session) {
    //RIL_REQUEST_SETUP_DATA_CALL
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession testDataCall enter ");
        RIL_RadioAccessNetworks accessNetwork = (RIL_RadioAccessNetworks)0x03;
        bool roamingAllowed = true;
        RIL_RadioDataRequestReasons reason = (RIL_RadioDataRequestReasons)0x01;
        RIL_DataProfileInfo_IRadio_1_4 dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo_IRadio_1_4));
        dataProfile.profileId = 1;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IP");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IP");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred =0 ;
        dataProfile.persistent = 1;
        if ( session->setupDataCall( accessNetwork, dataProfile, roamingAllowed, reason, [] (RIL_Errno err, Status status, const RIL_Data_Call_Response_v11* response) -> void {
            Log::getInstance().d("Got response for setupDatacall request: " + std::to_string(err));
            if (status == Status::FAILURE) {
                Log::getInstance().d("SetupDataCall : Failure in socket data read. Exiting testcase");
                return;
            }
            if(response) {
                 Log::getInstance().d("[RilApiSessionSampleApp]: status :"+std::to_string(response->status));
                 Log::getInstance().d("[RilApiSessionSampleApp]: suggestedRetryTime :"+std::to_string(response->suggestedRetryTime));
                 Log::getInstance().d("[RilApiSessionSampleApp]: cid :"+std::to_string(response->cid));
                 Log::getInstance().d("[RilApiSessionSampleApp]: active :"+std::to_string(response->active));
                 if(response->type) {
                     std::string str(response->type);
                     Log::getInstance().d("[RilApiSessionSampleApp]:: type :"+str);
                 }
                 if(response->ifname) {
                    std::string str(response->ifname);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: ifname :"+str);
                 }
                 if(response->addresses) {
                    std::string str(response->addresses);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: addresses :"+str);
                 }
                 if(response->dnses) {
                    std::string str(response->dnses);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: dnses :"+str);
                 }
                 if(response->gateways) {
                    std::string str(response->gateways);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: gateways :"+str);
                 }
                 if(response->pcscf) {
                    std::string str(response->pcscf);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: pcscf :"+str);
                 }
                 Log::getInstance().d("[RilApiSessionSampleApp]:: mtu :"+std::to_string(response->mtu));
            } else {
                Log::getInstance().d("[RilApiSessionSampleApp]:: response is NULL!!!");
            }
        }) == Status::FAILURE) {
            Log::getInstance().d("[RilApiSessionSampleApp]: SetupDataCall Failed Due to Marshalling Failure.");
        }
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetupDataCall exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testDeactivateCall(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession deactivateDataCall enter ");
        int32_t cid = 0;
        bool reason = true;
        session->deactivateDataCall( cid, reason, [] (RIL_Errno err) -> void {
            Log::getInstance().d("Got response for deactivateDataCall request: " + std::to_string(err)); });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession deactivateDataCall exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolDataCallListChanged(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession testUnsolDataCallListChanged enter ");
        session->registerDataCallListChangedIndicationHandler(
            [] (const RIL_Data_Call_Response_v11 response[], size_t sz) {
                Log::getInstance().d("[RilApiSessionSampleApp]: RadioDataCallListChangeInd  size = "+std::to_string(sz));
                for (int i = 0; (i < sz); i++) {
                    Log::getInstance().d("[RilApiSessionSampleApp]: RadioDataCallListChangeInd reading entry ");
                    Log::getInstance().d("[RilApiSessionSampleApp]: status :"+std::to_string(response[i].status));
                    Log::getInstance().d("[RilApiSessionSampleApp]: suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime));
                    Log::getInstance().d("[RilApiSessionSampleApp]: cid :"+std::to_string(response[i].cid));
                    Log::getInstance().d("[RilApiSessionSampleApp]: active :"+std::to_string(response[i].active));
                    if(response[i].type) {
                        std::string str(response[i].type);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: type :"+str);
                    }
                    if(response[i].ifname) {
                        std::string str(response[i].ifname);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: ifname :"+str);
                    }
                    if(response[i].addresses) {
                        std::string str(response[i].addresses);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: addresses :"+str);
                    }
                    if(response[i].dnses) {
                        std::string str(response[i].dnses);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: dnses :"+str);
                    }
                    if(response[i].gateways) {
                        std::string str(response[i].gateways);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: gateways :"+str);
                    }
                    if(response[i].pcscf) {
                        std::string str(response[i].pcscf);
                        Log::getInstance().d("[RilApiSessionSampleApp]:: pcscf :"+str);
                    }
                    Log::getInstance().d("[RilApiSessionSampleApp]:: mtu :"+std::to_string(response[i].mtu));
                }
                Log::getInstance().d("Received DataCallListChangedIndication");
            }
        );
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetInitialAttachApn(RilApiSession* session){
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: testSetInitialAttachApn enter ");
        RIL_DataProfileInfo_IRadio_1_4 dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo_IRadio_1_4));
        dataProfile.profileId = 1;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IPV4");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IPV4");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred =0 ;
        dataProfile.persistent = 1;
        session->setInitialAttachApn( dataProfile, [] (RIL_Errno err) -> void {
            Log::getInstance().d("Got response for setInitialAttachApn request: " + std::to_string(err)); });
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}


void testGetDataCallList(RilApiSession* session){
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList enter ");
        session->getDataCallList( [] (const RIL_Errno e, const RIL_Data_Call_Response_v11* response, const unsigned long sz) -> void {
            Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList status: " + std::to_string(e));
            if(!response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList response NULL ");
                return;
            }
            Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList  received Data Call size = "+std::to_string(sz));
            for (int i = 0; (i < sz); i++) {
                Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList reading entry ");
                Log::getInstance().d("[RilApiSessionSampleApp]: status :"+std::to_string(response[i].status));
                Log::getInstance().d("[RilApiSessionSampleApp]: suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime));
                Log::getInstance().d("[RilApiSessionSampleApp]: cid :"+std::to_string(response[i].cid));
                Log::getInstance().d("[RilApiSessionSampleApp]: active :"+std::to_string(response[i].active));
                if(response[i].type) {
                    std::string str(response[i].type);
                Log::getInstance().d("[RilApiSessionSampleApp]:: type :"+str);
                }
                if(response[i].ifname) {
                    std::string str(response[i].ifname);
                Log::getInstance().d("[RilApiSessionSampleApp]:: ifname :"+str);
                }
                if(response[i].addresses) {
                    std::string str(response[i].addresses);
                Log::getInstance().d("[RilApiSessionSampleApp]:: addresses :"+str);
                }
                if(response[i].dnses) {
                    std::string str(response[i].dnses);
                Log::getInstance().d("[RilApiSessionSampleApp]:: dnses :"+str);
                }
                if(response[i].gateways) {
                    std::string str(response[i].gateways);
                Log::getInstance().d("[RilApiSessionSampleApp]:: gateways :"+str);
                }
                if(response[i].pcscf) {
                    std::string str(response[i].pcscf);
                Log::getInstance().d("[RilApiSessionSampleApp]:: pcscf :"+str);
                }
                Log::getInstance().d("[RilApiSessionSampleApp]:: mtu :"+std::to_string(response[i].mtu));
            }
            Log::getInstance().d("Received testGetDataCallList Done!!");
        });
        Log::getInstance().d("[RilApiSessionSampleApp]: testGetDataCallList exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetDataProfile(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: testSetDataProfile enter ");
        RIL_DataProfileInfo_IRadio_1_4 dataProfile;
        memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo_IRadio_1_4));
        dataProfile.profileId = 0;
        std::string apnname("w1v4v6.com");
        dataProfile.apn = const_cast<char*>(apnname.c_str());
        std::string protocoltype("IPV4V6");
        dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
        std::string roamingProtocoltype("IPV4V6");
        dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
        dataProfile.authType = 0;
        std::string username("xyz");
        dataProfile.user = const_cast<char*>(username.c_str());
        std::string password("xyz");
        dataProfile.password = const_cast<char*>(password.c_str());
        dataProfile.type = 1;
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        dataProfile.supportedTypesBitmask = 0X1;
        dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile.mtu = 1400;
        dataProfile.preferred = 0 ;
        dataProfile.persistent = 1;

        RIL_DataProfileInfo_IRadio_1_4 dataProfile2;
        memset(&dataProfile2, 0, sizeof(RIL_DataProfileInfo_IRadio_1_4));

        dataProfile2.profileId = 2;

        std::string apnname_1("wirelessone.com");
        dataProfile2.apn = const_cast<char*>(apnname_1.c_str());

        std::string protocoltype_1("IPV6");
        dataProfile2.protocol = const_cast<char*>(protocoltype_1.c_str());

        std::string roamingProtocoltype_1("IPV6");
        dataProfile2.roamingProtocol = const_cast<char*>(roamingProtocoltype_1.c_str());

        dataProfile2.authType = 0;

        std::string username_1("xyz");
        dataProfile2.user = const_cast<char*>(username_1.c_str());

        std::string password_1("xyz");
        dataProfile2.password = const_cast<char*>(password_1.c_str());

        dataProfile2.type = 1;
        dataProfile2.maxConnsTime = 0;
        dataProfile2.maxConns = 0;
        dataProfile2.waitTime = 1;
        dataProfile2.enabled = 1;
        dataProfile2.supportedTypesBitmask = 0X1;
        dataProfile2.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
        dataProfile2.mtu = 1400;
        dataProfile2.preferred =0 ;
        dataProfile2.persistent = 1;

        RIL_DataProfileInfo_IRadio_1_4 dataProfileList[2] = {  dataProfile, dataProfile2 };
        RIL_DataProfileInfo_IRadio_1_4 *dataProfilePtr = dataProfileList;

        if(dataProfilePtr) {
           int len = sizeof(dataProfileList)/sizeof(RIL_DataProfileInfo_IRadio_1_4);
           Log::getInstance().d(" Sending SetDataProfile  ");
           session->setDataProfile( dataProfilePtr, len, [] (RIL_Errno err) -> void { Log::getInstance().d("Got response for setDataProfile request: " + std::to_string(err)); });
        }
        Log::getInstance().d(" testSetDataProfile done!! ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testStartLceData(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StartLCEData enter ");
        int32_t interval = 1000;
        int32_t mode = 1;
        session->StartLceData( interval, mode, [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
            Log::getInstance().d("Got response for StartLCEData request: " + std::to_string(err));
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:LCE status ="
                + std::to_string(response->lce_status));
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:Actual Interval in ms = "
                + std::to_string(response->actual_interval_ms));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StartLCEData exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testStopLceData(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StopLCEData enter ");
        session->StopLceData( [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
            Log::getInstance().d("Got response for StopLCEData request: " + std::to_string(err));
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:LCE status ="
                + std::to_string(response->lce_status));
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:Actual Interval in ms = "
                + std::to_string(response->actual_interval_ms));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StopLCEData exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testPullLceData(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession PullLCEData enter ");
        session->PullLceData( [] (RIL_Errno err, const RIL_LceDataInfo* response) -> void {
            Log::getInstance().d("Got response for PullLCEData request: " + std::to_string(err));
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:Last hop capacity in kbps ="
                + std::to_string(response->last_hop_capacity_kbps));
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:Confidence level = "
                + std::to_string(response->confidence_level));
                Log::getInstance().d("[RilApiSessionSampleApp]: Pull LCE:LCE suspended status="
                + std::to_string(response->lce_suspended));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession PullLCEData exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolPcoDataChanged(RilApiSession* session) {
    if (session) {
        session->registerPcoDataChangeHandler(
            [] (const  RIL_PCO_Data PcoData) {
                Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: cid = "
                + std::to_string(PcoData.cid));
                if (PcoData.bearer_proto) {
                    std::string str(PcoData.bearer_proto);
                    Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: BearerProtocol = "
                    + str);
                }
                else {
                    Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: bearer_proto is NULL");
                }
                Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: PcoId "
                + std::to_string(PcoData.pco_id));
                if (PcoData.contents) {
                    for (int i = 0; i < PcoData.contents_length; i++) {
                        Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: content"
                        + std::to_string(i) + "=" + std::to_string(PcoData.contents[i]));
                    }
                }
                else {
                    Log::getInstance().d("[RilApiSessionSampleApp]:: UnsolPcoDataChanged: contents is NULL");
                }
                Log::getInstance().d("Received PCODataChangeIndication");
            }
        );
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolLCEDataChanged(RilApiSession* session) {
    if (session) {
        session->registerLCEDataChangeHandler(
            [] (const  RIL_LinkCapacityEstimate LCEData) {
                Log::getInstance().d("[RilApiSessionSampleApp]: UnsolLceDataChanged:Downlink capacity in kbps ="
                + std::to_string(LCEData.downlinkCapacityKbps));
                Log::getInstance().d("[RilApiSessionSampleApp]: UnsolLceDataChanged:Uplink capacity in kbps = "
                + std::to_string(LCEData.uplinkCapacityKbps));
                Log::getInstance().d("Received LCEDataChangeIndication");
            }
        );
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testRadioKeepAliveChanged(RilApiSession* session) {
    if (session) {
        session->registerRadioKeepAliveHandler(
        [] (const Status status, const uint32_t handle, const RIL_KeepaliveStatusCode statuscode) {
            if (status == Status::FAILURE) {
                Log::getInstance().d("UnsolRadioKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            Log::getInstance().d("[RilApiSessionSampleApp]: UnsolRadioKeepAlive:Handle ="
            + std::to_string(handle));
            Log::getInstance().d("[RilApiSessionSampleApp]: UnsolRadioKeepAlive:Status = "
            + std::to_string(statuscode));
            Log::getInstance().d("Received RadioKeepAliveIndication");
        });
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetLinkCapFilterMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetLinkCapFilterMessage enter ");
        RIL_ReportFilter enable_bit = RIL_ReportFilter::enabled;
        session->SetLinkCapFilter(enable_bit, [] (RIL_Errno err, const int* response) -> void {
            Log::getInstance().d("Got response for SetLinkCapFilterMessage request: " + std::to_string(err));
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: SetLinkCapFilter:Response is ="
                + std::to_string(*response));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetLinkCapFilterMessage exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testUnsolDataNrIconChange(RilApiSession* session) {
    if (session) {
        session->registerDataNrIconChangeHandler(
        [] (const Status status, const  five_g_icon_type icon) {
            if (status == Status::FAILURE) {
                Log::getInstance().d("UnsolDataNrIconChange : Failure in socket data read. Exiting testcase");
                return;
            }
            Log::getInstance().d("[RilApiSessionSampleApp]: UnsolDataNrIconChange: NR Icon Type = "
            + std::to_string(static_cast<int>(icon)));
            Log::getInstance().d("Received UnsolDataNrIconChange");
            }
        );
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void PhysicalConfigStructUpdateMessage(RilApiSession* session) {
    if (session) {
        if ( session->registerPhysicalConfigStructHandler(
            [] (const Status status, const  std::vector<RIL_PhysicalChannelConfig> arg) {
            if (status == Status::FAILURE) {
                Log::getInstance().d("PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase");
                return;
            }
            for (int j = 0; j < arg.size(); j++) {
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: List content = "
                + std::to_string(j));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: status = "
                + std::to_string(static_cast<int>(arg[j].status)));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: BandwidthDownlink = "
                + std::to_string(arg[j].cellBandwidthDownlink));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Network RAT "
                + std::to_string(static_cast<int>(arg[j].rat)));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Range = "
                + std::to_string(static_cast<int>(arg[j].rfInfo.range)));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: ChannelNumber = "
                + std::to_string(arg[j].rfInfo.channelNumber));
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Number of Context Ids = "
                + std::to_string(arg[j].num_context_ids));
                for (int i = 0; i < arg[j].num_context_ids; i++) {
                    Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Cid " + std::to_string(i) + " = "
                    + std::to_string(arg[j].contextIds[i]));
                }
                Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: physicalCellId = "
                + std::to_string(arg[j].physicalCellId));
            }
            Log::getInstance().d("Received PhysicalConfigStructUpdate");
            }
        ) == Status::FAILURE) {
            Log::getInstance().d("[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Failed Due to Marshalling Failure.");
        }
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testGetDataNrIconTypeMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession GetDataNrIconType enter ");
        session->GetDataNrIconType([] (RIL_Errno err, const Status status, const five_g_icon_type* response) -> void {
            Log::getInstance().d("Got response for GetDataNrIconType request: " + std::to_string(err));
            if (status == Status::FAILURE) {
                Log::getInstance().d("GetDataNrIconType : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: GetDataNrIconType:Nr Icon Type is ="
                + std::to_string(*response));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession GetDataNrIconType exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testStopKeepAliveMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StopKeepAlive enter ");
        int handle = -1;
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StopKeepAlive enter handle id");
        std::cin >> handle;
        session->StopKeepAlive(handle, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
            Log::getInstance().d("Got response for StopKeepAlive request: " + std::to_string(err));
            if (status == Status::FAILURE) {
                Log::getInstance().d("StopKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: StopKeepAlive:RIL response is ="
                + std::to_string(*response));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StopKeepAlive exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testStartKeepAliveMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StartKeepAlive enter ");
        RIL_KeepaliveRequest request = {};
        request.type = RIL_KeepaliveType::NATT_IPV4;
        string s = "";
        int x = -1;
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter IPV4 source address");
        std::cin >> s;
        strlcpy(request.sourceAddress, s.c_str(), (s.length()+1));
        s = "";
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter IPV4 source port");
        std::cin >> x;
        request.sourcePort = x;
        x = -1;
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter IPV4 destination address");
        std::cin >> s;
        strlcpy(request.destinationAddress, s.c_str(), (s.length()+1));
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter IPV4 destination port");
        std::cin >> x;
        request.destinationPort = x;
        x = -1;
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter interval in miliseconds");
        std::cin >> x;
        request.maxKeepaliveIntervalMillis = x;
        x = -1;
        Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive enter cid");
        std::cin >> x;
        request.cid = x;
        session->StartKeepAlive(request, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response,
        const uint32_t* handle, const RIL_KeepaliveStatus_t* statuscode) -> void {
            Log::getInstance().d("Got response for StartKeepAlive request: " + std::to_string(err));
            if (status == Status::FAILURE) {
                Log::getInstance().d("StartKeepAlive : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive:Response Error is ="
                + std::to_string(*response));
            }
            if (handle) {
                Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive:Handle is ="
                + std::to_string(*handle));
            }
            if (statuscode) {
                Log::getInstance().d("[RilApiSessionSampleApp]: StartKeepAlive:Status is ="
                + std::to_string(*statuscode));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession StartKeepAlive exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetPreferredDataModemRequestMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetPreferredDataModem enter ");
        int modemId = 0;
        session->SetPreferredDataModem(modemId, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
            Log::getInstance().d("Got response for SetPreferredDataModem request: " + std::to_string(err));
            if (status == Status::FAILURE) {
                Log::getInstance().d("SetPreferredDataModem : Failure in socket data read. Exiting testcase");
                return;
            }
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: SetPreferredDataModem:Response is ="
                + std::to_string(*response));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetPreferredDataModem exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

void testSetLinkCapRptCriteriaMessage(RilApiSession* session) {
    if (session) {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetLinkCapRptCriteriaMessage enter ");
        RIL_LinkCapCriteria criteria = {};
        criteria.hysteresisMs = 3000;
        criteria.hysteresisDlKbps = 50;
        criteria.hysteresisUlKbps = 50;
        criteria.thresholdsDownLength = 11;
        criteria.thresholdsDownlinkKbps = (int*) calloc (criteria.thresholdsDownLength, sizeof(int));
        if (criteria.thresholdsDownlinkKbps) {
            criteria.thresholdsDownlinkKbps[0] = 100;
            criteria.thresholdsDownlinkKbps[1] = 500;
            criteria.thresholdsDownlinkKbps[2] = 1000;
            criteria.thresholdsDownlinkKbps[3] = 5000;
            criteria.thresholdsDownlinkKbps[4] = 10000;
            criteria.thresholdsDownlinkKbps[5] = 20000;
            criteria.thresholdsDownlinkKbps[6] = 50000;
            criteria.thresholdsDownlinkKbps[7] = 100000;
            criteria.thresholdsDownlinkKbps[8] = 200000;
            criteria.thresholdsDownlinkKbps[9] = 500000;
            criteria.thresholdsDownlinkKbps[10] = 1000000;
        }
        criteria.thresholdsUpLength = 9;
        criteria.thresholdsUplinkKbps = (int*) calloc (criteria.thresholdsUpLength, sizeof(int));
        if (criteria.thresholdsUplinkKbps) {
            criteria.thresholdsUplinkKbps[0] = 100;
            criteria.thresholdsUplinkKbps[1] = 500;
            criteria.thresholdsUplinkKbps[2] = 1000;
            criteria.thresholdsUplinkKbps[3] = 5000;
            criteria.thresholdsUplinkKbps[4] = 10000;
            criteria.thresholdsUplinkKbps[5] = 20000;
            criteria.thresholdsUplinkKbps[6] = 50000;
            criteria.thresholdsUplinkKbps[7] = 100000;
            criteria.thresholdsUplinkKbps[8] = 200000;
        }
        criteria.ran = RIL_RAN::LTE_RAN;
        session->SetLinkCapRptCriteria(criteria, [] (RIL_Errno err, const RIL_LinkCapCriteriaResult* response) -> void {
            Log::getInstance().d("Got response for SetLinkCapRptCriteriaMessage request: " + std::to_string(err));
            if (response) {
                Log::getInstance().d("[RilApiSessionSampleApp]: SetLinkCapRptCriteria:Response is ="
                + std::to_string(static_cast<uint8_t>(*response)));
            }
            });
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession SetLinkCapRptCriteriaMessage exit ");
    }
    else {
        Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession is NULL. Exiting Testcase");
    }
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
    Log::getInstance().d("[RilApiSessionSampleApp]: RilApiSession sample usage");
    RilApiSession session("/dev/socket/qcrild/rild0");
    Status s = session.initialize(
        [] (Status s) {
            Log::getInstance().d("Error detected in session. Error: " + std::to_string((int) s));
        }
    );

    if (s != Status::SUCCESS) {
        Log::getInstance().d("[RilApiSessionSampleApp]: Failed to initialize API session.");
    }

    RIL_Dial dialParams;
    std::string addr("0123456789");
    dialParams.address = const_cast<char*>(addr.c_str());
    dialParams.clir = 0;
    dialParams.uusInfo = nullptr;

    testUnsolDataCallListChanged(&session);
    testUnsolLCEDataChanged(&session);
    testUnsolPcoDataChanged(&session);
    testUnsolDataNrIconChange(&session);
    testRadioKeepAliveChanged(&session);
    // Disable this while loop for other testing .Data only test use below while loop
    while(true) {
        int x = 0;
        Log::getInstance().d("=======AVAILABLE OPTIONS===========");
        Log::getInstance().d("1  :: testSetInitialAttachApn");
        Log::getInstance().d("2  :: testSetDataProfile");
        Log::getInstance().d("3  :: testDataCall");
        Log::getInstance().d("4  :: testDeactivateCall");
        Log::getInstance().d("5  :: getDataCallList");
        Log::getInstance().d("6  :: StartLCE");
        Log::getInstance().d("7  :: StopLCE");
        Log::getInstance().d("8  :: PullLCE");
        Log::getInstance().d("9  :: SetLinkCapFilter");
        Log::getInstance().d("10 :: SetLinkCapRptCriteria");
        Log::getInstance().d("11 :: GetDataNrIconType");
        Log::getInstance().d("12 :: SetPreferredDataModem");
        Log::getInstance().d("13 :: StartKeepAlive");
        Log::getInstance().d("14 :: StopKeepAlive");
        Log::getInstance().d("Enter the option");
        std::cin >> x;
        Log::getInstance().d("input choice selected===>"+ std::to_string(x));
        switch (x) {
            case 1:
                Log::getInstance().d("Choice is 1 -testSetInitialAttachApn");
                testSetInitialAttachApn(&session);
                break;
            case 2:
                Log::getInstance().d("Choice is 2 -SetDataProfile");
                testSetDataProfile(&session);
                break;
            case 3:
                Log::getInstance().d("Choice is 3 testDataCall");
                testDataCall(&session);
                break;
            case 4:
                Log::getInstance().d("Choice is 4 testDeactivateCall");
                testDeactivateCall(&session);
                break;
            case 5:
                Log::getInstance().d("Choice is 5 get testGetDataCallList");
                testGetDataCallList(&session);
                break;
            case 6:
                Log::getInstance().d("Choice is 6 get testStartLceData");
                testStartLceData(&session);
                break;
            case 7:
                Log::getInstance().d("Choice is 7 get testStopLceData");
                testStopLceData(&session);
                break;
            case 8:
                Log::getInstance().d("Choice is 8 get testPullLceData");
                testPullLceData(&session);
                break;
            case 9:
                Log::getInstance().d("Choice is 9 SetLinkCapFilter");
                testSetLinkCapFilterMessage(&session);
                break;
            case 10:
                Log::getInstance().d("Choice is 10 SetLinkCapRptCriteria");
                testSetLinkCapRptCriteriaMessage(&session);
                break;
            case 11:
                Log::getInstance().d("Choice is 11 GetDataNrIconType");
                testGetDataNrIconTypeMessage(&session);
                break;
            case 12:
                Log::getInstance().d("Choice is 12 SetPreferredDataModem");
                testSetPreferredDataModemRequestMessage(&session);
                break;
            case 13:
                Log::getInstance().d("Choice is 13 StartKeepAlive");
                testStartKeepAliveMessage(&session);
                break;
            case 14:
                Log::getInstance().d("Choice is 14 StopKeepAlive");
                testStopKeepAliveMessage(&session);
                break;
            default:
                Log::getInstance().d("Choice other than 1 to 14");
                break;
        }
        sleep(60);
    }

    while (true) {
        session.dial(
            dialParams,
            [] (RIL_Errno err) -> void {
                Log::getInstance().d("Got response for dial request: " + std::to_string(err));
            }
        );

        session.registerSignalStrengthIndicationHandler(
            [] (const RIL_SignalStrength& signalStrength) {
                (void) signalStrength;
                Log::getInstance().d("Received signal strength indication");
            }
        );

        session.registerCellInfoIndicationHandler(
            [] (const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
                (void) cellInfoList;
                (void) cellInfoListLen;
                Log::getInstance().d("Received cell info indication");
            }
        );

        session.getCellInfo(
            [] (RIL_Errno err, const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
                Log::getInstance().d("Got response for get cell info request: " + std::to_string(err));

                if (cellInfoList == nullptr || cellInfoListLen == 0) {
                    Log::getInstance().d("No cell info received.");
                    return;
                } else {
                    Log::getInstance().d(
                        std::to_string(cellInfoListLen) +
                        " elements in the cell info array."
                    );
                }

                std::for_each(
                    cellInfoList,
                    cellInfoList + cellInfoListLen,
                    [] (const RIL_CellInfo_v12& cellInfo) {
                        const char* mcc = nullptr;
                        const char* mnc = nullptr;

                        switch (cellInfo.cellInfoType) {
                            case RIL_CELL_INFO_TYPE_GSM:
                                mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                                mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_LTE:
                                mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                                mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_WCDMA:
                                mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                                mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                                break;
                            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                                mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                                mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                                break;
                            default:
                                break;
                        }

                        Log::getInstance().d(
                            "cellInfoType: " + std::to_string(cellInfo.cellInfoType) +
                            " registered: " + std::to_string(cellInfo.registered) +
                            " mcc: " + (mcc ? mcc : "") +
                            " mnc: " + (mnc ? mnc : "")
                        );
                    }
                );
            }
        );

        sleep(60);
    }
}
