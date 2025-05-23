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
#include "screenlock_inner_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
using DeathRecipient = IRemoteObject::DeathRecipient;
class InnerListenerManager : public RefBase {
public:
    static sptr<InnerListenerManager> GetInstance();
    int32_t RegisterInnerListener(int32_t userId, const ListenType listenType,
                                  const sptr<InnerListenerIf> &listener);
    int32_t UnRegisterInnerListener(const ListenType listenType, const sptr<InnerListenerIf> &listener);
    int32_t AddDeathRecipient(const ListenType listenType, const sptr<InnerListenerIf> &listener);
    int32_t RemoveDeathRecipient(const sptr<InnerListenerIf> &listener);
    std::map<sptr<InnerListenerIf>, std::pair<ListenType, sptr<DeathRecipient>>> GetDeathRecipient();
    void OnStrongAuthChanged(int32_t userId, int32_t strongAuth);
    void OnDeviceLockStateChanged(int32_t userId, int32_t lockState);

protected:
    class InnerListenerDeathRecipient : public IRemoteObject::DeathRecipient, public NoCopyable {
    public:
        InnerListenerDeathRecipient() = default;
        ~InnerListenerDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };

    InnerListenerManager() = default;
    ~InnerListenerManager() = default;
    int32_t AddInnerListener(int32_t userId, const ListenType listenType,
                             const sptr<InnerListenerIf> &listener);
    int32_t RemoveInnerListener(const ListenType listenType, const sptr<InnerListenerIf> &listener);
    bool HasListenerSet(int32_t userId, ListenType listenType);
    std::recursive_mutex mutex_;
    std::map<sptr<InnerListenerIf>, std::pair<ListenType, sptr<DeathRecipient>>> deathRecipientMap_;
    std::map<ListenType, std::map<int32_t, std::set<sptr<InnerListenerIf>>>> innerListenMap_;

private:
    static std::mutex instanceLock_;
    static sptr<InnerListenerManager> instance_;
    std::set<sptr<InnerListenerIf>> getListenerSet(int32_t userId, ListenType listenType);
    void OnStateChanged(int32_t userId, int32_t lockState, ListenType listenType);
    struct FinderSet {
        explicit FinderSet(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject)
        {
        }
        bool operator()(sptr<InnerListenerIf> listener)
        {
            return listener->AsObject() == remoteObject_;
        }
        sptr<IRemoteObject> remoteObject_ {nullptr};
    };

    struct FinderMap {
        explicit FinderMap(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject)
        {
        }
        bool operator()(std::map<sptr<InnerListenerIf>, std::pair<ListenType, sptr<DeathRecipient>>>::value_type &pair)
        {
            return pair.first->AsObject() == remoteObject_;
        }
        sptr<IRemoteObject> remoteObject_ {nullptr};
    };
};
}
}
#endif // SCREENLOCK_STRONGAUTH_LISTENER_MANAGER_H