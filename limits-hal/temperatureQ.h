/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TEMPERATURE_Q_H__
#define TEMPERATURE_Q_H__

#include <vector>
#include <android-base/logging.h>

#define TEMP_HIST_SIZE 6
#define DEFAULT_POLLING_MSEC 1000
#define DEFAULT_HEADROOM (INT_MAX)

namespace vendor::qti::hardware::limits::implementation {

class tempQ {
public:
	tempQ(float thresh = 95);
	void UpdateThreshold(float val);
	void add(float val);
	int estimateHeadroom();

private:
	int hr;
	float threshold;
	std::vector<float> temp;
};

} // namespace vendor::qti::hardware::limits::implementation

#endif // TEMPERATURE_Q_H__
