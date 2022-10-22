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
#define OHOS_DEBUG
#include "screenlock_service_test.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <sys/time.h>

#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_manager.h"
#include "screenlock_notify_test_instance.h"
#include "screenlock_system_ability.h"
#include "screenlock_system_ability_stub.h"
#include "screenlock_app_manager.h"
#include "screenlock_callback_test.h"

namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::ScreenLock;

static EventListenerTest g_unlockTestListener;

void ScreenLockServiceTest::SetUpTestCase()
{
}

void ScreenLockServiceTest::TearDownTestCase()
{
}

void ScreenLockServiceTest::SetUp()
{
}

void ScreenLockServiceTest::TearDown()
{
}

/**
* @tc.name: SetScreenLockTest001
* @tc.desc: get unlockstate, IsScreenLocked state.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, SetScreenLockTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  IsScreenLocked state ,get unlockstate");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (status) {
        ScreenLockSystemAbility::GetInstance()->SetScreenlocked(false);
        bool result = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
        SCLOCK_HILOGD("IsScreenLocked  result is-------->%{public}d", result);
        EXPECT_EQ(result, false);
    } else {
        EXPECT_EQ(status, false);
    }
}

/**
* @tc.name: SetScreenLockTest002
* @tc.desc: get lockstate, IsScreenLocked state.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, SetScreenLockTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  IsScreenLocked state ,get lockstate");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (!status) {
        ScreenLockSystemAbility::GetInstance()->SetScreenlocked(true);
        bool result = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
        SCLOCK_HILOGD("IsScreenLocked  result is-------->%{public}d", result);
        EXPECT_EQ(result, true);
    } else {
        EXPECT_EQ(status, true);
    }
}

/**
* @tc.name: GetSecureTest003
* @tc.desc: get secure.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, GetSecureTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  secure");
    bool result = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD(" result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: RequestLockTest004
* @tc.desc: can not get foucs, lock fail.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, RequestLockTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("Test can not get foucs,expect lock fail");
    bool ret = ScreenLockManager::GetInstance()->IsScreenLocked();
    sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return ;
    }
    ScreenLockManager::GetInstance()->RequestLock(listener);
    std::string event = LOCK_SCREEN_RESULT;
    ScreenLockAppManager::GetInstance()->SendScreenLockEvent(event, SCREEN_FAIL);
    bool result = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("get not foucs IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, ret);
}

/**
* @tc.name: RequestUnlockTest005
* @tc.desc: can not get foucs, unlock fail.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, RequestUnlockTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("Test can not get foucs,expect unlock fail");
    bool ret = ScreenLockManager::GetInstance()->IsScreenLocked();
    sptr<ScreenLockSystemAbilityInterface> listener = new ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return ;
    }
    ScreenLockManager::GetInstance()->RequestUnlock(listener);
    std::string event = UNLOCK_SCREEN_RESULT;
    ScreenLockAppManager::GetInstance()->SendScreenLockEvent(event, SCREEN_FAIL);
    bool result = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("get not foucs IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, ret);
}

/**
* @tc.name: ScreenLockTest006
* @tc.desc: test beginWakeUp.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginWakeUp");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (status) {
        std::string message = BEGIN_WAKEUP;
        int param = 0;
        bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
        SCLOCK_HILOGD("RuntimeNotify beginWakeUp  result is-------->%{public}d", result);
        int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
        EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_WAKEUP), true);
    } else {
        EXPECT_EQ(status, false);
    }
}
#endif

/**
* @tc.name: ScreenLockTest007
* @tc.desc: test endWakeUp.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest007, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endWakeUp");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (status) {
        std::string message = END_WAKEUP;
        int param = 0;
        result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
        SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
        retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
        EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_WAKEUP), true);
    } else {
        EXPECT_EQ(status, false);
    }
}
#endif

/**
* @tc.name: ScreenLockTest008
* @tc.desc: test beginScreenOn.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginScreenOn");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (status) {
        std::string message = BEGIN_SCREEN_ON;
        int param = 0;
        result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
        SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
        retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
        EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_ON), true);
    } else {
        EXPECT_EQ(status, false);
    }
}
#endif

/**
* @tc.name: ScreenLockTest009
* @tc.desc: test endScreenOn.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest009, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endScreenOn");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    if (status) {
        std::string message = END_SCREEN_ON;
        int param = 0;
        result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
        SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
        retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
        EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_END_ON), true);
    } else {
        EXPECT_EQ(status, false);
    }
}
#endif

/**
* @tc.name: ScreenLockTest010
* @tc.desc: test beginScreenOff.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest010, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginScreenOff");
    std::string message = BEGIN_SCREEN_OFF;
    int param = 0;
    result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_OFF), true);
}
#endif

/**
* @tc.name: ScreenLockTest011
* @tc.desc: test endScreenOff.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest011, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endScreenOff");
    std::string message = END_SCREEN_OFF;
    int param = 0;
    result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == static_cast<int>(ScreenState::SCREEN_STATE_END_OFF), true);
}
#endif

/**
* @tc.name: ScreenLockTest012
* @tc.desc: test beginSleep.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest012, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of beginSleep");
    std::string message = BEGIN_SLEEP;
    int param = BEGIN_SLEEP_DEVICE_ADMIN_REASON;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_SLEEP), true);
}
#endif

/**
* @tc.name: ScreenLockTest013
* @tc.desc: test endSleep.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest013, TestSize.Level0)
{
    SCLOCK_HILOGD("Test event of endSleep");
    std::string message = END_SLEEP;
    int param = END_SLEEP_USER_REASON;
    result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_SLEEP), true);
}
#endif

/**
* @tc.name: ScreenLockTest014
* @tc.desc: test user ID is 10.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest014, TestSize.Level0)
{
    SCLOCK_HILOGD("Test userid 10");
    std::string message = CHANGE_USER;
    int param = 10;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == param, true);
}
#endif

/**
* @tc.name: ScreenLockTest015
* @tc.desc: test user ID is 0.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest015, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  userid is 0");
    std::string message = CHANGE_USER;
    int param = 0;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == param, true);
}
#endif

/**
* @tc.name: ScreenLockTest016
* @tc.desc: test negative value.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest016, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  userid -2");
    const int MINUSERID = 0;
    int param = -2;
    EXPECT_EQ(param >= MINUSERID, false);
}

/**
* @tc.name: ScreenLockTest017
* @tc.desc: test large values.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockServiceTest, ScreenLockTest017, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  userid 999999999");
    const int MAXUSERID = 999999999;
    int param = 999999999;
    EXPECT_EQ(param < MAXUSERID, false);
}

/**
* @tc.name: ScreenLockTest018
* @tc.desc: whether the lock screen application is available.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest018, TestSize.Level0)
{
    std::string message = SCREENLOCK_ENABLED;
    int param = SCREENLOCK_APP_CAN_USE;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == param, true);
}
#endif

/**
* @tc.name: ScreenLockTest019
* @tc.desc: whether the lock screen application is available.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest019, TestSize.Level0)
{
    std::string message = SCREENLOCK_ENABLED;
    int param = SCREENLOCK_APP_CAN_NOT_USE;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    int retVal = ScreenLockSystemAbility::GetInstance()->GetRuntimeState(message);
    EXPECT_EQ(retVal == param, true);
}
#endif

/**
* @tc.name: ScreenLockTest020
* @tc.desc: test exit animation.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
#ifdef OHOS_DEBUG
HWTEST_F(ScreenLockServiceTest, ScreenLockTest020, TestSize.Level0)
{
    std::string message = EXIT_ANIMATION;
    int param = 0;
    bool result = ScreenLockSystemAbility::GetInstance()->RuntimeNotify(message, param);
    SCLOCK_HILOGD("RuntimeNotify message=%{public}s,result =%{public}d", message.c_str(), result);
    EXPECT_EQ(result, true);
}
#endif
} // namespace ScreenLock
} // namespace OHOS