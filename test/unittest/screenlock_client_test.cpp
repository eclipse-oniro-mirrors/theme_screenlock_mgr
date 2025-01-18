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
* @tc.desc: set screen state and get state of the screen.
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
    bool isLocked = false;
    ScreenLockManager::GetInstance()->IsLocked(isLocked);
    bool result = ScreenLockManager::GetInstance()->GetSecure();
    SCLOCK_HILOGD(" result is-------->%{public}d", result);
    EXPECT_EQ(result, false);
}

/**
* @tc.name: LockTest003
* @tc.desc: Test Lock and Unlock
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest003, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock and RequestUnlock");
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCK, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCKSCREEN, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    listener = new (std::nothrow) ScreenlockCallbackTest(g_unlockTestListener);
    ASSERT_NE(listener, nullptr);
    result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCK, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
    result = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCKSCREEN, listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_FOCUS_APP);
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
    sptr<ScreenLockSystemAbilityInterface> listener = new (std::nothrow)
        ScreenLockSystemAbilityTest(g_unlockTestListener);
    ASSERT_NE(listener, nullptr);
    int32_t result = ScreenLockManager::GetInstance()->OnSystemEvent(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
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
    int32_t result = ScreenLockManager::GetInstance()->SendScreenLockEvent("test", testNum);
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
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
    int32_t result = ScreenLockManager::GetInstance()->OnSystemEvent(listener);
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
    ScreenLockManager::GetInstance()->screenlockManagerProxy_ = nullptr;
    sptr<ScreenLockManagerInterface> proxy = ScreenLockManager::GetInstance()->GetProxy();
    EXPECT_NE(proxy, nullptr);
    ScreenLockManager::GetInstance()->screenlockManagerProxy_ = nullptr;
    proxy = nullptr;
    proxy = ScreenLockManager::GetInstance()->GetProxy();
    EXPECT_NE(proxy, nullptr);
}

/**
* @tc.name: ProxyTest008
* @tc.desc: Test Lock, UnLock and OnSystemEvent.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, ProxyTest008, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock, RequestUnLock and OnSystemEvent.");
    auto proxy = ScreenLockManager::GetInstance()->GetProxy();
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    int32_t result = proxy->OnSystemEvent(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    sptr<ScreenLockCallbackInterface> callback = nullptr;
    result = proxy->Lock(callback);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = proxy->Unlock(callback);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
    result = proxy->Unlock(callback);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

/**
* @tc.name: LockTest009
* @tc.desc: Test Lock Screen.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest009, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestLock.");
    auto proxy = ScreenLockManager::GetInstance()->GetProxy();
    int32_t userId = 0;
    int32_t result = proxy->Lock(userId);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: LockTest010
* @tc.desc: Test SetScreenLockDisabled.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0010, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockDisabled.");
    auto proxy = ScreenLockManager::GetInstance()->GetProxy();
    int32_t userId = 0;
    int32_t result = proxy->SetScreenLockDisabled(false, userId);
    SCLOCK_HILOGD("SetScreenLockDisabled.[result]:%{public}d", result);
    bool isDisabled = true;
    result = proxy->IsScreenLockDisabled(userId, isDisabled);
    SCLOCK_HILOGD("SetScreenLockDisabled.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}


/**
* @tc.name: LockTest0011
* @tc.desc: Test SetScreenLockAuthState.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0011, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockAuthState.");
    auto proxy = ScreenLockManager::GetInstance()->GetProxy();
    int32_t userId = 0;
    std::string authtoken = "test";
    int32_t result = proxy->SetScreenLockAuthState(1, userId, authtoken);
    SCLOCK_HILOGD("SetScreenLockAuthState.[result]:%{public}d", result);
    int32_t authState = 0;
    result = proxy->GetScreenLockAuthState(userId, authState);
    SCLOCK_HILOGD("SetScreenLockAuthState.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: LockTest0012
* @tc.desc: Test SetScreenLockAuthState.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0012, TestSize.Level0)
{
    SCLOCK_HILOGD("Test RequestStrongAuth.");
    int32_t userId = 0;
    std::string authtoken = "test";
    int32_t result = ScreenLockManager::GetInstance()->RequestStrongAuth(1, userId);
    SCLOCK_HILOGD("RequestStrongAuth.[result]:%{public}d", result);
    int32_t reasonFlag = 0;
    result = ScreenLockManager::GetInstance()->GetStrongAuth(userId, reasonFlag);
    SCLOCK_HILOGD("GetStrongAuth.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: LockTest013
* @tc.desc: Test SetScreenLockDisabled.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0013, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockDisabled.");
    int32_t userId = 0;
    int32_t result = ScreenLockManager::GetInstance()->SetScreenLockDisabled(false, userId);
    SCLOCK_HILOGD("SetScreenLockDisabled.[result]:%{public}d", result);
    bool isDisabled = true;
    result = ScreenLockManager::GetInstance()->IsScreenLockDisabled(userId, isDisabled);
    SCLOCK_HILOGD("SetScreenLockDisabled.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}


/**
* @tc.name: LockTest0014
* @tc.desc: Test SetScreenLockAuthState.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0014, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockAuthState.");
    int32_t userId = 0;
    std::string authtoken = "test";
    int32_t result = ScreenLockManager::GetInstance()->SetScreenLockAuthState(1, userId, authtoken);
    SCLOCK_HILOGD("SetScreenLockAuthState.[result]:%{public}d", result);
    int32_t authState = 0;
    result = ScreenLockManager::GetInstance()->GetScreenLockAuthState(userId, authState);
    SCLOCK_HILOGD("SetScreenLockAuthState.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

/**
* @tc.name: LockTest0015
* @tc.desc: Test RequestStrongAuth.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0015, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockAuthState.");
    auto proxy = ScreenLockManager::GetInstance()->GetProxy();
    int32_t userId = 0;
    int32_t result = proxy->RequestStrongAuth(1, userId);
    SCLOCK_HILOGD("RequestStrongAuth.[result]:%{public}d", result);
    int32_t reasonFlag = 0;
    result = proxy->GetStrongAuth(userId, reasonFlag);
    SCLOCK_HILOGD("GetStrongAuth.[result]:%{public}d", result);
    EXPECT_EQ(result, E_SCREENLOCK_NO_PERMISSION);
}

} // namespace ScreenLock
} // namespace OHOS