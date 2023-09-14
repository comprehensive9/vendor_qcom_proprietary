#define LOG_TAG "vendor.qti.hardware.radio.lpa@1.2::UimLpaResponse"

#include <log/log.h>
#include <cutils/trace.h>
#include <utils/Trace.h>
#include <vendor/qti/hardware/radio/lpa/1.2/IUimLpaResponse.h>
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_2 {

const char* IUimLpaResponse::descriptor("vendor.qti.hardware.radio.lpa@1.2::IUimLpaResponse");


// Methods from ::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaResponse follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpaResponse::UimLpaUserResponse(int32_t token, const ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaUserResp& response)
// no default implementation for: ::android::hardware::Return<void> IUimLpaResponse::UimLpaHttpTxnCompletedResponse(int32_t token, ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaResult result)

// Methods from ::vendor::qti::hardware::radio::lpa::V1_2::IUimLpaResponse follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpaResponse::UimLpaUserResponse_1_2(int32_t token, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& response)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IUimLpaResponse::interfaceChain(interfaceChain_cb _hidl_cb){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){115,81,132,209,150,147,180,194,61,78,159,211,121,170,52,169,240,217,63,59,133,132,19,94,96,130,187,227,169,136,19,70} /* 735184d19693b4c23d4e9fd379aa34a9f0d93f3b8584135e6082bbe3a9881346 */,
        (uint8_t[32]){182,124,122,27,76,238,90,59,238,125,158,244,2,53,76,163,245,92,106,77,168,33,154,51,211,91,213,41,63,132,145,204} /* b67c7a1b4cee5a3bee7d9ef402354ca3f55c6a4da8219a33d35bd5293f8491cc */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpaResponse::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IUimLpaResponse::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = -1;
    info.ptr = 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpaResponse::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpaResponse::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpaResponse>> IUimLpaResponse::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpaResponse>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpaResponse>> IUimLpaResponse::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaResponse>& parent, bool emitError) {
    return nullptr;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpaResponse>> IUimLpaResponse::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return nullptr;
}


::android::sp<IUimLpaResponse> IUimLpaResponse::tryGetService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::sp<IUimLpaResponse> IUimLpaResponse::getService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::status_t IUimLpaResponse::registerAsService(const std::string &serviceName) {
    return ::android::OK;
}

bool IUimLpaResponse::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
        return true;
    }
static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_2
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
