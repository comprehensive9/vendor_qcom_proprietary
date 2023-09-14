/******************************************************************************
#  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_service_1_5.h>
#include <framework/Log.h>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <RilServiceModule_1_4.h>
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"
#include <UnSolMessages/RadioDataCallListChangeIndMessage.h>
#include <UnSolMessages/GetDataCallListResponseIndMessage.h>
#include <request/SetupDataCallRequestMessage.h>
#include <SsrIndicationMessage.h>

class RilServiceModule_1_5: public RilServiceModule_1_4 {
    private:
        RadioImpl_1_5 &mRadioImpl_1_5;
        static constexpr unsigned int LTE_EXP_COUNT{3};
        unsigned int lte_req_count{0};
        unsigned int lte_disabled_count{0};
        static constexpr unsigned int NR5G_EXP_COUNT{3};
        unsigned int nr5g_req_count{0};
        unsigned int nr5g_disabled_count{0};

    public:
        RilServiceModule_1_5(qcril_instance_id_e_type instance, RadioImpl_1_5 &radioImpl);
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleUiccAppsStatusChangedMessage(
                std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);
        void handleNwRegistrationRejectMessage(
                std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg);
        void handleCellBarringMessage(
                std::shared_ptr<RilUnsolCellBarringMessage> msg);
        void handleSsrIndicationMessage(
                std::shared_ptr<SsrIndicationMessage> msg);
        /**
         * Resets the request and disabled count for LTE.
         * Must reset the counts when:
         *  1) a new telephony client connects.
         *  2) on Sub System Restart
         *  3) Received LTE request count equals expected count
         */
        void resetLteCounts();

        /**
         * Resets the request and disabled count for NR5G.
         * Must reset the counts when:
         *  1) a new telephony client connects.
         *  2) on Sub System Restart
         *  3) Received NR5G request count equals expected count
         */
        void reset5gnrCounts();

        /**
         * Increment the request count when a call to
         * setSignalStrengthReportingCriteria_1_5 is made to enable/disable
         * a signal measurement type for LTE.
         */
        void incLteReqCnt();

        /**
         * Increment the disabled count when a call to
         * setSignalStrengthReportingCriteria_1_5 is made to disable a
         * signal measurement type for LTE.
         */
        void incLteDisCnt();

        /**
         * Increment the request count when a call to
         * setSignalStrengthReportingCriteria_1_5 is made to enable/disable
         * a signal measurement type for NR5G.
         */
        void incNr5gReqCnt();

        /**
         * Increment the disabled count when a call to
         * setSignalStrengthReportingCriteria_1_5 is made to disable a
         * signal measurement type for NR5G.
         */
        void incNr5gDisCnt();

        /**
         * Checks if all signal types for LTE are disabled. If all disabled,
         * resets the request and disabled counts for LTE.
         *
         * @return true if all signal types are disabled, else false.
         */
        bool checkLteDisCnt();

        /**
         * Checks if all signal types for NR5G are disabled. If all disabled,
         * resets the request and disabled counts for NR5G.
         *
         * @return true if all signal types are disabled, else false.
         */
        bool checkNr5gDisCnt();

        /**
         * check if all requests for LTE are received and reset request and
         * disable count.
         */
        void checkLteReqCnt();

        /**
         * check if all requests for NR5G are received and reset request and
         * disable count.
         */
        void checkNr5gReqCnt();

        void handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage_1_5> msg);
        void handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage_1_5> msg);
        void handleGetDataCallListResponseIndMessage(std::shared_ptr<rildata::GetDataCallListResponseIndMessage> msg);
};

