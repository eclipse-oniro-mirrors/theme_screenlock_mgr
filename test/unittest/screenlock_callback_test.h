/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef NAPI_SCREENLOCK_CALL_BACK_TEST_H
#define NAPI_SCREENLOCK_CALL_BACK_TEST_H

#include "screenlock_callback_stub.h"
#include "screenlock_common.h"
#include "screenlock_event_list_test.h"
#include "screenlock_callback_interface.h"
#include "screenlock_manager_stub.h"
#include "screenlock_system_ability_stub.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockSystemAbilityTest : public ScreenLockSystemAbilityStub {
public:
    explicit ScreenLockSystemAbilityTest(const EventListenerTest &eventListener);
    virtual ~ScreenLockSystemAbilityTest();
    void OnCallBack(const SystemEvent &systemEvent) override;
};

class ScreenlockCallbackTest : public ScreenLockCallbackStub {
public:
    explicit ScreenlockCallbackTest(const EventListenerTest &eventListener);
    virtual ~ScreenlockCallbackTest();
    void OnCallBack(const int32_t screenLockResult) override;
};

class ScreenLockManagerStubTest : public ScreenLockManagerStub {
public:
    ScreenLockManagerStubTest();
    ScreenLockManagerStubTest(bool flag);
    virtual ~ScreenLockManagerStubTest();
    virtual int32_t IsLocked(bool &isLocked);
    virtual bool IsScreenLocked();
    virtual bool GetSecure();
    virtual int32_t Unlock(const sptr<ScreenLockCallbackInterface> &listener);
    virtual int32_t UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener);
    virtual int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener);
    virtual int32_t Lock(int32_t userId);
    virtual int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener);
    virtual int32_t SendScreenLockEvent(const std::string &event, int param);
    virtual int32_t IsScreenLockDisabled(int userId, bool &isDisabled);
    virtual int32_t SetScreenLockDisabled(bool disable, int userId);
    virtual int32_t SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken);
    virtual int32_t GetScreenLockAuthState(int userId, int32_t &authState);
    virtual int32_t RequestStrongAuth(int reasonFlag, int32_t userId);
    virtual int32_t GetStrongAuth(int32_t userId, int32_t &reasonFlag);
    virtual int32_t IsDeviceLocked(int userId, bool &isDeviceLocked);
    virtual int32_t IsLockedWithUserId(int userId, bool &isLocked);
    virtual int32_t RegisterInnerListener(const int32_t userId, const ListenType listenType,
                                          const sptr<InnerListenerIf> &listener);
    virtual int32_t UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
                                            const sptr<InnerListenerIf> &listener);
private:
    bool mFlag = false;
};

} // namespace ScreenLock
} // namespace OHOS
#endif //  NAPI_SCREENLOCK_CALL_BACK_TEST_H