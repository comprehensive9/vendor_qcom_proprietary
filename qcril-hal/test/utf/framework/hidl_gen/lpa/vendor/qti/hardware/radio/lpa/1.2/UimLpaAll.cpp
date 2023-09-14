#define LOG_TAG "vendor.qti.hardware.radio.lpa@1.2::UimLpa"

#include <log/log.h>
#include <cutils/trace.h>
#include <vendor/qti/hardware/radio/lpa/1.2/IUimLpa.h>
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_2 {

const char* IUimLpa::descriptor("vendor.qti.hardware.radio.lpa@1.2::IUimLpa");

// Methods from ::vendor::qti::hardware::radio::lpa::V1_0::IUimLpa follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpa::setCallback(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaResponse>& responseCallback, const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_0::IUimLpaIndication>& indicationCallback)
// no default implementation for: ::android::hardware::Return<void> IUimLpa::UimLpaUserRequest(int32_t token, const ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaUserReq& userReq)
// no default implementation for: ::android::hardware::Return<void> IUimLpa::UimLpaHttpTxnCompletedRequest(int32_t token, ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaResult result, const ::android::hardware::hidl_vec<uint8_t>& responsePayload, const ::android::hardware::hidl_vec<::vendor::qti::hardware::radio::lpa::V1_0::UimLpaHttpCustomHeader>& customHeaders)

// Methods from ::vendor::qti::hardware::radio::lpa::V1_1::IUimLpa follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpa::UimLpaUserRequest_1_1(int32_t token, const ::vendor::qti::hardware::radio::lpa::V1_1::UimLpaUserReq& userReq)

// Methods from ::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa follow.
// no default implementation for: ::android::hardware::Return<void> IUimLpa::dummy()

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IUimLpa::interfaceChain(interfaceChain_cb _hidl_cb){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpa::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpa::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpa::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){117,222,117,117,109,105,146,143,112,216,76,180,167,137,247,39,204,75,219,164,95,11,227,207,110,133,201,132,40,54,236,100} /* 75de75756d69928f70d84cb4a789f727cc4bdba45f0be3cf6e85c9842836ec64 */,
        (uint8_t[32]){163,132,55,56,7,189,128,94,115,171,85,229,208,149,222,251,243,217,87,176,148,193,247,232,71,216,132,173,230,52,221,73} /* a384373807bd805e73ab55e5d095defbf3d957b094c1f7e847d884ade634dd49 */,
        (uint8_t[32]){76,225,73,109,203,109,170,211,182,21,150,93,149,179,251,44,77,226,24,219,55,201,77,172,185,195,122,91,38,41,24,44} /* 4ce1496dcb6daad3b615965d95b3fb2c4de218db37c94dacb9c37a5b2629182c */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpa::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpa::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IUimLpa::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IUimLpa::getDebugInfo(getDebugInfo_cb _hidl_cb){
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

::android::hardware::Return<void> IUimLpa::notifySyspropsChanged(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IUimLpa::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa>> IUimLpa::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa>> IUimLpa::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_1::IUimLpa>& parent, bool emitError) {
    return nullptr;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa>> IUimLpa::castFrom(const ::android::sp<::vendor::qti::hardware::radio::lpa::V1_0::IUimLpa>& parent, bool emitError) {
    return nullptr;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::radio::lpa::V1_2::IUimLpa>> IUimLpa::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return nullptr;
}

::android::sp<IUimLpa> IUimLpa::tryGetService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::sp<IUimLpa> IUimLpa::getService(const std::string &serviceName, const bool getStub) {
    return nullptr;
}

::android::status_t IUimLpa::registerAsService(const std::string &serviceName) {
    return ::android::OK;
}

bool IUimLpa::registerForNotifications(
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
