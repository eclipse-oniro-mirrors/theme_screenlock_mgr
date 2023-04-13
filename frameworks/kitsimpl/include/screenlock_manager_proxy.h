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
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability_interface.h"

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
    int32_t Unlock(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t UnlockScreen(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t Lock(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t SendScreenLockEvent(const std::string &event, int param) override;

private:
    int32_t UnlockInner(MessageParcel &reply, int32_t command, const sptr<ScreenLockSystemAbilityInterface> &listener);
    int32_t IsScreenLockedInner(MessageParcel &reply, int32_t command);
    static inline BrokerDelegator<ScreenLockManagerProxy> delegator_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_PROXY_H