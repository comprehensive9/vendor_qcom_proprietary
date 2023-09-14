/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "temperatureQ.h"
#include <android-base/logging.h>

namespace vendor::qti::hardware::limits::implementation {

tempQ::tempQ(float thresh) {
	threshold = thresh;
	hr = DEFAULT_HEADROOM;
}

void tempQ::UpdateThreshold(float val) {
	threshold = val;
}

void tempQ::add(float val)
{
	std::vector<float>::iterator it = temp.begin();

	if (temp.size() == TEMP_HIST_SIZE)
		temp.pop_back();
	temp.insert(it, val);
}

int tempQ::estimateHeadroom()
{
	int i = 0, tmp = 0;
	float x = 0, x2 = 0, y = 0, xy = 0, b1, b2;

	if (temp.size() < TEMP_HIST_SIZE)
		return DEFAULT_HEADROOM;
	if (temp.front() >= threshold) {
		hr = 0;
		return hr;
	}
	/*
	 * Algorithm reference:
	 * https://www.codesansar.com/numerical-methods/linear-regression-method-algorithm.htm
	 */
	for (i = 0; i < temp.size(); i++) {
		x += (i + 1);
		x2 += ((i + 1) * (i + 1));
		y += temp[i];
		xy += (i + 1) * temp[i];
	}
	/* y = b1 * x + b2 */
	b1 = (TEMP_HIST_SIZE * xy - x * y) / (TEMP_HIST_SIZE * x2 - x * x);
	b2 = (y - b1 * x) / TEMP_HIST_SIZE;

	if (b1 <= 0) {
		if (hr == 0) //temp is coming down from threshold temp
			hr = DEFAULT_HEADROOM;
		return hr;
	}
	tmp = (threshold - b2) / b1;
	/* Check for possible overflow */
	if (tmp <= (INT_MAX / DEFAULT_POLLING_MSEC))
		hr = tmp * DEFAULT_POLLING_MSEC;
	else
		hr = DEFAULT_HEADROOM;

	LOG(DEBUG) << "x:" << x << " x2:" << x2 << " y:" << y << " xy:" << xy << std::endl;
	LOG(DEBUG) << "b1:" << b1 << " b2:"<< b2 << " thresh:" << threshold << std::endl;
	LOG(DEBUG) << "hr: " << hr << std::endl;

	return hr;
}

}  // namespace vendor::qti::hardware::limits::implementation
