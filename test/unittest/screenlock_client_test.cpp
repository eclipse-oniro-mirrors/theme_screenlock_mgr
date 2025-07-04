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
#include "dump_helper.h"
#include "screenlock_get_info_callback.h"
#include "screenlock_inner_listener.h"
#include "inner_listener_test.h"

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
    int32_t userId = ScreenLockSystemAbility::GetInstance()->GetState().GetCurrentUser();
    ScreenLockSystemAbility::GetInstance()->SetScreenlocked(true, userId);
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
    EXPECT_EQ(result, E_SCREENLOCK_OK);
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
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
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
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
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
    EXPECT_EQ(result, E_SCREENLOCK_OK);
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
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
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
    EXPECT_EQ(result, E_SCREENLOCK_NOT_SYSTEM_APP);
}


/**
* @tc.name: LockTest0016
* @tc.desc: Test IsDeviceLocked.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0016, TestSize.Level0)
{
    SCLOCK_HILOGD("Test IsDeviceLocked.");
    int userId = 100;
    bool isDeviceLocked = false;
    int32_t retCode = 0;
    retCode = ScreenLockManager::GetInstance()->IsDeviceLocked(userId, isDeviceLocked);
    SCLOCK_HILOGI("LockTest0016.[retCode]:%{public}d", retCode);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);
}

/**
* @tc.name: LockTest0017
* @tc.desc: Test StrongAuthListener.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0017, TestSize.Level0)
{
    SCLOCK_HILOGD("Test StrongAuthListener.");
    int userId = 100;
    int32_t retCode = 0;
    sptr<StrongAuthListener> StrongAuthListenerTest1 = nullptr;
    retCode = ScreenLockManager::GetInstance()->RegisterStrongAuthListener(StrongAuthListenerTest1);
    SCLOCK_HILOGI("LockTest0017.[retCode]:%{public}d", retCode);
    EXPECT_EQ(retCode, E_SCREENLOCK_NULLPTR);

    retCode = ScreenLockManager::GetInstance()->UnRegisterStrongAuthListener(StrongAuthListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NULLPTR);

    StrongAuthListenerTest1 = new (std::nothrow) StrongAuthListenerTest(userId);
    retCode = ScreenLockManager::GetInstance()->RegisterStrongAuthListener(StrongAuthListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);

    retCode = ScreenLockManager::GetInstance()->UnRegisterStrongAuthListener(StrongAuthListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);
}

/**
* @tc.name: LockTest0018
* @tc.desc: Test StrongAuthListener.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0018, TestSize.Level0)
{
    SCLOCK_HILOGD("Test DeviceLockedListener");
    int userId = 100;
    int32_t retCode = 0;
    sptr<DeviceLockedListener> DeviceLockedListenerTest1 = nullptr;
    retCode = ScreenLockManager::GetInstance()->RegisterDeviceLockedListener(DeviceLockedListenerTest1);
    SCLOCK_HILOGI("LockTest0018.[retCode]:%{public}d", retCode);
    EXPECT_EQ(retCode, E_SCREENLOCK_NULLPTR);

    retCode = ScreenLockManager::GetInstance()->UnRegisterDeviceLockedListener(DeviceLockedListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NULLPTR);

    DeviceLockedListenerTest1 = new (std::nothrow) DeviceLockedListenerTest(userId);
    retCode = ScreenLockManager::GetInstance()->RegisterDeviceLockedListener(DeviceLockedListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);

    retCode = ScreenLockManager::GetInstance()->UnRegisterDeviceLockedListener(DeviceLockedListenerTest1);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);
}

/**
* @tc.name: LockTest0019
* @tc.desc: Test IsLockedWithUserId.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0019, TestSize.Level0)
{
    SCLOCK_HILOGD("Test IsDeviceLocked.");
    int userId = 100;
    bool isLocked = false;
    int32_t retCode = 0;
    bool isDeviceLocked = false;
    retCode = ScreenLockManager::GetInstance()->IsLockedWithUserId(userId, isDeviceLocked);
    SCLOCK_HILOGI("LockTest0019.[retCode]:%{public}d", retCode);
    EXPECT_EQ(retCode, E_SCREENLOCK_NOT_SYSTEM_APP);
}

/**
* @tc.name: LockTest0020
* @tc.desc: Test SetScreenLockAuthState.
* @tc.type: FUNC
* @tc.require:
* @tc.author:
*/
HWTEST_F(ScreenLockClientTest, LockTest0020, TestSize.Level0)
{
    SCLOCK_HILOGD("Test SetScreenLockAuthState.");
    int userId = 0;
    int state = 0;
    sptr<StrongAuthListener> listener = nullptr;
    sptr<InnerListenerWrapper> wrapper = new (std::nothrow) InnerListenerWrapper(listener);
    wrapper->OnStateChanged(userId, state);
    listener = new (std::nothrow) StrongAuthListenerTest(userId);
    wrapper->OnStateChanged(userId, state);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest021, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnRemoveUser state");
    AccountSA::OsAccountSubscribeInfo info;
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest022, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnRemoveUser state");
    AccountSA::OsAccountSubscribeInfo info;
    ScreenLockSystemAbility::AccountSubscriber acount(info, [](const int lastUser, const int targetUser) { return; });
    acount.OnAccountsChanged(0);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest023, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnRemoveUser state");
    AccountSA::OsAccountSubscribeInfo info;
    ScreenLockSystemAbility::AccountSubscriber acount(info, [](const int lastUser, const int targetUser) { return; });
    acount.OnAccountsChanged(0);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest024, TestSize.Level0)
{
    SCLOCK_HILOGD("Test OnRemoveUser state");
    sptr<ScreenLockSystemAbility> instance = ScreenLockSystemAbility::GetInstance();
    instance->SetScreenLockDisabled(true, 0);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest025, TestSize.Level0)
{
    SCLOCK_HILOGD("Test dumpHelper.Dispatch");
    std::vector<std::string> args;
    DumpHelper dumpHelper = DumpHelper::GetInstance();
    bool result = dumpHelper.Dispatch(0, args);
    EXPECT_EQ(result, false);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest026, TestSize.Level0)
{
    SCLOCK_HILOGD("Test dumpHelper.Dispatch");
    std::vector<std::string> args;
    std::string param("-h");
    args.emplace_back(param);
    DumpHelper dumpHelper = DumpHelper::GetInstance();
    bool result = dumpHelper.Dispatch(0, args);
    EXPECT_EQ(result, false);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest027, TestSize.Level0)
{
    SCLOCK_HILOGD("Test duScreenLockGetInfoCallbackpHelper.OnCredentialInfo");
    ScreenLockGetInfoCallback callback;
    std::vector<OHOS::UserIam::UserAuth::CredentialInfo> infoList;
    callback.OnCredentialInfo(0, infoList);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest028, TestSize.Level0)
{
    SCLOCK_HILOGD("Test duScreenLockGetInfoCallbackpHelper.OnCredentialInfo");
    ScreenLockGetInfoCallback callback;
    OHOS::UserIam::UserAuth::CredentialInfo createntialInfo;
    std::vector<OHOS::UserIam::UserAuth::CredentialInfo> infoList;
    infoList.emplace_back(createntialInfo);
    callback.OnCredentialInfo(0, infoList);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

HWTEST_F(ScreenLockClientTest, SetScreenLockTest029, TestSize.Level0)
{
    SCLOCK_HILOGD("Test duScreenLockGetInfoCallbackpHelper.OnCredentialInfo");
    ScreenLockGetInfoCallback callback;
    OHOS::UserIam::UserAuth::CredentialInfo createntialInfo;
    std::vector<OHOS::UserIam::UserAuth::CredentialInfo> infoList;
    infoList.emplace_back(createntialInfo);
    callback.OnCredentialInfo(UserIam::UserAuth::SUCCESS + 10001, infoList);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    int32_t result = ScreenLockManager::GetInstance()->Lock(listener);
    EXPECT_EQ(result, E_SCREENLOCK_NULLPTR);
}

} // namespace ScreenLock
} // namespace OHOS