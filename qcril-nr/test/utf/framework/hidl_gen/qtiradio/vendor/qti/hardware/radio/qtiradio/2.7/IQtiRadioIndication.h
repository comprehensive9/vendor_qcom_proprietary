/*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_IQTIRADIOINDICATION_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_IQTIRADIOINDICATION_H

#include <vendor/qti/hardware/radio/qtiradio/2.5/IQtiRadioIndication.h>
#include <vendor/qti/hardware/radio/qtiradio/2.7/types.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V2_7 {

/*
 * Interface declaring unsolicited qtiradio indications
 */
struct IQtiRadioIndication : public ::vendor::qti::hardware::radio::qtiradio::V2_5::IQtiRadioIndication {
    /**
     * Type tag for use in template logic that indicates this is a 'pure' class.
     */
    typedef ::android::hardware::details::i_tag _hidl_tag;

    /**
     * Fully qualified interface name: "vendor.qti.hardware.radio.qtiradio@2.7::IQtiRadioIndication"
     */
    static const char* descriptor;

    /**
     * Returns whether this object's implementation is outside of the current process.
     */
    virtual bool isRemote() const override { return false; }

    /*
     * Send qtiradio indication
     *
     * @param data returned as raw bytes
     *
     */
    virtual ::android::hardware::Return<void> qtiRadioIndication(::vendor::qti::hardware::radio::qtiradio::V1_0::QtiRadioIndicationType type) = 0;

    /*
     * Unsol msg to indicate change in 5g status in NSA and SA mode.
     *
     * @param enableStatus as per types.hal to indicate enabled/disabled state.
     *
     */
    virtual ::android::hardware::Return<void> on5gStatusChange(::vendor::qti::hardware::radio::qtiradio::V2_0::EnableStatus status) = 0;

    /*
     * Unsol msg to indicate changes in EutraNewRadioDualConnectivity(EN-DC)
     * and RestrictDualConnectivityWithNewRadio(restrictDCNR) for current LTE cell.
     *
     * @param DcParam as per types.hal to indicate various states of EN-DC and restrictDCNR.
     *
     */
    virtual ::android::hardware::Return<void> onNrDcParamChange(const ::vendor::qti::hardware::radio::qtiradio::V2_0::DcParam& dcParam) = 0;

    /*
     * Unsol msg to indicate changes in 5G SCG configuration.
     *
     * @param BearerStatus as per types.hal to indicate bearer allocated/removed.
     *
     */
    virtual ::android::hardware::Return<void> onNrBearerAllocationChange(::vendor::qti::hardware::radio::qtiradio::V2_0::BearerStatus bearerStatus) = 0;

    /*
     * Unsol msg to indicate changes in 5G signal strength.
     *
     * @param SignalStrength as per types.hal to indicate 5g signal strength.
     *
     */
    virtual ::android::hardware::Return<void> onSignalStrengthChange(const ::vendor::qti::hardware::radio::qtiradio::V2_0::SignalStrength& signalStrength) = 0;

    /*
     * Unsol msg to notify of upper layer indication.
     *
     * @param upperLayerIndInfo upper layer indication received from the modem.
     */
    virtual ::android::hardware::Return<void> onUpperLayerIndInfoChange(const ::vendor::qti::hardware::radio::qtiradio::V2_1::UpperLayerIndInfo& uliInfo) = 0;

    /*
     * Unsol msg to indicate changes in 5G SCG configuration.
     *
     * @param BearerStatus as per types.hal to indicate bearer allocated/removed.
     *
     */
    virtual ::android::hardware::Return<void> onNrBearerAllocationChange_2_1(::vendor::qti::hardware::radio::qtiradio::V2_1::BearerStatus bearerStatus) = 0;

    /*
     * Unsol msg to indicate changes in 5G configuration.
     *
     * @param 5gConfigType as per types.hal to indicate configuration NSA/SA.
     *
     */
    virtual ::android::hardware::Return<void> on5gConfigInfoChange(::vendor::qti::hardware::radio::qtiradio::V2_1::ConfigType config) = 0;

    /*
     * Unsol msg to indicate changes in 5G Icon Type.
     *
     * @param NrIconType as per types.hal to indicate 5G icon - NONE(Non-5G) or
     *         5G BASIC or 5G UWB shown on the UI.
     *
     */
    virtual ::android::hardware::Return<void> onNrIconTypeChange(::vendor::qti::hardware::radio::qtiradio::V2_2::NrIconType iconType) = 0;

    /*
     * Unsol msg to indicate change in NR Config.
     *
     * @param NrConfig as per types.hal to indicate NSA/SA/NSA+SA.
     *
     */
    virtual ::android::hardware::Return<void> onNrConfigChange(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig config) = 0;

    /*
     * Unsol message to indicate changes in QoS parameters for a given cid.
     * This should be received from vendor RIL only for data calls with IMS as an APN type.
     *
     * @param cid Connection id of the data call for which QoS parameters are received.
     * @param qosParamsResult QosParametersResult defined in types.hal
     *
     */
    virtual ::android::hardware::Return<void> onQosParametersChanged(int32_t cid, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& qosParamsResult) = 0;

    /**
     * Return callback for interfaceChain
     */
    using interfaceChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& descriptors)>;
    /*
     * Provides run-time type information for this object.
     * For example, for the following interface definition:
     *     package android.hardware.foo@1.0;
     *     interface IParent {};
     *     interface IChild extends IParent {};
     * Calling interfaceChain on an IChild object must yield the following:
     *     ["android.hardware.foo@1.0::IChild",
     *      "android.hardware.foo@1.0::IParent"
     *      "android.hidl.base@1.0::IBase"]
     *
     * @return descriptors a vector of descriptors of the run-time type of the
     *         object.
     */
    virtual ::android::hardware::Return<void> interfaceChain(interfaceChain_cb _hidl_cb) override;

    /*
     * Emit diagnostic information to the given file.
     *
     * Optionally overriden.
     *
     * @param fd      File descriptor to dump data to.
     *                Must only be used for the duration of this call.
     * @param options Arguments for debugging.
     *                Must support empty for default debug information.
     */
    virtual ::android::hardware::Return<void> debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options) override;

    /**
     * Return callback for interfaceDescriptor
     */
    using interfaceDescriptor_cb = std::function<void(const ::android::hardware::hidl_string& descriptor)>;
    /*
     * Provides run-time type information for this object.
     * For example, for the following interface definition:
     *     package android.hardware.foo@1.0;
     *     interface IParent {};
     *     interface IChild extends IParent {};
     * Calling interfaceDescriptor on an IChild object must yield
     *     "android.hardware.foo@1.0::IChild"
     *
     * @return descriptor a descriptor of the run-time type of the
     *         object (the first element of the vector returned by
     *         interfaceChain())
     */
    virtual ::android::hardware::Return<void> interfaceDescriptor(interfaceDescriptor_cb _hidl_cb) override;

    /**
     * Return callback for getHashChain
     */
    using getHashChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_array<uint8_t, 32>>& hashchain)>;
    /*
     * Returns hashes of the source HAL files that define the interfaces of the
     * runtime type information on the object.
     * For example, for the following interface definition:
     *     package android.hardware.foo@1.0;
     *     interface IParent {};
     *     interface IChild extends IParent {};
     * Calling interfaceChain on an IChild object must yield the following:
     *     [(hash of IChild.hal),
     *      (hash of IParent.hal)
     *      (hash of IBase.hal)].
     *
     * SHA-256 is used as the hashing algorithm. Each hash has 32 bytes
     * according to SHA-256 standard.
     *
     * @return hashchain a vector of SHA-1 digests
     */
    virtual ::android::hardware::Return<void> getHashChain(getHashChain_cb _hidl_cb) override;

    /*
     * This method trigger the interface to enable/disable instrumentation based
     * on system property hal.instrumentation.enable.
     */
    virtual ::android::hardware::Return<void> setHALInstrumentation() override;

    /*
     * Registers a death recipient, to be called when the process hosting this
     * interface dies.
     *
     * @param recipient a hidl_death_recipient callback object
     * @param cookie a cookie that must be returned with the callback
     * @return success whether the death recipient was registered successfully.
     */
    virtual ::android::hardware::Return<bool> linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie) override;

    /*
     * Provides way to determine if interface is running without requesting
     * any functionality.
     */
    virtual ::android::hardware::Return<void> ping() override;

    /**
     * Return callback for getDebugInfo
     */
    using getDebugInfo_cb = std::function<void(const ::android::hidl::base::V1_0::DebugInfo& info)>;
    /*
     * Get debug information on references on this interface.
     * @return info debugging information. See comments of DebugInfo.
     */
    virtual ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb) override;

    /*
     * This method notifies the interface that one or more system properties
     * have changed. The default implementation calls
     * (C++)  report_sysprop_change() in libcutils or
     * (Java) android.os.SystemProperties.reportSyspropChanged,
     * which in turn calls a set of registered callbacks (eg to update trace
     * tags).
     */
    virtual ::android::hardware::Return<void> notifySyspropsChanged() override;

    /*
     * Unregisters the registered death recipient. If this service was registered
     * multiple times with the same exact death recipient, this unlinks the most
     * recently registered one.
     *
     * @param recipient a previously registered hidl_death_recipient callback
     * @return success whether the death recipient was unregistered successfully.
     */
    virtual ::android::hardware::Return<bool> unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient) override;

    // cast static functions
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_5::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_2::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_1::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_0::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V1_0::IQtiRadioIndication>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    // helper methods for interactions with the hwservicemanager
    /**
     * This gets the service of this type with the specified instance name. If the
     * service is currently not available or not in the VINTF manifest on a Trebilized
     * device, this will return nullptr. This is useful when you don't want to block
     * during device boot. If getStub is true, this will try to return an unwrapped
     * passthrough implementation in the same process. This is useful when getting an
     * implementation from the same partition/compilation group.
     *
     * In general, prefer getService(std::string,bool)
     */
    static ::android::sp<IQtiRadioIndication> tryGetService(const std::string &serviceName="default", bool getStub=false);
    /**
     * Deprecated. See tryGetService(std::string, bool)
     */
    static ::android::sp<IQtiRadioIndication> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    /**
     * Deprecated. See tryGetService(std::string, bool)
     */
    static ::android::sp<IQtiRadioIndication> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    /**
     * Calls tryGetService("default", bool). This is the recommended instance name for singleton services.
     */
    static ::android::sp<IQtiRadioIndication> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    /**
     * This gets the service of this type with the specified instance name. If the
     * service is not in the VINTF manifest on a Trebilized device, this will return
     * nullptr. If the service is not available, this will wait for the service to
     * become available. If the service is a lazy service, this will start the service
     * and return when it becomes available. If getStub is true, this will try to
     * return an unwrapped passthrough implementation in the same process. This is
     * useful when getting an implementation from the same partition/compilation group.
     */
    static ::android::sp<IQtiRadioIndication> getService(const std::string &serviceName="default", bool getStub=false);
    /**
     * Deprecated. See getService(std::string, bool)
     */
    static ::android::sp<IQtiRadioIndication> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    /**
     * Deprecated. See getService(std::string, bool)
     */
    static ::android::sp<IQtiRadioIndication> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    /**
     * Calls getService("default", bool). This is the recommended instance name for singleton services.
     */
    static ::android::sp<IQtiRadioIndication> getService(bool getStub) { return getService("default", getStub); }
    /**
     * Registers a service with the service manager. For Trebilized devices, the service
     * must also be in the VINTF manifest.
     */
    __attribute__ ((warn_unused_result))::android::status_t registerAsService(const std::string &serviceName="default");
    /**
     * Registers for notifications for when a service is registered.
     */
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

//
// type declarations for package
//

static inline std::string toString(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>& o);

//
// type header definitions for package
//

static inline std::string toString(const ::android::sp<::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication>& o) {
    std::string os = "[class or subclass of ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::IQtiRadioIndication::descriptor;
    os += "]";
    os += o->isRemote() ? "@remote" : "@local";
    return os;
}


}  // namespace V2_7
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

//
// global type declarations for package
//


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_IQTIRADIOINDICATION_H
