/**
 * Copyright (c)2012-2013, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import java.io.Serializable;

public class Contact {
    String name;
    String phone;
    CapabilityInfo capabilities;
    boolean isSubscribed = false;

    public Contact(String name, String phone, int availability, String status,
                   String individualContactURI) {
        this.name = name;
        this.phone = phone;
    }

    public void setCapabilities(CapabilityInfo caps) {
        capabilities = caps;
    }
    public void setSubscribed(boolean x) {
        isSubscribed = x;
    }
    public boolean isSubscribed() {
        return isSubscribed;
    }
    public CapabilityInfo getCapabilities() {
        return capabilities;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getUriString() {
        return "tel:" + phone;
    }

    @Override
    public String toString() {
        return "Contact [name=" + name + ", phone=" + phone
                +  "]";
    }


}
