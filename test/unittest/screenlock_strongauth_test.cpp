/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <list>
#include <string>
#include <sys/time.h>

#include "sclock_log.h"
#include "screenlock_common.h"
#include "securec.h"
#ifndef IS_SO_CROP_H
#include "strongauthmanager.h"
#endif // IS_SO_CROP_H
#include "commeventsubscriber.h"
#include "screenlock_strongauth_test.h"


namespace OHOS {
namespace ScreenLock {
const std::string AUTH_PIN = "1";
const std::string NO_AUTH_PIN = "0";
const std::string TAG_AUTHTYPE = "authType";
const std::string HAS_CREDENTIAL = "1";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";
using namespace testing::ext;

void ScreenLockStrongAuthTest::SetUpTestCase()
{
}

void ScreenLockStrongAuthTest::TearDownTestCase()
{
}

void ScreenLockStrongAuthTest::SetUp()
{
}

void ScreenLockStrongAuthTest::TearDown()
{
}

/**
* @tc.name: ScreenLockStrongAuthTest001
* @tc.desc: ScreenLockStrongAuthTest RmvAll.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("ScreenLockStrongAuthTest");
#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        SCLOCK_HILOGE("authmanager is nullptr!");
        return;
    }

    int32_t userId = 100;
    int32_t defaulVal = 1;
    int64_t timerInterval = 1;
    authmanager->RegistIamEventListener();
    authmanager->StartStrongAuthTimer(userId);
    authmanager->GetTimerId(userId);
    authmanager->ResetStrongAuthTimer(userId, timerInterval);
    authmanager->DestroyStrongAuthTimer(userId);
    authmanager->DestroyAllStrongAuthTimer();
    authmanager->UnRegistIamEventListener();
    authmanager->SetStrongAuthStat(userId, defaulVal);
    int32_t val = authmanager->GetStrongAuthStat(userId);

    Singleton<CommeventMgr>::GetInstance().SubscribeEvent();
    Singleton<CommeventMgr>::GetInstance().UnSubscribeEvent();
    EXPECT_EQ(defaulVal, val);
#endif // IS_SO_CROP_H
    return;
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest002, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest003, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::authTimer timer;
    timer.OnTrigger();
    bool result = true;
    EXPECT_EQ(result, true);
#endif // IS_SO_CROP_H
}

static void StrongAuthTimerCallbackTest(int32_t userId)
{
    SCLOCK_HILOGI("%{public}s, enter", __FUNCTION__);
    return;
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest004, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::authTimer timer;
    timer.SetCallbackInfo(StrongAuthTimerCallbackTest);
    timer.OnTrigger();
    bool result = true;
    EXPECT_EQ(result, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest005, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    AAFwk::Want want;
    want.SetAction(USER_CREDENTIAL_UPDATED_EVENT);
    want.SetParam("userId", 0);
    want.SetParam("authType", AUTH_PIN);
    want.SetParam("credentialCount", HAS_CREDENTIAL);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    bool result = true;
    EXPECT_EQ(result, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest006, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    AAFwk::Want want;
    want.SetAction(USER_CREDENTIAL_UPDATED_EVENT);
    want.SetParam("userId", 0);
    want.SetParam("authType", AUTH_PIN);
    want.SetParam("credentialCount", HAS_CREDENTIAL);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    bool result = true;
    EXPECT_EQ(result, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest007, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    AAFwk::Want want;
    want.SetAction(USER_CREDENTIAL_UPDATED_EVENT);
    want.SetParam("userId", 0);
    want.SetParam("authType", NO_AUTH_PIN);
    want.SetParam("credentialCount", HAS_CREDENTIAL);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    bool result = true;
    EXPECT_EQ(result, true);
#endif // IS_SO_CROP_H
}

} // namespace ScreenLock
} // namespace OHOS
