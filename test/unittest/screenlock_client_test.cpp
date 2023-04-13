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
#include "screenlock_manager.h"
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
    SCLOCK_HILOGD("Test IsScreenLocked state");
    ScreenLockSystemAbility::GetInstance()->SetScreenlocked(true);
    bool isLocked = ScreenLockSystemAbility::GetInstance()->IsScreenLocked();
    EXPECT_EQ(isLocked, true);
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
    SCLOCK_HILOGD("Test RequestLock and RequestUnlock");
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCK, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCKSCREEN, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);

    listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener object is nullptr");
        EXPECT_EQ(false, true);
        return;
    }
    result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCK, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCKSCREEN, listener);
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

/**
* @tc.name: OnSystemEventTest006
* @tc.desc: Test OnSystemEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, OnSystemEventTest006, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnSystemEvent");
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    int32_t result = ScreenLockAppManager::GetInstance()->OnSystemEvent(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

/**
* @tc.name: GetProxyTest007
* @tc.desc: Test GetProxy.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, GetProxyTest007, TestSize.Level0)
{
    SCLOCK_HILOGD("Test GetProxy");
    ScreenLockAppManager::GetInstance()->screenlockManagerProxy_ = nullptr;
    sptr<ScreenLockManagerInterface> proxy = ScreenLockAppManager::GetInstance()->GetProxy();
    EXPECT_NE(proxy, nullptr);
    ScreenLockManager::GetInstance()->screenlockManagerProxy_ = nullptr;
    proxy = nullptr;
    proxy = ScreenLockManager::GetInstance()->GetProxy();
    EXPECT_NE(proxy, nullptr);
}

/**
* @tc.name: ProxyTest008
* @tc.desc: Test RequestLock, RequestUnLock and OnSystemEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, ProxyTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock, RequestUnLock and OnSystemEvent.");
    auto proxy = ScreenLockAppManager::GetInstance()->GetProxy();
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    int32_t result = proxy->OnSystemEvent(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = proxy->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = proxy->Unlock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}
} // namespace ScreenLock
} // namespace OHOS