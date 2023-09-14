/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "DatabaseModule.h"
#include "qcril_db.h"
#include "DbState.h"


DatabaseModule& DatabaseModule::getInstance() {
    static DatabaseModule sInstance;
    return sInstance;
}

void DatabaseModule::createDataBaseModule(int id)
{
    getInstance().setDatabaseInstanceId(id);
    getInstance().init();
}


/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
DatabaseModule::DatabaseModule() {
  mName = "DatabaseModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(DatabaseUpdateNotifyMessage, DatabaseModule::handleDatabaseUpdateNotification),
  };
}

/* Follow RAII.
*/
DatabaseModule::~DatabaseModule() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void DatabaseModule::init() {
  Module::init();

  (void)DbState::createDbState(mId == 0);
  qcril_db_init(mId);
}

void DatabaseModule::setDatabaseInstanceId(int id) {
  mId = id;
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void DatabaseModule::handleDatabaseUpdateNotification(std::shared_ptr<DatabaseUpdateNotifyMessage> msg) {

    Log::getInstance().d("[DatabaseModule]: Handling msg = " + msg->dump());
}
