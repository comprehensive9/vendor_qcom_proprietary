/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATACONFIG
#define DATACONFIG

#include "framework/Log.h"
#ifdef RIL_FOR_MDM_LE

#include "qcril_config.h"
#else
  #include <cutils/properties.h>
#endif

#define TAG "DATACONFIG"

#ifdef __cplusplus
#include <string>

using namespace std;

namespace rildata {

#ifdef RIL_FOR_MDM_LE
  #include <unordered_map>
  void propertySetMap
  (
     string s1,
     property_id p1
  );
#endif

string qcril_get_property_value
(
   string name,
   string defaultValue
);
string readProperty
(
   string id,
   string propValue
);

} /* namespace rildata */
#else

char* readProperty
(
  char id[],
  char* propValue,
  char defaultValue[]
);

#endif

#endif