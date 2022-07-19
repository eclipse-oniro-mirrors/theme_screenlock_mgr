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
#include "system_ability_definition.h"

namespace OHOS {
namespace ScreenLock {
std::mutex ScreenLockAppManager::instanceLock_;
sptr<ScreenLockAppManager> ScreenLockAppManager::instance_;
sptr<ScreenLockManagerInterface> ScreenLockAppManager::screenlockManagerProxy_;
sptr<ScreenLockAppDeathRecipient> ScreenLockAppManager::deathRecipient_;

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
            screenlockManagerProxy_ = GetScreenLockManagerProxy();
        }
    }
    return instance_;
}

bool ScreenLockAppManager::SendScreenLockEvent(const std::string &event, int param)
{
    bool flag = false;
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGW("Redo GetScreenLockManagerProxy");
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGE(
            "ScreenLockAppManager::SendScreenLockEvent quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    flag = screenlockManagerProxy_->SendScreenLockEvent(event, param);
    SCLOCK_HILOGD("ScreenLockAppManager::SendScreenLockEvent succeeded.");
    return flag;
}

bool ScreenLockAppManager::On(const sptr<ScreenLockSystemAbilityInterface> &listener, const std::string &type)
{
    SCLOCK_HILOGD("ScreenLockAppManager::On in");
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGW("Redo GetScreenLockManagerProxy");
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGE("ScreenLockAppManager::On quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr.");
        return false;
    }
    bool status = screenlockManagerProxy_->On(listener, type);
    SCLOCK_HILOGD("ScreenLockAppManager::On out, status=%{public}d", status);
    return status;
}

bool ScreenLockAppManager::Off(const std::string &type)
{
    SCLOCK_HILOGD("ScreenLockAppManager::Off in");
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGW("Redo GetScreenLockManagerProxy");
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGE("ScreenLockAppManager::Off quit because redoing GetScreenLockManagerProxy failed.");
        return false;
    }
    bool status = screenlockManagerProxy_->Off(type);
    SCLOCK_HILOGD("ScreenLockAppManager::Off out");
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
    screenlockManagerProxy_ = GetScreenLockManagerProxy();
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
