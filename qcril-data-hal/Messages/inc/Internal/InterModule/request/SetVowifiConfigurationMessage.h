/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

class SetVowifiConfigurationMessage : public SolicitedMessage<RIL_Errno>,
                                      public add_message_id<SetVowifiConfigurationMessage> {

private:
    std::optional<std::string> fqdn;
    std::optional<std::string> v4Address;
    std::optional<std::string> v6Address;

public:
    static constexpr const char *MESSAGE_NAME = "SetVowifiConfigurationMessage";
    SetVowifiConfigurationMessage():SolicitedMessage<RIL_Errno>(get_class_message_id()) {
      mName = MESSAGE_NAME;
    }

    ~SetVowifiConfigurationMessage() {
    }

    bool hasFqdn() { return fqdn ? true : false; }
    std::string getFqdn() { return *fqdn; }
    void setFqdn(const std::string& setFqdn) { fqdn = setFqdn; }

    bool hasV4Address() { return v4Address ? true : false; }
    std::string getV4Address() { return *v4Address; }
    void setV4Address(const std::string& setV4Address) { v4Address = setV4Address; }

    bool hasV6Address() { return v6Address ? true : false; }
    std::string getV6Address() { return *v6Address; }
    void setV6Address(const std::string& setV6Address) { v6Address = setV6Address; }

    string dump() {
        return mName + "{" +
            (hasFqdn() ? getFqdn() : "") + "," +
            (hasV4Address() ? getV4Address() : "") + "," +
            (hasV6Address() ? getV6Address() : "") + "}";
    }
};
}
