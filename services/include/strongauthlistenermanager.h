/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SCREENLOCK_STRONGAUTH_LISTENER_MANAGER_H
#define SCREENLOCK_STRONGAUTH_LISTENER_MANAGER_H

#include <map>
#include <mutex>
#include <set>

#include "iremote_object.h"
#include "screenlock_common.h"
#include "screenlock_strongauth_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
using DeathRecipient = IRemoteObject::DeathRecipient;
class StrongAuthListenerManager {
public:
    static StrongAuthListenerManager &GetInstance();
    int32_t RegisterStrongAuthListener(const int32_t userId, const sptr<StrongAuthListenerInterface> &listener);
    int32_t UnRegisterStrongAuthListener(const sptr<StrongAuthListenerInterface> &listener);
    void OnStrongAuthChanged(int32_t userId, int32_t strongAuth);
    int32_t AddDeathRecipient(const sptr<StrongAuthListenerInterface> &listener);
    int32_t RemoveDeathRecipient(const sptr<StrongAuthListenerInterface> &listener);
    std::map<sptr<StrongAuthListenerInterface>, sptr<DeathRecipient>> GetDeathRecipientMap();

protected:
    class StrongAuthListenerDeathRecipient : public IRemoteObject::DeathRecipient, public NoCopyable {
    public:
        StrongAuthListenerDeathRecipient() = default;
        ~StrongAuthListenerDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };

    StrongAuthListenerManager() = default;
    ~StrongAuthListenerManager() = default;
    void AddStrongAuthListener(int32_t userId, const sptr<StrongAuthListenerInterface> &listener);
    void RemoveStrongAuthListener(int32_t userId, const sptr<StrongAuthListenerInterface> &listener);
    std::set<sptr<StrongAuthListenerInterface>> GetListenerSet(int32_t userId);
    std::recursive_mutex mutex_;
    std::map<int32_t, std::set<sptr<StrongAuthListenerInterface>>> eventListenerMap_;
    std::map<sptr<StrongAuthListenerInterface>, sptr<DeathRecipient>> deathRecipientMap_;

private:
    struct FinderSet {
        explicit FinderSet(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject)
        {
        }
        bool operator()(sptr<StrongAuthListenerInterface> listener)
        {
            return listener->AsObject() == remoteObject_;
        }
        sptr<IRemoteObject> remoteObject_ {nullptr};
    };

    struct FinderMap {
        explicit FinderMap(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject)
        {
        }
        bool operator()(std::map<sptr<StrongAuthListenerInterface>, sptr<DeathRecipient>>::value_type &pair)
        {
            return pair.first->AsObject() == remoteObject_;
        }
        sptr<IRemoteObject> remoteObject_ {nullptr};
    };
};
}
}
#endif // SCREENLOCK_STRONGAUTH_LISTENER_MANAGER_H