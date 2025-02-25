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

#ifndef SERVICES_INCLUDE_SCLOCK_SERVICE_PROXY_H
#define SERVICES_INCLUDE_SCLOCK_SERVICE_PROXY_H

#include <string>

#include "iremote_proxy.h"
#include "refbase.h"
#include "screenlock_callback_interface.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability_interface.h"
#include "screenlock_strongauth_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockManagerProxy : public IRemoteProxy<ScreenLockManagerInterface> {
public:
    explicit ScreenLockManagerProxy(const sptr<IRemoteObject> &object);
    ~ScreenLockManagerProxy() = default;
    DISALLOW_COPY_AND_MOVE(ScreenLockManagerProxy);
    int32_t IsLocked(bool &isLocked) override;
    bool IsScreenLocked() override;
    bool GetSecure() override;
    int32_t Unlock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t Lock(int32_t userId) override;
    int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t SendScreenLockEvent(const std::string &event, int param) override;
    int32_t IsScreenLockDisabled(int userId, bool &isDisabled) override;
    int32_t SetScreenLockDisabled(bool disable, int userId) override;
    int32_t SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken) override;
    int32_t GetScreenLockAuthState(int userId, int32_t &authState) override;
    int32_t RequestStrongAuth(int reasonFlag, int32_t userId) override;
    int32_t GetStrongAuth(int userId, int32_t &reasonFlag) override;
    int32_t IsDeviceLocked(int userId, bool &isDeviceLocked) override;
    int32_t RegisterStrongAuthListener(const int32_t userId, const sptr<StrongAuthListenerInterface> &listener) override;
    int32_t UnRegisterStrongAuthListener(const int32_t userId, const sptr<StrongAuthListenerInterface> &listener) override;
private:
    int32_t UnlockInner(MessageParcel &reply, int32_t command, const sptr<ScreenLockCallbackInterface> &listener);
    int32_t IsScreenLockedInner(MessageParcel &reply, uint32_t command);
    static inline BrokerDelegator<ScreenLockManagerProxy> delegator_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_PROXY_H