/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "lce_hidl_test"

#include <log/log.h>
#include <vendor/qti/data/factory/2.3/IFactory.h>
#include <vendor/qti/hardware/data/lce/1.0/ILceIndication.h>
#include <vendor/qti/hardware/data/lce/1.0/ILceService.h>
#include <vendor/qti/hardware/data/lce/1.0/IToken.h>
#include <VtsHalHidlTargetCallbackBase.h>
#include <VtsHalHidlTargetTestBase.h>

using android::hardware::Return;
using android::hardware::Void;
using android::sp;

using vendor::qti::data::factory::V2_3::IFactory;
using vendor::qti::hardware::data::lce::V1_0::AccessNetwork;
using vendor::qti::hardware::data::lce::V1_0::Action;
using vendor::qti::hardware::data::lce::V1_0::ILceIndication;
using vendor::qti::hardware::data::lce::V1_0::ILceService;
using vendor::qti::hardware::data::lce::V1_0::IToken;
using vendor::qti::hardware::data::lce::V1_0::LinkDirection;
using vendor::qti::hardware::data::lce::V1_0::Report;
using vendor::qti::hardware::data::lce::V1_0::ReportType;
using vendor::qti::hardware::data::lce::V1_0::StatusCode;

class LceHidlTestBase : public ::testing::VtsHalHidlTargetTestBase {
public:
    virtual void SetUp() override {
        setUpFactory();
    }

    void setUpFactory() {
        iFactory = ::testing::VtsHalHidlTargetTestBase::getService<IFactory>();
        ASSERT_NE(nullptr, iFactory.get()) << "Could not get iFactory HIDL instance";
    }

    sp<ILceService> requestService(const ::vendor::qti::data::factory::V2_3::IFactory::StatusCode
                                                  expected_result) {
        sp<ILceService> LceService;

        auto cb = [&](::vendor::qti::data::factory::V2_3::IFactory::StatusCode status,
                      sp<ILceService> service) {
            ASSERT_EQ(expected_result, status);

            LceService = service;
            ASSERT_NE(nullptr, LceService.get()) << "Could not get HIDL instance";
        };

        sp<IToken> token = new IToken();
        const Return<void> ret = iFactory->createILceService(token, cb);
        return LceService;
    }

    sp<IFactory> iFactory;
};

class GetLceIndicationArgs {
public:
    GetLceIndicationArgs(Report report) : accessNetwork(report.accessNetwork), rate(report.rate),
        level(report.level), dir(report.dir), queueSize(report.queueSize), type(report.type) {}

private:
    AccessNetwork accessNetwork;
    uint32_t rate;
    uint32_t level;
    LinkDirection dir;
    uint32_t queueSize;
    ReportType type;
};

class MockGetLceIndication : public ILceIndication,
    public ::testing::VtsHalHidlTargetCallbackBase<GetLceIndicationArgs> {

public:
    MockGetLceIndication() {};
    virtual ~MockGetLceIndication() {};

    Return<void> linkEstimation(const Report& report) override {
        ALOGI("GetLceIndication arrived");

        GetLceIndicationArgs args(report);
        NotifyFromCallback("onResult", args);
        return Void();
    };
};

// Test that reporting period is correct.
/* Note that this test needs background data, otherwise it will timeout after 1 minute */
TEST_F(LceHidlTestBase, TestSetReportingPeriod) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    vector<uint32_t> dl = {};
    vector<uint32_t> ul = {};
    service->setLinkCapacityReportingCriteria(1000, 0, dl, ul, AccessNetwork::NONE);

    // Expect UL and DL CB after 1 second.
    cb->WaitForCallback("onResult");
    cb->WaitForCallback("onResult");
}

// Test that reporting threshold is correct.
TEST_F(LceHidlTestBase, TestSetReportingThreshold) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    vector<uint32_t> dl = {100};
    vector<uint32_t> ul = {100};
    res = service->setLinkCapacityReportingCriteria(0, 0, dl, ul, AccessNetwork::LTE);
    ASSERT_EQ(res, StatusCode::OK);

    // Threshold based reporting indications can not be tested without knowing data rates on device
    // cb->WaitForCallback("onResult");
    // cb->WaitForCallback("onResult");
}

// Test that errors are correctly returned for invalid args.
TEST_F(LceHidlTestBase, TestSetReportingInvalidArgs) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    vector<uint32_t> dl = {100};
    vector<uint32_t> ul = {100};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);

    dl = {100};
    ul = {};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);

    dl = {};
    ul = {100};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);

    dl = {};
    ul = {};
    res = service->setLinkCapacityReportingCriteria(100, 100, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);

    dl = {};
    ul = {};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::LTE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);

    dl = {100};
    ul = {100};
    res = service->setLinkCapacityReportingCriteria(100, 100, dl, ul, AccessNetwork::LTE);
    ASSERT_EQ(res, StatusCode::INVALID_ARGUMENTS);
}

// Test that error are correctly returned when periodic reporting is set after threshold is already active.
TEST_F(LceHidlTestBase, TestSetReportingThresholdToPeriodicError) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    vector<uint32_t> dl = {100};
    vector<uint32_t> ul = {100};
    res = service->setLinkCapacityReportingCriteria(0, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::OK);

    dl = {};
    ul = {};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::REPORTING_CRITERIA_TYPE_MISMATCH);

    /* Threshold based reporting should also be set if all params are 0 or empty */
    dl = {};
    ul = {};
    res = service->setLinkCapacityReportingCriteria(0, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::OK);
}

// Test that error are correctly returned when periodic reporting is set after threshold is already active.
TEST_F(LceHidlTestBase, TestSetReportingPeriodicToThresholdError) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    vector<uint32_t> dl = {};
    vector<uint32_t> ul = {};
    res = service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::OK);

    dl = {100};
    ul = {100};
    res = service->setLinkCapacityReportingCriteria(0, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::REPORTING_CRITERIA_TYPE_MISMATCH);
}

// Test get last estimation report.
TEST_F(LceHidlTestBase, TestGetEstimationReport) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    res = service->getLastEstimationReport();
    // Should fail since setLinkCapacityReportingCriteria() has not been set
    ASSERT_EQ(res, StatusCode::REPORTING_CRITERIA_NOT_SET);

    vector<uint32_t> dl = {};
    vector<uint32_t> ul = {};
    res = service->setLinkCapacityReportingCriteria(10000, 0, dl, ul, AccessNetwork::NONE);
    ASSERT_EQ(res, StatusCode::OK);

    res = service->getLastEstimationReport();
    ASSERT_EQ(res, StatusCode::OK);

    // Expect callback from getLastEstimationReport before the 10 second period set in
    // setLinkCapacityReportingCriteria.
    // TODO: add check that callback is less than 10 seconds
    cb->WaitForCallback("onResult");
}

// Test that Stop and Start work for periodic reporting.
TEST_F(LceHidlTestBase, TestReportingPeriodicAction) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    /* Test Stop/Start for Periodic Reporting */
    vector<uint32_t> dl = {};
    vector<uint32_t> ul = {};
    service->setLinkCapacityReportingCriteria(100, 0, dl, ul, AccessNetwork::NONE);
    cb->WaitForCallback("onResult");

    service->performReportingAction(Action::STOP);
    // Even though stop is called client can still get last report.
    res = service->getLastEstimationReport();
    ASSERT_EQ(res, StatusCode::OK);
    service->performReportingAction(Action::START);
    res = service->getLastEstimationReport();
    ASSERT_EQ(res, StatusCode::OK);
}

// Test that Stop and Start work for threshold reporting.
TEST_F(LceHidlTestBase, TestReportingThresholdAction) {
    StatusCode res;
    sp<ILceService> service =
        requestService(::vendor::qti::data::factory::V2_3::IFactory::StatusCode::OK);

    MockGetLceIndication *cb = new MockGetLceIndication();
    service->setCallback(cb);

    /* Test Stop/Start for Threshold Reporting */
    vector<uint32_t> dl = {100};
    vector<uint32_t> ul = {100};
    service->setLinkCapacityReportingCriteria(0, 0, dl, ul, AccessNetwork::LTE);

    // Don't wait for callback here unless we know thresholds on device, otherwise testcase
    // takes longer then 1 min to timeout.
    //cb->WaitForCallback("onResult");

    service->performReportingAction(Action::STOP);
    // Even though stop is called client can still get last report.
    res = service->getLastEstimationReport();
    ASSERT_EQ(res, StatusCode::OK);
    service->performReportingAction(Action::START);
    res = service->getLastEstimationReport();
    ASSERT_EQ(res, StatusCode::OK);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();
    ALOGE("Test result with status=%d", status);
    return status;
}
