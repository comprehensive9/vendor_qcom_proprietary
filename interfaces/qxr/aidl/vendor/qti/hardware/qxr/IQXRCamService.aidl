//
// Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//

package vendor.qti.hardware.qxr;

@VintfStability

interface IQXRCamService {

    int setFd(in ParcelFileDescriptor fd, in int prot);

}

