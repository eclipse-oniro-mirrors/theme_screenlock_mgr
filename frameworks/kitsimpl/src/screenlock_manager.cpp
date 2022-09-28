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

#include "screenlock_manager.h"

#include <hitrace_meter.h>

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ScreenLock {

ScreenLockManager::ScreenLockManager()
{
}

ScreenLockManager::~ScreenLockManager()
{
}

sptr<ScreenLockManager> ScreenLockManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new ScreenLockManager;
            std::lock_guard<std::mutex> autoLock(managerProxyLock_);
            screenlockManagerProxy_ = GetScreenLockManagerProxy();
        }
    }
    return instance_;
}

bool ScreenLockManager::IsScreenLocked()
{
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("IsScreenLocked quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManager IsScreenLocked succeeded.");
    return proxy->IsScreenLocked();
}

bool ScreenLockManager::GetSecure()
{
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("GetSecure quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManager GetSecure succeeded.");
    return proxy->GetSecure();
}

void ScreenLockManager::RequestUnlock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("RequestUnlock quit because redoing GetScreenLockManagerProxy failed.");
        return;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return;
    }
    SCLOCK_HILOGD("ScreenLockManager RequestUnlock succeeded.");
    StartAsyncTrace(HITRACE_TAG_MISC, "ScreenLockManager RequestUnlock start", HITRACE_UNLOCKSCREEN);
    proxy->RequestUnlock(listener);
}

int32_t ScreenLockManager::RequestLock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("RequestLock quit because redoing GetScreenLockManagerProxy failed.");
        return -1;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return -1;
    }
    SCLOCK_HILOGD("ScreenLockManager RequestLock succeeded.");
    return proxy->RequestLock(listener);
}

bool ScreenLockManager::Test_SetScreenLocked(bool isScreenlocked)
{
    bool flag = false;
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::Test_SetScreenLocked quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManager::Test_SetScreenLocked succeeded.");
    flag = proxy->Test_SetScreenLocked(isScreenlocked);
    return flag;
}

bool ScreenLockManager::Test_RuntimeNotify(const std::string &event, int param)
{
    bool flag = false;
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::Test_RuntimeNotify quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManager::Test_RuntimeNotify succeeded.  event=%{public}s", event.c_str());
    SCLOCK_HILOGD("ScreenLockManager::Test_RuntimeNotify succeeded.  param=%{public}d", param);
    flag = proxy->Test_RuntimeNotify(event, param);
    return flag;
}

int ScreenLockManager::Test_GetRuntimeState(const std::string &event)
{
    int flag = -100;
    auto proxy = GetProxy();
    if (proxy  == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::Test_GetRuntimeState quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManager::Test_GetRuntimeState succeeded.  event=%{public}s", event.c_str());
    flag = proxy->Test_GetRuntimeState(event);
    return flag;
}

sptr<ScreenLockManagerInterface> ScreenLockManager::GetScreenLockManagerProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        SCLOCK_HILOGE("Getting SystemAbilityManager failed.");
        return nullptr;
    }
    auto systemAbility = systemAbilityManager->GetSystemAbility(SCREENLOCK_SERVICE_ID, "");
    if (systemAbility == nullptr) {
        SCLOCK_HILOGE("Get SystemAbility failed.");
        return nullptr;
    }
    deathRecipient_ = new ScreenLockSaDeathRecipient();
    systemAbility->AddDeathRecipient(deathRecipient_);
    sptr<ScreenLockManagerInterface> screenlockServiceProxy = iface_cast<ScreenLockManagerInterface>(systemAbility);
    if (screenlockServiceProxy == nullptr) {
        SCLOCK_HILOGE("Get ScreenLockManagerProxy from SA failed.");
        return nullptr;
    }
    SCLOCK_HILOGD("Getting ScreenLockManagerProxy succeeded.");
    return screenlockServiceProxy;
}

void ScreenLockManager::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> autoLock(managerProxyLock_);
    screenlockManagerProxy_ = GetScreenLockManagerProxy();
}

sptr<ScreenLockManagerInterface> ScreenLockManager::GetProxy()
{
    std::lock_guard<std::mutex> autoLock(managerProxyLock_);
    if (screenlockManagerProxy_ != nullptr) {
        return screenlockManagerProxy_;
    }
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGW("Redo GetScreenLockManagerProxy");
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    return screenlockManagerProxy_;
}

ScreenLockSaDeathRecipient::ScreenLockSaDeathRecipient()
{
}

void ScreenLockSaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    SCLOCK_HILOGE("ScreenLockSaDeathRecipient on remote systemAbility died.");
    ScreenLockManager::GetInstance()->OnRemoteSaDied(object);
}
} // namespace ScreenLock
} // namespace OHOS
