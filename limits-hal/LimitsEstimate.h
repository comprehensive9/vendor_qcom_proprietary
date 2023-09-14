/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef LIMITS_ESTIMATE_H__
#define LIMITS_ESTIMATE_H__

#include <mutex>
#include <cmath>
#include <pthread.h>
#include <android/hardware/thermal/2.0/IThermal.h>

#include "temperatureQ.h"

#define UNKNOWN_TEMPERATURE (NAN)

namespace vendor::qti::hardware::limits::implementation {

using ::android::sp;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::thermal::V2_0::IThermal;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::thermal::V1_0::ThermalStatusCode;
using Temperature = ::android::hardware::thermal::V2_0::Temperature;
using TemperatureType = ::android::hardware::thermal::V2_0::TemperatureType;
using TemperatureThreshold =
	::android::hardware::thermal::V2_0::TemperatureThreshold;
using ThrottlingSeverity =
	::android::hardware::thermal::V2_0::ThrottlingSeverity;

enum tempType {
	CPU_TEMP,
	GPU_TEMP,
	NSP_TEMP,
	SOC_TEMP,
	SKIN_TEMP,
	MAX_TEMP_TYPE
};

static TemperatureType tempTypeMap[MAX_TEMP_TYPE] = {
	TemperatureType::CPU,
	TemperatureType::GPU,
	TemperatureType::NPU,
	TemperatureType::BCL_PERCENTAGE,
	TemperatureType::SKIN
};

static std::vector<std::string> tempName = {
	"CPU",
	"GPU",
	"NSP",
	"SoCD",
	"skin"};

class LimitsEstimate {
	public:
		LimitsEstimate();
		~LimitsEstimate();

		int getHeadRoom();
		void stopEstimate(void);
		void startEstimate(void);
		void temperatureCB(ThermalStatus status,
				const hidl_vec<Temperature>& temps);
		void thresholdCB(ThermalStatus status,
				const hidl_vec<TemperatureThreshold>& temps);
		static void *limitsPoll(void *data);
	private:
		std::mutex estimateMutex;
		pthread_mutex_t waitMutex;
		pthread_cond_t waitCond;
		pthread_t pollThread;
		sp<android::hardware::thermal::V2_0::IThermal> ThermalHal;
		tempQ temp[MAX_TEMP_TYPE];
		bool pollingMode, exitMode;

		void estimate();
};

} // namespace vendor::qti::hardware::limits::implementation

#endif  // LIMITS_ESTIMATE_H__
