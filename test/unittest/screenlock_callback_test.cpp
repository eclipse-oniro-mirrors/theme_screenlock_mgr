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
#include "screenlock_callback_test.h"

#include "sclock_log.h"
#include "screenlock_common.h"
#include <string>

namespace OHOS {
namespace ScreenLock {
ScreenLockSystemAbilityTest::ScreenLockSystemAbilityTest(const EventListenerTest &eventListener)
{
}

ScreenLockSystemAbilityTest::~ScreenLockSystemAbilityTest()
{
}

void ScreenLockSystemAbilityTest::OnCallBack(const SystemEvent &systemEvent)
{
    SCLOCK_HILOGD("event=%{public}s,params=%{public}s", systemEvent.eventType_.c_str(), systemEvent.params_.c_str());
}

ScreenlockCallbackTest::ScreenlockCallbackTest(const EventListenerTest &eventListener)
{
}

ScreenlockCallbackTest::~ScreenlockCallbackTest()
{
}

void ScreenlockCallbackTest::OnCallBack(const int32_t screenLockResult)
{
    SCLOCK_HILOGD("screenLockResult=%{public}d", screenLockResult);
}

int32_t ScreenLockManagerStubTest::IsLocked(bool &isLocked)
{
    return 0;
}

bool ScreenLockManagerStubTest::IsScreenLocked()
{
    return false;
}

bool ScreenLockManagerStubTest::GetSecure()
{
    return false;
}

int32_t ScreenLockManagerStubTest::Unlock(const sptr<ScreenLockCallbackInterface> &listener)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::Lock(const sptr<ScreenLockCallbackInterface> &listener)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::Lock(int32_t userId)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::SendScreenLockEvent(const std::string &event, int param)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::IsScreenLockDisabled(int userId, bool &isDisabled)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::SetScreenLockDisabled(bool disable, int userId)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::GetScreenLockAuthState(int userId, int32_t &authState)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::RequestStrongAuth(int reasonFlag, int32_t userId)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::GetStrongAuth(int32_t userId, int32_t &reasonFlag)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::IsDeviceLocked(int userId, bool &isDeviceLocked)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::IsLockedWithUserId(int userId, bool &isLocked)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::RegisterInnerListener(const int32_t userId, const ListenType listenType,
    const sptr<InnerListenerIf> &listener)
{
    return 0;
}

int32_t ScreenLockManagerStubTest::UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
    const sptr<InnerListenerIf> &listener)
{
    return 0;
}
} // namespace ScreenLock
} // namespace OHOS
