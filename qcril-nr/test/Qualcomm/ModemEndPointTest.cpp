/******************************************************************************
#  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/DmsModemEndPoint.h"
#include "modules/qmi/PdcModemEndPoint.h"

TEST(BasicFeatureSanity, TestEqualityOfModemEndPoints) {
    std::shared_ptr<DmsModemEndPoint> endPoint1 =
        ModemEndPointFactory<DmsModemEndPoint>::getInstance().buildEndPoint();

    std::shared_ptr<DmsModemEndPoint> endPoint2 =
        ModemEndPointFactory<DmsModemEndPoint>::getInstance().buildEndPoint();

    EXPECT_EQ(endPoint1, endPoint2);
}

/* ToDo: update the test case with actual async requestSetup procedure.
 */

/* ToDo: update the test case with actual async requestSetup procedure.
 */

TEST(BasicFeatureSanity, TestUniquenessOfModemEndPoints) {
    std::shared_ptr<VoiceModemEndPoint> voiceModemPoint1 =
        ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();

    int initial_count = voiceModemPoint1.use_count();

    std::shared_ptr<VoiceModemEndPoint> voiceModemPoint2 =
        ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();
    /* Verify that local references to ModemEndPoint correctly increments and
     * refcount*/
    ASSERT_EQ(voiceModemPoint2.use_count(), initial_count+1);
}

TEST(FrameworkUnitTest, VerifyThatModemEndPointRefCoutingIsCorrect) {
    class SampleModemEndPoint : public ModemEndPoint {
    public:
        SampleModemEndPoint() :ModemEndPoint("Sample") {
        }
        ~SampleModemEndPoint() {
        }

    };

    std::shared_ptr<SampleModemEndPoint> sampleEndPoint =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint == nullptr);

    ASSERT_EQ(2, sampleEndPoint.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint.use_count());

    sampleEndPoint = nullptr;
    ASSERT_EQ(1, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

    ModemEndPointFactory<SampleModemEndPoint>::getInstance().reset();
    ASSERT_EQ(0, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

}

TEST(FrameworkUnitTest, VerifyThatModemEndPointAreUnique) {
    class SampleModemEndPoint : public ModemEndPoint {
    public:
        SampleModemEndPoint() :ModemEndPoint("Sample") {
        }
        ~SampleModemEndPoint() {
        }

    };

    std::shared_ptr<SampleModemEndPoint> sampleEndPoint1 =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint1 == nullptr);

    ASSERT_EQ(2, sampleEndPoint1.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint1.use_count());

    /* A new instance/reference should result in ref-count increment*/
    std::shared_ptr<SampleModemEndPoint> sampleEndPoint2 =
        ModemEndPointFactory<SampleModemEndPoint>::getInstance().buildEndPoint();
    ASSERT_FALSE(sampleEndPoint2 == nullptr);
    ASSERT_EQ(3, sampleEndPoint2.use_count());
    ASSERT_EQ(sampleEndPoint1.use_count(), sampleEndPoint2.use_count());
    ASSERT_EQ(ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount(), sampleEndPoint2.use_count());


    /* Removal of existing refrences should result in decrement of ref-count*/
    sampleEndPoint1 = nullptr;
    ASSERT_EQ(2, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());

    sampleEndPoint2 = nullptr;
    ASSERT_EQ(1, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());


    ModemEndPointFactory<SampleModemEndPoint>::getInstance().reset();
    ASSERT_EQ(0, ModemEndPointFactory<SampleModemEndPoint>::getInstance().getRefCount());
}

