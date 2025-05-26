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
#include "innerlistenermanager.h"
#include "screenlock_common.h"
#include "common_helper.h"

namespace OHOS {
namespace ScreenLock {
using DeathRecipient = IRemoteObject::DeathRecipient;
std::mutex InnerListenerManager::instanceLock_;
sptr<InnerListenerManager> InnerListenerManager::instance_;

sptr<InnerListenerManager> InnerListenerManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(instanceLock_);
        if (instance_ == nullptr) {
            SCLOCK_HILOGI("InnerListenerManager create instance.");
            instance_ = new (std::nothrow) InnerListenerManager;
        }
    }
    return instance_;
}

int32_t InnerListenerManager::RegisterInnerListener(int32_t userId, const ListenType listenType,
                                                    const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    int32_t result = AddDeathRecipient(listenType, listener);
    if (result != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("AddDeathRecipient fail");
        return result;
    }

    if (userId == static_cast<int32_t>(SpecialUserId::USER_CURRENT)) {
        userId = GetUserIdFromCallingUid();
    }

    result = AddInnerListener(userId, listenType, listener);
    if (result != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("RegisterInnerListener fail");
        return result;
    }

    SCLOCK_HILOGI("RegisterInnerListener success");
    return E_SCREENLOCK_OK;
}

int32_t InnerListenerManager::UnRegisterInnerListener(const ListenType listenType,
                                                      const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    int32_t result = RemoveDeathRecipient(listener);
    if (result != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("RemoveDeathRecipient fail");
        return result;
    }
    // Remove the listener from the map
    result = RemoveInnerListener(listenType, listener);
    if (result != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("RemoveInnerListener fail");
        return result;
    }

    SCLOCK_HILOGI("UnRegisterInnerListener success");
    return E_SCREENLOCK_OK;
}

int32_t InnerListenerManager::AddInnerListener(int32_t userId, const ListenType listenType,
                                               const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    SCLOCK_HILOGI("AddInnerListener, userId:%{public}d, listenType:%{public}d",
                  userId, static_cast<int32_t>(listenType));
    if (innerListenMap_.find(listenType) == innerListenMap_.end()) {
        std::map<int32_t, std::set<sptr<InnerListenerIf>>> eventListenerMap;
        innerListenMap_.emplace(listenType, eventListenerMap);
    }

    if (innerListenMap_[listenType].find(userId) == innerListenMap_[listenType].end()) {
        std::set<sptr<InnerListenerIf>> listenSet;
        innerListenMap_[listenType].emplace(userId, listenSet);
    }

    auto iter = std::find_if(innerListenMap_[listenType][userId].begin(), innerListenMap_[listenType][userId].end(),
                             FinderSet(listener->AsObject()));
    if (iter != innerListenMap_[listenType][userId].end()) {
        SCLOCK_HILOGE("listener is already registed");
        return E_SCREENLOCK_OK;
    }
    innerListenMap_[listenType][userId].insert(listener);
    return E_SCREENLOCK_OK;
}

int32_t InnerListenerManager::RemoveInnerListener(const ListenType listenType,
                                                  const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto listenMapIter = innerListenMap_.find(listenType);
    if (listenMapIter == innerListenMap_.end()) {
        SCLOCK_HILOGE("RemoveInnerListener listenType not exit in innerListenMap_");
        return E_SCREENLOCK_OK;
    }

    for (auto& pair : innerListenMap_[listenType]) {
        int32_t userId = pair.first;
        auto iter = std::find_if(innerListenMap_[listenType][userId].begin(), innerListenMap_[listenType][userId].end(),
                                 FinderSet(listener->AsObject()));
        if (iter != innerListenMap_[listenType][userId].end()) {
            innerListenMap_[listenType][userId].erase(iter);
            auto length = static_cast<int>(innerListenMap_[listenType][userId].size());
            SCLOCK_HILOGI("Remove userId:%{public}d, length=%{public}d", static_cast<int32_t>(userId), length);
            return E_SCREENLOCK_OK;
        }
    }

    SCLOCK_HILOGI("listener not exit");
    return E_SCREENLOCK_OK;
}

bool InnerListenerManager::HasListenerSet(int32_t userId, ListenType listenType)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (innerListenMap_.find(listenType) == innerListenMap_.end()) {
        return false;
    }

    if (innerListenMap_[listenType].find(userId) == innerListenMap_[listenType].end()) {
        return false;
    }
    return true;
}

std::set<sptr<InnerListenerIf>> InnerListenerManager::getListenerSet(int32_t userId, ListenType listenType)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto itemList = innerListenMap_.find(listenType);
    if (itemList == innerListenMap_.end()) {
        return std::set<sptr<InnerListenerIf>>();
    }

    auto itemList1 = itemList.second.find(userId);
    if (itemList1 == innerListenMap_[listenType].end()) {
        return std::set<sptr<InnerListenerIf>>();
    }
    return itemList1.second;
}

void InnerListenerManager::OnStrongAuthChanged(int32_t userId, int32_t strongAuth)
{
    SCLOCK_HILOGI("OnStrongAuthChanged enter.");
    OnStateChanged(userId, strongAuth, ListenType::STRONG_AUTH);
}

void InnerListenerManager::OnDeviceLockStateChanged(int32_t userId, int32_t lockState)
{
    SCLOCK_HILOGI("OnDeviceLockStateChanged enter.");
    OnStateChanged(userId, lockState, ListenType::DEVICE_LOCK);
}

void InnerListenerManager::OnStateChanged(int32_t userId, int32_t state, ListenType listenType)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::set<sptr<InnerListenerIf>> listenerSetTemp = getListenerSet(userId, listenType);
    SCLOCK_HILOGI("OnStateChanged, userId=%{public}d, listenType=%{public}d, length=%{public}d, State=%{public}d",
        userId, static_cast<int>(listenType), static_cast<int>(listenerSetTemp.size()), state);
    for (auto &iter : listenerSetTemp) {
        if (iter != nullptr) {
            iter->OnStateChanged(userId, state);
        }
    }

    int32_t allUser = static_cast<int32_t>(SpecialUserId::USER_ALL);
    listenerSetTemp = getListenerSet(allUser, listenType);
    SCLOCK_HILOGI("OnStateChanged allUser, listenType=%{public}d, length=%{public}d, State=%{public}d",
        static_cast<int>(listenType), static_cast<int>(listenerSetTemp.size()), state);
    for (auto &iter : listenerSetTemp) {
        if (iter != nullptr) {
            iter->OnStateChanged(userId, state);
        }
    }
}

int32_t InnerListenerManager::AddDeathRecipient(const ListenType listenType,
                                                const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto obj = listener->AsObject();
    if (obj == nullptr) {
        SCLOCK_HILOGE("remote object is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto iter = std::find_if(deathRecipientMap_.begin(), deathRecipientMap_.end(), FinderMap(listener->AsObject()));
    if (iter != deathRecipientMap_.end()) {
        SCLOCK_HILOGE("deathRecipient is already registed");
        return E_SCREENLOCK_OK;
    }

    sptr<DeathRecipient> dr(new (std::nothrow) InnerListenerDeathRecipient());
    if ((dr == nullptr) || (!obj->AddDeathRecipient(dr))) {
        SCLOCK_HILOGE("AddDeathRecipient failed");
        return E_SCREENLOCK_NULLPTR;
    }

    deathRecipientMap_.emplace(listener, std::make_pair(listenType, dr));
    SCLOCK_HILOGI("AddDeathRecipient success length=%{public}d", static_cast<int>(deathRecipientMap_.size()));
    return E_SCREENLOCK_OK;
}

int32_t InnerListenerManager::RemoveDeathRecipient(const sptr<InnerListenerIf>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto obj = listener->AsObject();
    if (obj == nullptr) {
        SCLOCK_HILOGE("remote object is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    auto iter = std::find_if(deathRecipientMap_.begin(), deathRecipientMap_.end(), FinderMap(listener->AsObject()));
    if (iter == deathRecipientMap_.end()) {
        SCLOCK_HILOGE("deathRecipient is not registed");
        return E_SCREENLOCK_OK;
    }

    sptr<DeathRecipient> deathRecipient = iter->second.second;
    if (deathRecipient == nullptr) {
        SCLOCK_HILOGE("deathRecipient is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }

    obj->RemoveDeathRecipient(deathRecipient);
    deathRecipientMap_.erase(iter);
    SCLOCK_HILOGE("RemoveDeathRecipient success length=%{public}d", static_cast<int>(deathRecipientMap_.size()));
    return E_SCREENLOCK_OK;
}

std::map<sptr<InnerListenerIf>, std::pair<ListenType, sptr<DeathRecipient>>> InnerListenerManager::GetDeathRecipient()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return deathRecipientMap_;
}

void InnerListenerManager::InnerListenerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    SCLOCK_HILOGI("start");
    if (remote == nullptr) {
        SCLOCK_HILOGE("remote is nullptr");
        return;
    }

    std::map<sptr<InnerListenerIf>, std::pair<ListenType, sptr<DeathRecipient>>> deathRecipientMap =
        InnerListenerManager::GetInstance()->GetDeathRecipient();
    for (auto& iter : deathRecipientMap) {
        if (iter.first != nullptr && remote == iter.first->AsObject()) {
            SCLOCK_HILOGD("OnRemoteDied success");
            auto result = InnerListenerManager::GetInstance()->UnRegisterInnerListener(iter.second.first, iter.first);
            if (result != E_SCREENLOCK_OK) {
                SCLOCK_HILOGE("UnRegisterInnerListener fail");
                return;
            }
        }
    }
}

}  // namespace ScreenLock
}  // namespace OHOS