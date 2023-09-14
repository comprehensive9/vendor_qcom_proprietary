/******************************************************************************
#  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef DATAINITMESSAGE
#define DATAINITMESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

/**
  * DataInitMessage: Sent by the DATA module.
  * Indicates that DATA module is ready to receive requests.
  *
  */

class DataInitMessage: public UnSolicitedMessage, public add_message_id<DataInitMessage>
{
    private:
      int mInstanceId;

    public:
      static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_INIT";

    public:

      DataInitMessage(int instance_id);
      ~DataInitMessage();

      std::shared_ptr<UnSolicitedMessage> clone();
      string dump();
      int getInstance();
};

}

#endif