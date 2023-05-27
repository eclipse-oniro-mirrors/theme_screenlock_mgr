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

#ifndef SERVICES_INCLUDE_SCLOCK_MANAGER_H
#define SERVICES_INCLUDE_SCLOCK_MANAGER_H

#include <mutex>
#include <string>

#include "iremote_object.h"
#include "refbase.h"
#include "screenlock_callback_interface.h"
#include "screenlock_common.h"
#include "screenlock_manager_interface.h"
#include "visibility.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockSaDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ScreenLockSaDeathRecipient();
    ~ScreenLockSaDeathRecipient() override;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
};

class ScreenLockManager : public RefBase {
public:
    SCREENLOCK_API ScreenLockManager();
    SCREENLOCK_API ~ScreenLockManager() override;
    SCREENLOCK_API static sptr<ScreenLockManager> GetInstance();
    SCREENLOCK_API int32_t IsLocked(bool &isLocked);

    /**
     * @brief Checks whether the screen is currently locked.
     *
     * This function is used to Check whether the screen is currently locked.
     *
     * @return Returns true if the screen is currently locked; returns false otherwise.
     * @since 7
     */
    SCREENLOCK_API bool IsScreenLocked();

    SCREENLOCK_API bool GetSecure();
    SCREENLOCK_API int32_t Unlock(Action action, const sptr<ScreenLockCallbackInterface> &listener);
    SCREENLOCK_API int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener);
    void OnRemoteSaDied(const wptr<IRemoteObject> &object);
    SCREENLOCK_API sptr<ScreenLockManagerInterface> GetProxy();

private:
    static sptr<ScreenLockManagerInterface> GetScreenLockManagerProxy();
    static std::mutex instanceLock_;
    static sptr<ScreenLockManager> instance_;
    static sptr<ScreenLockSaDeathRecipient> deathRecipient_;
    std::mutex managerProxyLock_;
    sptr<ScreenLockManagerInterface> screenlockManagerProxy_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICES_MANAGER_H