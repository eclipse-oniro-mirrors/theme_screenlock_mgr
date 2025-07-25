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
#define private public
#define protected public
#include "strongauthmanager.h"
#undef private
#undef protected
#endif // IS_SO_CROP_H
#include "commeventsubscriber.h"
#include "screenlock_strongauth_test.h"
#include "user_idm_client_defines.h"
#include "os_account_manager.h"

namespace OHOS {
namespace ScreenLock {
const std::string AUTH_PIN = "1";
const std::string NO_AUTH_PIN = "0";
const std::string TAG_AUTHTYPE = "authType";
const std::string HAS_CREDENTIAL = "1";
const std::string HAS_NO_CREDENTIAL = "0";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";
using namespace OHOS::UserIam::UserAuth;
using namespace OHOS::AccountSA;
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
    ASSERT_NE(authmanager, nullptr);

    int32_t userId = 100;
    int32_t defaulVal = 1;
    int64_t timerInterval = 1;
    int32_t reasonFlag = 0;
    authmanager->RegistIamEventListener();
    authmanager->StartStrongAuthTimer(userId);
    authmanager->InitStrongAuthStat(userId, reasonFlag);
    authmanager->GetCredInfo(userId);
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
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest002, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    int type = 1;
    timer.SetType(type);
    timer.SetUserId(1);
    timer.GetUserId();
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest003, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::authTimer timer;
    timer.OnTrigger();
    StrongAuthManger::authTimer timer1(true, 1000, true, true);
    EXPECT_EQ(timer1.repeat, true);
    EXPECT_EQ(timer1.interval, 1000);
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
    StrongAuthManger::authTimer timer1(true, 1000, true, true);
    EXPECT_EQ(timer1.repeat, true);
    EXPECT_EQ(timer1.interval, 1000);
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
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
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
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
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
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
    EXPECT_EQ(timer.interval, 1000);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest008, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    AAFwk::Want want;
    const std::string action = "test";
    want.SetAction(action);
    want.SetParam("userId", 0);
    want.SetParam("authType", NO_AUTH_PIN);
    want.SetParam("credentialCount", HAS_CREDENTIAL);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest009, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    AAFwk::Want want;
    want.SetAction(USER_CREDENTIAL_UPDATED_EVENT);
    want.SetParam("userId", 0);
    want.SetParam("authType", AUTH_PIN);
    want.SetParam("credentialCount", HAS_NO_CREDENTIAL);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest10, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    std::shared_ptr<StrongAuthManger::AuthEventListenerService> authSuccessListener =
        std::make_shared<StrongAuthManger::AuthEventListenerService>();
    std::string bundleName = "test";
    authSuccessListener->OnNotifyAuthSuccessEvent(1, AuthType::FACE, 1, bundleName);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest11, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    std::shared_ptr<StrongAuthManger::AuthEventListenerService> authSuccessListener =
        std::make_shared<StrongAuthManger::AuthEventListenerService>();;
    std::string bundleName = "test";
    authSuccessListener->OnNotifyAuthSuccessEvent(1, AuthType::PIN, 1, bundleName);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest12, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    std::shared_ptr<StrongAuthManger::CredChangeListenerService> creChangeListener =
        std::make_shared<StrongAuthManger::CredChangeListenerService>();
    CredChangeEventType eventType = CredChangeEventType::ADD_CRED;
    UserIam::UserAuth::CredChangeEventInfo changeInfo = {};
    creChangeListener->OnNotifyCredChangeEvent(1, AuthType::PIN, eventType, changeInfo);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest13, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    std::shared_ptr<StrongAuthManger::CredChangeListenerService> creChangeListener =
        std::make_shared<StrongAuthManger::CredChangeListenerService>();
    CredChangeEventType eventType = CredChangeEventType::ADD_CRED;
    UserIam::UserAuth::CredChangeEventInfo changeInfo = {};
    creChangeListener->OnNotifyCredChangeEvent(1, AuthType::FACE, eventType, changeInfo);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest14, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    std::shared_ptr<StrongAuthManger::CredChangeListenerService> creChangeListener =
        std::make_shared<StrongAuthManger::CredChangeListenerService>();
    CredChangeEventType eventType = CredChangeEventType::UPDATE_CRED;
    UserIam::UserAuth::CredChangeEventInfo changeInfo = {};
    creChangeListener->OnNotifyCredChangeEvent(1, AuthType::FACE, eventType, changeInfo);
    StrongAuthManger::authTimer timer(true, 1000, true, true);
    EXPECT_EQ(timer.repeat, true);
#endif
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest015, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    ASSERT_NE(authmanager, nullptr);

    int32_t otherUserId = 102;
    int32_t reasonFlag = 0;
    authmanager->GetStrongAuthTimeTrigger(otherUserId);

    authmanager->IsUserHasStrongAuthTimer(otherUserId);

    authmanager->ResetStrongAuthTimer(otherUserId, DEFAULT_STRONG_AUTH_TIMEOUT_MS);

    auto flag = authmanager->IsUserHasStrongAuthTimer(otherUserId);

    authmanager->GetStrongAuthTimeTrigger(otherUserId);

    authmanager->GetCredInfo(otherUserId);

    authmanager->InitStrongAuthStat(otherUserId, reasonFlag);

    authmanager->GetCredInfo(otherUserId);

    authmanager->DestroyStrongAuthTimer(otherUserId);

    EXPECT_EQ(flag, true);
#endif  // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest016, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    ASSERT_NE(authmanager, nullptr);

    int32_t otherUserId = 102;
    int32_t reasonFlag = 0;
    authmanager->DestroyStrongAuthStateInfo(otherUserId);

    authmanager->IsUserExitInStrongAuthInfo(otherUserId);

    authmanager->InitStrongAuthStat(otherUserId, reasonFlag);

    auto flag = authmanager->IsUserExitInStrongAuthInfo(otherUserId);

    authmanager->InitStrongAuthStat(otherUserId, reasonFlag);

    authmanager->DestroyStrongAuthStateInfo(otherUserId);

    EXPECT_EQ(flag, true);
#endif  // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest017, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    ASSERT_NE(authmanager, nullptr);

    int32_t otherUserId = 102;
    int32_t reasonFlag = 0;

    authmanager->GetStrongAuthTriggerPeriod(otherUserId);

    authmanager->ResetStrongAuthTimer(otherUserId, DEFAULT_STRONG_AUTH_TIMEOUT_MS);

    authmanager->GetStrongAuthTriggerPeriod(otherUserId);

    authmanager->ResetStrongAuthTimer(otherUserId, CRED_CHANGE_FIRST_STRONG_AUTH_TIMEOUT_MS);

    authmanager->GetStrongAuthTriggerPeriod(otherUserId);

    authmanager->ResetStrongAuthTimer(otherUserId, CRED_CHANGE_SECOND_STRONG_AUTH_TIMEOUT_MS);

    authmanager->GetStrongAuthTriggerPeriod(otherUserId);

    authmanager->GetStrongAuthTimeTrigger(otherUserId);

    auto flag = authmanager->IsUserHasStrongAuthTimer(otherUserId);

    authmanager->DestroyStrongAuthTimer(otherUserId);

    EXPECT_EQ(flag, true);
#endif  // IS_SO_CROP_H
}

HWTEST_F(ScreenLockStrongAuthTest, ScreenLockStrongAuthTest018, TestSize.Level0)
{
#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    ASSERT_NE(authmanager, nullptr);

    int32_t otherUserId = 102;
    int32_t reasonFlag = 1;

    authmanager->SetStrongAuthStat(otherUserId, reasonFlag);

    authmanager->SetStrongAuthStat(otherUserId, reasonFlag);

    authmanager->SetStrongAuthStat(otherUserId, 2);

    authmanager->SetStrongAuthStat(otherUserId, 0);

    authmanager->SetStrongAuthStat(otherUserId, 2);

    authmanager->SetStrongAuthStat(otherUserId, 0);

    authmanager->SetStrongAuthStat(otherUserId, 0);

    auto flag = authmanager->IsUserExitInStrongAuthInfo(otherUserId);

    authmanager->DestroyStrongAuthStateInfo(otherUserId);

    EXPECT_EQ(flag, true);
#endif  // IS_SO_CROP_H
}
} // namespace ScreenLock
} // namespace OHOS
