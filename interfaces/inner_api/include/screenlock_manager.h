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
class ScreenLockManager : public RefBase {
public:
    SCREENLOCK_API static sptr<ScreenLockManager> GetInstance();
    /**
     * Lock the screen.
     *
     * @param userId Indicates the user ID.
     * @return Returns E_SCREENLOCK_OK if success; otherwise failed.
     */
    SCREENLOCK_API int32_t Lock(int32_t userId);

    /**
     * Check whether the screen is currently locked.
     *
     * @param isLocked Indicates the screen lock state.
     * @return Returns E_SCREENLOCK_OK if success; otherwise failed.
     */
    SCREENLOCK_API int32_t IsLocked(bool &isLocked);

    SCREENLOCK_API bool IsScreenLocked();
    bool GetSecure();
    int32_t Unlock(Action action, const sptr<ScreenLockCallbackInterface> &listener);
    int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener);
private:
    class ScreenLockSaDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ScreenLockSaDeathRecipient(){};
        ~ScreenLockSaDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject> &object) override
        {
            ScreenLockManager::GetInstance()->OnRemoteSaDied(object);
        }
    };

    ScreenLockManager();
    ~ScreenLockManager() override;
    void RemoveDeathRecipient();
    void OnRemoteSaDied(const wptr<IRemoteObject> &object);
    sptr<ScreenLockManagerInterface> GetProxy();
    sptr<ScreenLockManagerInterface> GetScreenLockManagerProxy();
    static std::mutex instanceLock_;
    static sptr<ScreenLockManager> instance_;
    sptr<ScreenLockSaDeathRecipient> deathRecipient_;
    std::mutex managerProxyLock_;
    sptr<ScreenLockManagerInterface> screenlockManagerProxy_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICES_MANAGER_H