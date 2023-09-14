/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 NTRIP Client Library API

 Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/

#ifndef NTRIP_CLIENT_H
#define NTRIP_CLIENT_H

#include <functional>
#include <loc_misc_utils.h>

using std::string;

/** @fn
    @brief
    CorrectionDataCb is for dealing with correction data
*/
typedef std::function<void(uint8_t *correctionData, uint32_t lengthInBytes)>
        CorrectionDataCb;

struct DataStreamingNtripSettings {
    /** Ntrip server name / IP address*/
    string hostName;
    /** Mount Point*/
    string mountPoint;
    /** User Name*/
    string userName;
    /** Password*/
    string password;
    /** Port Number*/
    uint32_t port;
    /** whether use NMEA location or not*/
    bool requireNmeaLocation;
    /** NMEA GGA sentence*/
    string nmeaGGA;
    /** whether use SSL or not*/
    bool useSSL;
};


class NtripClient {
public:
    inline virtual ~NtripClient() {};
    /** get NtripClient instance
    * Do NOT  change the implementation of this inline method.
    * The caller is from a prebuilt library. Changing this inline method
    * would NOT be able to change the implementation that is already built in.
    */
    typedef NtripClient* (*creator_t)();
    static inline NtripClient* getInstance() {
        char libName[LOC_MAX_PARAM_NAME] = {};
        uint8_t nameSet = 0;
        const loc_param_s_type conf[] = {
            {"NTRIP_CLIENT_LIB_NAME", libName, &nameSet, 's'}
        };
        UTIL_READ_CONF(LOC_PATH_GPS_CONF, conf);

        NtripClient* ntripClient = nullptr;
        if (nameSet) {
            void* libHandle = nullptr;
            creator_t creator = (creator_t)dlGetSymFromLib(libHandle, libName,
                    "getNtripClientInstance");

            if (nullptr != creator) {
                ntripClient = creator();
            }
        }
        return ntripClient;
    };

    /** @brief API to start streaming for correction data.
     *   @param
     *   ntripSettings, NTRIP settings for data streaming
     *   @param
     *   corrDataCb, Callback to deal with the correction data
     */
    virtual void startCorrectionDataStreaming(DataStreamingNtripSettings& ntripSettings,
                         CorrectionDataCb corrDataCb) = 0;

    /** @brief API to stop streaming for correction data.
    */
    virtual void stopCorrectionDataStreaming() = 0;
    /** @brief API to update Nmea to Ntrip Caster.
     *  @param
     *  nmea, nmea sentence such as GGA
     */
    virtual void updateNmeaToNtripCaster(string& nmea) = 0;

};


#endif //NTRIP_CLIENT_H
