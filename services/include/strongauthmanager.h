/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef SCREENLOCK_STRONG_AUTH_MANAGER_H
#define SCREENLOCK_STRONG_AUTH_MANAGER_H

#include <mutex>
#include <string>
#include <singleton.h>
#include <sys/time.h>
#include "iremote_object.h"
#include "refbase.h"
#include "screenlock_common.h"
#include "visibility.h"
#include "time_service_client.h"
#include "itimer_info.h"
#include "user_auth_event_listener_stub.h"

namespace OHOS {
namespace ScreenLock {
class StrongAuthManger : public RefBase {
public:
    SCREENLOCK_API static sptr<StrongAuthManger> GetInstance();

    StrongAuthManger();
    ~StrongAuthManger() override;

    uint64_t GetTimerId(int32_t userId);
    void StartStrongAuthTimer(int32_t userId);
    void DestroyStrongAuthTimer(int32_t userId);
    void DestroyAllStrongAuthTimer();
    void ResetStrongAuthTimer(int32_t userId);
    void SetStrongAuthStat(int32_t userId, int32_t reasonFlag);
    int32_t GetStrongAuthStat(int32_t userId);
    void RegistUserAuthSuccessEventListener();
    void UnRegistUserAuthSuccessEventListener();

public:

    class AuthEventListenerService : public UserIam::UserAuth::AuthEventListenerStub {
    public:
        AuthEventListenerService() = default;
        ~AuthEventListenerService() = default;
        void OnNotifyAuthSuccessEvent(int32_t userId, UserIam::UserAuth::AuthType authType, int32_t callerType,
                                   std::string &bundleName) override;
    };

    class authTimer : public MiscServices::ITimerInfo {
    public:
        authTimer();
        authTimer(bool repeat, uint64_t interval, bool isExact, bool isIdle = false);
        virtual ~authTimer();
        virtual void OnTrigger() override;
        virtual void SetType(const int &type) override;
        virtual void SetRepeat(bool repeat) override;
        virtual void SetInterval(const uint64_t &interval) override;
        virtual void SetWantAgent(std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent) override;
        void SetCallbackInfo(const std::function<void(int32_t)> &callBack);
        int32_t GetUserId();
        void SetUserId(int32_t userId);

    private:
        int32_t userId_ = 0;
        std::function<void(int32_t)> callBack_ = nullptr;
    };

private:
    std::mutex strongAuthTimerMutex;
    static std::mutex instanceLock_;
    static sptr<StrongAuthManger> instance_;
    std::map<int32_t, int32_t> strongAuthStateInfo;
    std::map<int32_t, uint64_t> strongAuthTimerInfo;
    sptr<UserIam::UserAuth::AuthEventListenerInterface> listener_;
    std::mutex strongAuthMutex_;
};
} // namespace OHOS
} // namespace ScreenLock
#endif // SCREENLOCK_STRONG_AUTH_MANAGER_H