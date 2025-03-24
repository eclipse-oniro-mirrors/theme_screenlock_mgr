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
#ifndef IS_SO_CROP_H

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
#include "user_auth_client.h"
#include "user_idm_client.h"

namespace OHOS {
namespace ScreenLock {
class StrongAuthManger : public RefBase {
public:
    SCREENLOCK_API static sptr<StrongAuthManger> GetInstance();

    StrongAuthManger();
    ~StrongAuthManger() override;

    uint64_t GetTimerId(int32_t userId);
    void StartStrongAuthTimer(int32_t userId);
    void ResetStrongAuthTimer(int32_t userId, int64_t triggerPeriod);
    void DestroyStrongAuthTimer(int32_t userId);
    void DestroyAllStrongAuthTimer();
    void SetStrongAuthStat(int32_t userId, int32_t reasonFlag);
    int32_t GetStrongAuthStat(int32_t userId);
    void RegistIamEventListener();
    void UnRegistIamEventListener();

public:

    class AuthEventListenerService : public UserIam::UserAuth::AuthSuccessEventListener {
    public:
        AuthEventListenerService() = default;
        virtual ~AuthEventListenerService() = default;
        void OnNotifyAuthSuccessEvent(int32_t userId, UserIam::UserAuth::AuthType authType, int32_t callerType,
            std::string &bundleName) override;
    };

    class CredChangeListenerService : public UserIam::UserAuth::CredChangeEventListener {
    public:
        CredChangeListenerService() = default;
        virtual ~CredChangeListenerService() = default;
        void OnNotifyCredChangeEvent(int32_t userId, UserIam::UserAuth::AuthType authType,
            UserIam::UserAuth::CredChangeEventType eventType, uint64_t credentialId) override;
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
    void StartStrongAuthTimer(int32_t userId, int64_t triggerPeriod);
    int64_t SetCredChangeTriggerPeriod(int32_t userId, int64_t triggerPeriod);
    int64_t GetStrongAuthTriggerPeriod(int32_t userId);

    struct TimerInfo {
        uint64_t timerId{0};
        int64_t triggerPeriod{-1};
        int64_t credChangeTimerStamp{-1};
    };

    std::mutex strongAuthTimerMutex;
    static std::mutex instanceLock_;
    static sptr<StrongAuthManger> instance_;
    std::map<int32_t, int32_t> strongAuthStateInfo;
    std::map<int32_t, TimerInfo> strongAuthTimerInfo;
    std::shared_ptr<UserIam::UserAuth::AuthSuccessEventListener> authSuccessListener_;
    std::shared_ptr<UserIam::UserAuth::CredChangeEventListener> credChangeListener_;
};
} // namespace OHOS
} // namespace ScreenLock
#endif // IS_SO_CROP_H
#endif // SCREENLOCK_STRONG_AUTH_MANAGER_H