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
#define private public
#define protected public
#include "screenlock_app_manager.h"
#undef private
#undef protected

#include <cstdint>
#include <list>
#include <string>
#include <sys/time.h>

#include "sclock_log.h"
#include "screenlock_callback_test.h"
#include "screenlock_client_test.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_manager.h"
#include "screenlock_notify_test_instance.h"
#include "screenlock_system_ability.h"
#include "securec.h"

namespace OHOS {
namespace ScreenLock {
using namespace testing::ext;

static EventListenerTest g_unlockTestListener;

void ScreenLockClientTest::SetUpTestCase()
{
}

void ScreenLockClientTest::TearDownTestCase()
{
}

void ScreenLockClientTest::SetUp()
{
}

void ScreenLockClientTest::TearDown()
{
}

/**
* @tc.name: SetScreenLockTest001
* @tc.desc: get unlockstate, IsScreenLocked state.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, SetScreenLockTest001, TestSize.Level0)
{
    SCLOCK_HILOGD("Test  IsScreenLocked state, get unlockstate");
    bool status = ScreenLockManager::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  status is-------->%{public}d", status);
    ScreenLockSystemAbility::GetInstance()->SetScreenlocked(false);
    bool result = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    SCLOCK_HILOGD("IsScreenLocked  result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: GetSecureTest002
* @tc.desc: get secure.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, GetSecureTest002, TestSize.Level0)
{
    SCLOCK_HILOGD("Test secure");
    bool result = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD(" result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: RequestLockTest003
* @tc.desc: Test RequestLock and RequestUnlock
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, RequestLockTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test can not get foucs,expect lock fail");
    sptr<ScreenLockSystemAbilityInterface> listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return;
    }
    int32_t result = ScreenLockManager::GetInstance()->RequestLock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
    result = ScreenLockManager::GetInstance()->RequestUnlock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: OnSystemEventTest004
* @tc.desc: Test OnSystemEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, OnSystemEventTest004, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnSystemEvent");
    sptr<ScreenLockSystemAbilityInterface> listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    int32_t result = ScreenLockAppManager::GetInstance()->OnSystemEvent(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: SendScreenLockEventTest005
* @tc.desc: Test SendScreenLockEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, SendScreenLockEventTest005, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SendScreenLockEvent");
    int testNum = 0;
    int32_t result = ScreenLockAppManager::GetInstance()->SendScreenLockEvent("test", testNum);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}
} // namespace ScreenLock
} // namespace OHOS