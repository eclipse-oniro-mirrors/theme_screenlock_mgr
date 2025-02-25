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
#include "screenlock_manager_proxy.h"
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
std::mutex ScreenLockManager::listenerLock_;
sptr<ScreenLockSystemAbilityInterface> ScreenLockManager::systemEventListener_;
ScreenLockManager::ScreenLockManager()
{
}

ScreenLockManager::~ScreenLockManager()
{
    SCLOCK_HILOGW("~ScreenLockManager");
    RemoveDeathRecipient();
}

sptr<ScreenLockManager> ScreenLockManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new ScreenLockManager;
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

int32_t ScreenLockManager::Unlock(Action action, const sptr<ScreenLockCallbackInterface> &listener)
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
    StartAsyncTrace(HITRACE_TAG_MISC, "ScreenLockManager Unlock start", HITRACE_UNLOCKSCREEN);
    int32_t ret = 0;
    if (action == Action::UNLOCKSCREEN) {
        ret = proxy->UnlockScreen(listener);
    } else {
        ret = proxy->Unlock(listener);
    }
    FinishAsyncTrace(HITRACE_TAG_MISC, "ScreenLockManager Unlock end", HITRACE_UNLOCKSCREEN);
    return ret;
}


int32_t ScreenLockManager::Lock(const sptr<ScreenLockCallbackInterface> &listener)
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

int32_t ScreenLockManager::Lock(int32_t userId)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    return proxy->Lock(userId);
}

int32_t ScreenLockManager::SendScreenLockEvent(const std::string &event, int param)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::SendScreenLockEvent quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int ret = proxy->SendScreenLockEvent(event, param);
    SCLOCK_HILOGD("SendScreenLockEvent result = %{public}d", ret);
    return ret;
}

int32_t ScreenLockManager::IsScreenLockDisabled(int userId, bool &isDisabled)
{
    SCLOCK_HILOGD("ScreenLockManager::IsScreenLockDisabled in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::IsScreenLockDisabled quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->IsScreenLockDisabled(userId, isDisabled);
    SCLOCK_HILOGD("ScreenLockManager::IsScreenLockDisabled out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::SetScreenLockDisabled(bool disable, int userId)
{
    SCLOCK_HILOGD("ScreenLockManager::SetScreenLockDisabled in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::SetScreenLockDisabled quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->SetScreenLockDisabled(disable, userId);
    SCLOCK_HILOGD("ScreenLockManager::SetScreenLockDisabled out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken)
{
    SCLOCK_HILOGD("ScreenLockManager::SetScreenLockAuthState in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::SetScreenLockAuthState quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->SetScreenLockAuthState(authState, userId, authToken);
    SCLOCK_HILOGD("ScreenLockManager::SetScreenLockAuthState out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::GetScreenLockAuthState(int userId, int32_t &authState)
{
    SCLOCK_HILOGD("ScreenLockManager::GetScreenLockAuthState in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::GetScreenLockAuthState quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->GetScreenLockAuthState(userId, authState);
    SCLOCK_HILOGD("ScreenLockManager::GetScreenLockAuthState out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::RequestStrongAuth(int reasonFlag, int32_t userId)
{
    SCLOCK_HILOGD("ScreenLockManager::RequestStrongAuth in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::RequestStrongAuth quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->RequestStrongAuth(reasonFlag, userId);
    SCLOCK_HILOGD("ScreenLockManager::RequestStrongAuth out, status=%{public}d", status);
    return status;
    return 0;
}

int32_t ScreenLockManager::GetStrongAuth(int userId, int32_t &reasonFlag)
{
    SCLOCK_HILOGD("ScreenLockManager::GetStrongAuth in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::GetStrongAuth quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->GetStrongAuth(userId, reasonFlag);
    SCLOCK_HILOGD("ScreenLockManager::GetStrongAuth out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::IsDeviceLocked(int userId, bool &isDeviceLocked)
{
    SCLOCK_HILOGD("ScreenLockManager::IsDeviceLocked in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::IsDeviceLocked quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t status = proxy->IsDeviceLocked(userId, isDeviceLocked);
    SCLOCK_HILOGD("ScreenLockManager::IsDeviceLocked out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::RegisterStrongAuthListener(const sptr<StrongAuthListener> &listener)
{
    SCLOCK_HILOGD("RegisterStrongAuthListener in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("RegisterStrongAuthListener quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }
    sptr<StrongAuthListenerWrapper> wrapper = new (std::nothrow) StrongAuthListenerWrapper(listener);
    if (wrapper == nullptr) {
        SCLOCK_HILOGE("Failed to create StrongAuthListenerWrapper.");
        return E_SCREENLOCK_NULLPTR;
    }

    std::lock_guard<std::mutex> lock(mWrapperMapMutex);
    // 检查是否已经存在对应的Wrapper
    if (mWrapperMap.find(listener) != mWrapperMap.end()) {
        SCLOCK_HILOGW("Wrapper already exists for this listener.");
        delete wrapper;
        return E_SCREENLOCK_NULLPTR;
    }
    mWrapperMap[listener] = wrapper;
    int32_t userId = listener->GetUserId();
    int32_t status = proxy->RegisterStrongAuthListener(userId, wrapper);
    SCLOCK_HILOGD("RegisterStrongAuthListener out, status=%{public}d", status);
    return status;
}

int32_t ScreenLockManager::UnRegisterStrongAuthListener(const sptr<StrongAuthListener> &listener)
{
    SCLOCK_HILOGD("UnRegisterStrongAuthListener in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("UnRegisterStrongAuthListener quit because redoing GetProxy failed.");
        return E_SCREENLOCK_NULLPTR;
    }

    std::lock_guard<std::mutex> lock(mWrapperMapMutex);
    auto it = mWrapperMap.find(listener);
    if (it == mWrapperMap.end()) {
        SCLOCK_HILOGW("No wrapper found for this listener.");
        return E_SCREENLOCK_NULLPTR;
    }
    sptr<StrongAuthListenerWrapper> wrapper = it->second;
    int32_t userId = listener->GetUserId();
    int32_t status = proxy->UnRegisterStrongAuthListener(userId, wrapper);
    SCLOCK_HILOGD("UnRegisterStrongAuthListener out, status=%{public}d", status);
    // 移除Wrapper对象
    mWrapperMap.erase(it);
    return status;
}

int32_t ScreenLockManager::OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    SCLOCK_HILOGD("ScreenLockManager::OnSystemEvent in");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        SCLOCK_HILOGE("ScreenLockManager::OnSystemEvent quit because redoing GetScreenLockManagerProxy failed.");
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
    SCLOCK_HILOGD("ScreenLockManager::OnSystemEvent out, status=%{public}d", status);
    return status;
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
    SCLOCK_HILOGE("ScreenLockDeathRecipient on remote systemAbility died.");
    std::lock_guard<std::mutex> autoLock(managerProxyLock_);
    screenlockManagerProxy_ = GetScreenLockManagerProxy();
    if (systemEventListener_ != nullptr) {
        SystemEvent systemEvent(SERVICE_RESTART);
        systemEventListener_->OnCallBack(systemEvent);
    }
}

sptr<ScreenLockManagerInterface> ScreenLockManager::GetProxy()
{
    std::lock_guard<std::mutex> autoLock(managerProxyLock_);
    if (screenlockManagerProxy_ == nullptr) {
        SCLOCK_HILOGW("Redo GetScreenLockManagerProxy");
        screenlockManagerProxy_ = GetScreenLockManagerProxy();
    }
    return screenlockManagerProxy_;
}

void ScreenLockManager::RemoveDeathRecipient()
{
    if (screenlockManagerProxy_ != nullptr && deathRecipient_ != nullptr) {
        screenlockManagerProxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
}
} // namespace ScreenLock
} // namespace OHOS
