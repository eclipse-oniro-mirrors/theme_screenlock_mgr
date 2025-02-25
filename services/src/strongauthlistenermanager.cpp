/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <sstream>
#include "sclock_log.h"
#include "strongauthlistenermanager.h"

namespace OHOS {
namespace ScreenLock {
using DeathRecipient = IRemoteObject::DeathRecipient;
const std::int64_t SUCCESS = 0;

#define IF_FALSE_LOGE_AND_RETURN_VAL(cond, retVal)      \
    do {                                                \
        if (!(cond)) {                                  \
            SCLOCK_HILOGE("(" #cond ") check fail, return"); \
            return (retVal);                            \
        }                                               \
    } while (0)

StrongAuthListenerManager &StrongAuthListenerManager::GetInstance()
{
    static StrongAuthListenerManager strongAuthListenerManager;
    return strongAuthListenerManager;
}

int32_t StrongAuthListenerManager::RegisterStrongAuthListener(const int32_t userId,
    const sptr<StrongAuthListenerInterface> &listener)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(listener != nullptr, E_SCREENLOCK_NULLPTR);

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int32_t result = AddDeathRecipient(listener);
    if (result != SUCCESS) {
        SCLOCK_HILOGE("AddDeathRecipient fail");
        return result;
    }

    AddStrongAuthListener(userId, listener);
    SCLOCK_HILOGI("RegistUserAuthSuccessEventListener success");
    return SUCCESS;
}

int32_t StrongAuthListenerManager::UnRegisterStrongAuthListener(const sptr<StrongAuthListenerInterface> &listener)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(listener != nullptr, E_SCREENLOCK_NULLPTR);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int32_t result = RemoveDeathRecipient(listener);
    if (result != SUCCESS) {
        SCLOCK_HILOGE("RemoveDeathRecipient fail");
        return result;
    }
    // Remove the listener from the map
    for (auto &pair : eventListenerMap_) {
        RemoveStrongAuthListener(pair.first, listener);
    }
    
    SCLOCK_HILOGI("UnRegistUserAuthSuccessEventListener success");
    return SUCCESS;
}

void StrongAuthListenerManager::AddStrongAuthListener(int32_t userId, const sptr<StrongAuthListenerInterface> &listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    SCLOCK_HILOGI("AddStrongAuthListener, userId:%{public}d", static_cast<int32_t>(userId));
    auto iter = std::find_if(eventListenerMap_[userId].begin(), eventListenerMap_[userId].end(),
        FinderSet(listener->AsObject()));
    if (iter != eventListenerMap_[userId].end()) {
        SCLOCK_HILOGE("listener is already registed");
        return;
    }
    eventListenerMap_[userId].insert(listener);
}

void StrongAuthListenerManager::RemoveStrongAuthListener(int32_t userId, const sptr<StrongAuthListenerInterface> &listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    SCLOCK_HILOGI("RemoveStrongAuthListener, userId:%{public}d", static_cast<int32_t>(userId));
    auto iter = std::find_if(eventListenerMap_[userId].begin(), eventListenerMap_[userId].end(),
        FinderSet(listener->AsObject()));
    if (iter == eventListenerMap_[userId].end()) {
        SCLOCK_HILOGE("listener is not registed");
        return;
    }
    eventListenerMap_[userId].erase(listener);
}

std::set<sptr<StrongAuthListenerInterface>> StrongAuthListenerManager::GetListenerSet(int32_t userId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::set<sptr<StrongAuthListenerInterface>> listenerSet(eventListenerMap_[userId]);
    return listenerSet;
}

void StrongAuthListenerManager::OnStrongAuthChanged(int32_t userId, int32_t strongAuth)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    SCLOCK_HILOGI("OnStrongAuthChanged enter.");
    std::set<sptr<StrongAuthListenerInterface>> listenerSetTemp = GetListenerSet(userId);
    for (auto &iter : listenerSetTemp) {
        if (iter != nullptr) {
            iter->OnStrongAuthChanged(userId, strongAuth);
            SCLOCK_HILOGI("OnStrongAuthChanged, userId: %{public}d, strongAuth: %{public}d", userId, strongAuth);
        }
    }
}

int32_t StrongAuthListenerManager::AddDeathRecipient(const sptr<StrongAuthListenerInterface> &listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    IF_FALSE_LOGE_AND_RETURN_VAL(listener != nullptr, E_SCREENLOCK_NULLPTR);

    auto obj = listener->AsObject();
    if (obj == nullptr) {
        SCLOCK_HILOGE("remote object is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto iter = std::find_if(deathRecipientMap_.begin(), deathRecipientMap_.end(), FinderMap(listener->AsObject()));
    if (iter != deathRecipientMap_.end()) {
        SCLOCK_HILOGE("deathRecipient is already registed");
        return SUCCESS;
    }

    sptr<DeathRecipient> dr(new (std::nothrow) StrongAuthListenerDeathRecipient());
    if ((dr == nullptr) || (!obj->AddDeathRecipient(dr))) {
        SCLOCK_HILOGE("AddDeathRecipient failed");
        return E_SCREENLOCK_NULLPTR;
    }

    deathRecipientMap_.emplace(listener, dr);
    SCLOCK_HILOGI("AddDeathRecipient success");
    return SUCCESS;
}

int32_t StrongAuthListenerManager::RemoveDeathRecipient(const sptr<StrongAuthListenerInterface> &listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    IF_FALSE_LOGE_AND_RETURN_VAL(listener != nullptr, E_SCREENLOCK_NULLPTR);

    auto obj = listener->AsObject();
    if (obj == nullptr) {
        SCLOCK_HILOGE("remote object is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto iter = std::find_if(deathRecipientMap_.begin(), deathRecipientMap_.end(), FinderMap(listener->AsObject()));
    if (iter == deathRecipientMap_.end()) {
        SCLOCK_HILOGE("deathRecipient is not registed");
        return SUCCESS;
    }

    sptr<DeathRecipient> deathRecipient = iter->second;
    if (deathRecipient == nullptr) {
        SCLOCK_HILOGE("deathRecipient is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    obj->RemoveDeathRecipient(deathRecipient);
    deathRecipientMap_.erase(listener);
    SCLOCK_HILOGE("RemoveDeathRecipient success");
    return SUCCESS;
}

std::map<sptr<StrongAuthListenerInterface>, sptr<DeathRecipient>> StrongAuthListenerManager::GetDeathRecipientMap()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return deathRecipientMap_;
}

void StrongAuthListenerManager::StrongAuthListenerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    SCLOCK_HILOGI("start");
    if (remote == nullptr) {
        SCLOCK_HILOGE("remote is nullptr");
        return;
    }

    std::map<sptr<StrongAuthListenerInterface>, sptr<DeathRecipient>> deathRecipientMap =
        StrongAuthListenerManager::GetInstance().GetDeathRecipientMap();
    for (auto &iter : deathRecipientMap) {
        if (iter.first != nullptr && remote == iter.first->AsObject()) {
            int32_t result = StrongAuthListenerManager::GetInstance().UnRegisterStrongAuthListener(iter.first);
            if (result != SUCCESS) {
                SCLOCK_HILOGE("UnRegisterStrongAuthListener fail");
                return;
            }
        }
    }
}

} // namespace OHOS
} // namespace ScreenLock