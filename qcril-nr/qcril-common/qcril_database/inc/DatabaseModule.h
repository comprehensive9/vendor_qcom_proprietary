/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "DatabaseUpdateNotifyMessage.h"

class DatabaseModule : public Module {
  public:
    static void createDataBaseModule(int id);
    static DatabaseModule& getInstance();
    DatabaseModule();
    ~DatabaseModule();
    void setDatabaseInstanceId(int id);
    void init();

  private:
    int mId;
    void handleDatabaseUpdateNotification(std::shared_ptr<DatabaseUpdateNotifyMessage> msg);
};

DatabaseModule& getDatabaseModule();
