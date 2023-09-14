/*==========================================================================

# Copyright (c) 2020 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <unistd.h>
#include <utils/Log.h>
#include <sys/ioctl.h>
#include <cutils/sockets.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <string.h>
#define LOG_TAG "bluetooth_transport_properties"

static const int get_soc_id_cmd = 0xbfaf;

void setVendorPropertiesDefault() {
  int ret = 0;
  char soc_name[PROPERTY_VALUE_MAX];
  char compatable_chipset_id[32] = {'\0'};
  int  fd_btdev;
  ret = property_get("persist.vendor.qcom.bluetooth.soc", soc_name, "");
  if (ret == 0) {
    fd_btdev = open("/dev/btpower", O_RDWR | O_NONBLOCK);
    if (fd_btdev < 0) {
      ALOGE("\nfailed to open bt device error = (%s)\n", strerror(errno));
      return;
    }
    ret = ioctl(fd_btdev, get_soc_id_cmd, compatable_chipset_id);
    close(fd_btdev);
    if (ret < 0) {
      ALOGE("%s: ioctl failed error = %d\n",__func__, ret);
      return;
    } else {
      ALOGD("%s:Soc version recevied : %s\n", __func__, compatable_chipset_id);
    }
    if (strstr(compatable_chipset_id, "qca6490")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastings");
    } else if (strstr(compatable_chipset_id, "qca6390")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "hastings");
    } else if (strstr(compatable_chipset_id, "wcn3990")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "cherokee");
    } else if (strstr(compatable_chipset_id, "wcn6750")) {
      ret = property_set("persist.vendor.qcom.bluetooth.soc", "moselle");
    }

    if (ret < 0) {
      ALOGE("%s: set soc name property failed error = %d\n",__func__, ret);
    }
  } else {
    ALOGD("%s:vendor properties set : soc name  : %s\n", __func__, soc_name);
  }
  return;
}
