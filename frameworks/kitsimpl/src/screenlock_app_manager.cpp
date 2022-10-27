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

#include "screenlock_app_manager.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace ScreenLock {
std::mutex ScreenLockAppManager::instanceLock_;
sptr<ScreenLockAppManager> ScreenLockAppManager::instance_;
sptr<ScreenLockAppDeathRecipient> ScreenLockAppManager::deathRecipient_;
std::mutex ScreenLockAppManager::listenerLock_;
sptr<ScreenLockSystemAbilityInterface> ScreenLockAppManager::systemEventListener_;

ScreenLockAppManager::ScreenLockAppManager()
{
}

ScreenLockAppManager::~ScreenLockAppManager()
{
}

sptr<ScreenLockAppManager> ScreenLockAppManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new ScreenLockAppManager;
            std::lock_guard<std::mutex> guard(instance_->managerProxyLock_);
            instance_->screenlockManagerProxy_ = GetScreenLockManagerProxy();
        }
    }
    return instance_;
}

int32_t ScreenLockAppManager::SendScreenLockEvent(const std::string &event, int param)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockAppManager::SendScreenLockEvent quit because redoing GetScreenLockManagerProxy "
                      "failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    SCLOCK_HILOGD("ScreenLockAppManager::SendScreenLockEvent succeeded.");
    return proxy->SendScreenLockEvent(event, param);
}

int32_t ScreenLockAppManager::OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    SCLOCK_HILOGD("ScreenLockAppManager::OnSystemEvent in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockAppManager::OnSystemEvent quit because redoing GetScreenLockManagerProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return E_SCREENLOCK_NULLPTR;
    }
    listenerLock_.lock();
    systemEventListener_ = listener;
    listenerLock_.unlock();
    int32_t status = proxy->OnSystemEvent(listener);
    SCLOCK_HILOGD("ScreenLockAppManager::OnSystemEvent out, status=%{public}d", status);
    return status;
}

sptr<ScreenLockManagerInterface> ScreenLockAppManager::GetScreenLockManagerProxy()
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
    deathRecipient_ = new ScreenLockAppDeathRecipient();
    systemAbility->AddDeathRecipient(deathRecipient_);
    sptr<ScreenLockManagerInterface> screenlockServiceProxy = iface_cast<ScreenLockManagerInterface>(systemAbility);
    if (screenlockServiceProxy == nullptr) {
        SCLOCK_HILOGE("Get ScreenLockManagerProxy from SA failed.");
        return nullptr;
    }
    SCLOCK_HILOGD("Getting ScreenLockManagerProxy succeeded.");
    return screenlockServiceProxy;
}

void ScreenLockAppManager::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
{
    {
        std::lock_guard<std::mutex> autoLock(managerProxyLock_);
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    if (systemEventListener_ != nullptr) {
        SystemEvent systemEvent(SERVICE_RESTART);
        systemEventListener_->OnCallBack(systemEvent);
    }
}

sptr<ScreenLockManagerInterface> ScreenLockAppManager::GetProxy()
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

ScreenLockAppDeathRecipient::ScreenLockAppDeathRecipient()
{
}

void ScreenLockAppDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    SCLOCK_HILOGE("ScreenLockAppDeathRecipient on remote systemAbility died.");
    ScreenLockAppManager::GetInstance()->OnRemoteSaDied(object);
}
} // namespace ScreenLock
} // namespace OHOS
