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
std::mutex ScreenLockManager::instanceLock_;
sptr<ScreenLockManager> ScreenLockManager::instance_;
sptr<ScreenLockSaDeathRecipient> ScreenLockManager::deathRecipient_;

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
            std::lock_guard<std::mutex> guard(instance_->managerProxyLock_);
            instance_->screenlockManagerProxy_ = GetScreenLockManagerProxy();
        }
    }
    return instance_;
}

int32_t ScreenLockManager::IsLocked(bool &isLocked)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("IsLocked quit because GetScreenLockManagerProxy failed.");
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    return proxy->IsLocked(isLocked);
}

bool ScreenLockManager::IsScreenLocked()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("IsScreenLocked quit because GetScreenLockManagerProxy failed.");
        return false;
    }
    return proxy->IsScreenLocked();
}

bool ScreenLockManager::GetSecure()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("GetSecure quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    return proxy->GetSecure();
}

int32_t ScreenLockManager::Unlock(Action action, const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("RequestUnlock quit because redoing GetScreenLockManagerProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return E_SCREENLOCK_NULLPTR;
    }
    StartAsyncTrace(HITRACE_TAG_MISC, "ScreenLockManager RequestUnlock start", HITRACE_UNLOCKSCREEN);
    if (action == Action::UNLOCKSCREEN) {
        return proxy->UnlockScreen(listener);
    }
    return proxy->Unlock(listener);
}

int32_t ScreenLockManager::Lock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("RequestLock quit because redoing GetScreenLockManagerProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return E_SCREENLOCK_NULLPTR;
    }
    SCLOCK_HILOGD("ScreenLockManager RequestLock succeeded.");
    return proxy->Lock(listener);
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
    if (screenlockManagerProxy_ != nullptr) {
        return screenlockManagerProxy_;
    }
    std::lock_guard<std::mutex> autoLock(managerProxyLock_);
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
