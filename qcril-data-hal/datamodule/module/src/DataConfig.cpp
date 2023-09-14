/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "DataConfig.h"
#define PROPERTY_VALUE_MAX 256

namespace rildata {

#ifdef RIL_FOR_MDM_LE

unordered_map <std::string, property_id> property_map;
void propertySetMap(string s1, property_id p1)
{
  property_map[s1] = p1;
}

#endif

string readProperty(string id, string propValue)
{
#ifdef RIL_FOR_MDM_LE
  if (property_map.find(id) ==  property_map.end())
  {
    Log::getInstance().d("[DataConfig]: Could not find cache entry for " + id );
    return propValue;
  }
  Log::getInstance().d("[DataConfig]: Attempting to read property " + to_string(property_map[id]));

  if (qcril_config_get(property_map[id], propValue) == E_SUCCESS)
  {
    Log::getInstance().d("[DataConfig]:" + id + " property is " + propValue);
  }
  else
  {
    Log::getInstance().d("[DataConfig]:" + id + " property could not be read ");
  }
  return propValue;

#else

  propValue = qcril_get_property_value(id, propValue);
  Log::getInstance().d("[DataConfig]:" + id + " property is " + propValue);
  return propValue;

#endif
}

#ifndef RIL_FOR_MDM_LE

string qcril_get_property_value(string name, string defaultValue) {
  char cPropValue[PROPERTY_VALUE_MAX] = {'\0'};
  property_get(name.c_str(), cPropValue, defaultValue.c_str());
  return string(cPropValue);
}

#endif

}

extern "C" {

#ifdef RIL_FOR_MDM_LE

static bool checkListOfProperties(char id[], enum property_id *id1)
{
  if ( id1 && (strcmp(id, "persist.vendor.radio.force_ltd_sys_ind") == 0) )
  {
    *id1 = PERSIST_VENDOR_RADIO_FORCE_LTD_SYS_IND;
    return true;
  }
  else if (id1 && (strcmp(id, "persist.vendor.data.profile_update") == 0))
  {
    *id1 = PERSIST_VENDOR_DATA_PROFILE_UPDATE;
    return true;
  }
  else
  {
    QCRIL_LOG_DEBUG("Property %s could not be found in cache", id);
    return false;
  }
}
char* readPropertyLE(char id[], char* propValue, char defaultValue[])
{

  enum property_id id1 = (enum property_id) 0;
  if( !checkListOfProperties(id, &id1) )
  {
    propValue = (char*) (calloc ((sizeof(defaultValue) / sizeof(char)), sizeof(char)));
    if (propValue != NULL)
    {
      strlcpy(propValue, defaultValue, (sizeof(defaultValue) / sizeof(char)));
    }
    return propValue;
  }
  errno_enum_type res = qcril_config_get(id1, &propValue);
  if (res == E_SUCCESS && propValue != NULL)
  {
    QCRIL_LOG_DEBUG("Property %s can be read", id);
  }
  else
  {
    QCRIL_LOG_DEBUG("Property %s could not be read",id);
    propValue = (char*) (calloc ((sizeof(defaultValue) / sizeof(char)), sizeof(char)));
    if (propValue != NULL)
    {
      strlcpy(propValue, defaultValue, (sizeof(defaultValue) / sizeof(char)));
    }
  }
  return propValue;
}
#else
char* readPropertyLA(char id[], char* propValue, char defaultValue[])
{
  propValue = (char*) (calloc (PROPERTY_VALUE_MAX, sizeof(char)));
  if(propValue != NULL)
  {
    property_get(id, propValue, defaultValue);
    QCRIL_LOG_DEBUG("Property %s is %s", id, propValue);
  }
  return propValue;
}
#endif

char* readProperty(char id[], char* propValue, char defaultValue[])
{
#ifdef RIL_FOR_MDM_LE
  return readPropertyLE(id, propValue, defaultValue);
#else
  return readPropertyLA(id, propValue, defaultValue);
#endif
}

}
