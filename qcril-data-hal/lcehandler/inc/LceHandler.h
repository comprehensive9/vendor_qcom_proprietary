/******************************************************************************
#  Copyright (c) 2018, 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef LCEHANDLER
#define LCEHANDLER

#include <map>
#include "request/SetLinkCapRptCriteriaMessage.h"
#include "request/SetLinkCapFilterMessage.h"
#include "qcril_data_defs.h"

using namespace rildata;

/**
 * @brief      Main class for Link Capacity Estimation handling.
 *
 */
class LceHandler {
private:
   ReportFilter_t m_reportingState;
   const string mName = "LceHandler";
   /**
    * Local cache for when we need to re-apply criteria
    */
   std::map<AccessNetwork_t, qcril_data_lce_rpt_criteria> m_criteriaCache;

   /**
    * @brief      convert to qcril_data_lce_rpt_criteria
    *
    * @param[in]  in    LinkCapCriteria_t
    *
    * @return     qcril_data_lce_rpt_criteria
    */
   static inline qcril_data_lce_rpt_criteria
   convertCriteria(const LinkCapCriteria_t& in) {
      qcril_data_lce_rpt_criteria out;
      memset( &out, 0x0, sizeof(qcril_data_lce_rpt_criteria) );
      out.hysteresisMs = in.hysteresisMs;
      out.hysteresisDlKbps = in.hysteresisDlKbps;
      out.hysteresisUlKbps = in.hysteresisUlKbps;
      for (auto i = 0; i < (int) in.thresholdsDownlinkKbps.size(); i++) {
         out.thresholdsDownlinkKbps[i] = in.thresholdsDownlinkKbps[i];
      }
      out.thresholdsDownlinkKbps_size = in.thresholdsDownlinkKbps.size();
      for (auto i = 0; i < (int) in.thresholdsUplinkKbps.size(); i++) {
         out.thresholdsUplinkKbps[i] = in.thresholdsUplinkKbps[i];
      }
      out.thresholdsUplinkKbps_size = in.thresholdsUplinkKbps.size();
      if (in.ran == AccessNetwork_t::UTRAN) {
         out.ran = OTT_RAT_WCDMA_V01;
      } else if (in.ran == AccessNetwork_t::EUTRAN) {
         out.ran = OTT_RAT_LTE_V01;
      } else if (in.ran == AccessNetwork_t::NGRAN) {
         out.ran = OTT_RAT_NR5G_V01;
      }

      return out;
   }
public:
   /**
    * @brief      Toogle reporting
    *
    * @param[in]  reporting_state  The reporting state
    *
    * @return     true if handled
    */
   RIL_Errno toggleReporting(ReportFilter_t reporting_state);
   /**
    * @brief      Sets the reporting criteria.
    *
    * @param[in]  criteria  The criteria
    *
    * @return     result
    */
   LinkCapCriteriaResult_t setCriteria(LinkCapCriteria_t criteria);

   LceHandler() {
    m_reportingState = ReportFilter_t::disabled;
    memset(&m_criteriaCache[AccessNetwork_t::NGRAN], 0, sizeof(qcril_data_lce_rpt_criteria));
    memset(&m_criteriaCache[AccessNetwork_t::EUTRAN], 0, sizeof(qcril_data_lce_rpt_criteria));
    memset(&m_criteriaCache[AccessNetwork_t::UTRAN], 0, sizeof(qcril_data_lce_rpt_criteria));
   }

   virtual ~LceHandler() {}
};

#endif
