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

#include "strongauthmanager.h"
#include "screenlock_common.h"
#include "sclock_log.h"
#include "screenlock_system_ability.h"
#include "user_auth_client_callback.h"
#include "user_auth_client_impl.h"

namespace OHOS {
namespace ScreenLock {
std::mutex StrongAuthManger::instanceLock_;
sptr<StrongAuthManger> StrongAuthManger::instance_;
using namespace OHOS::UserIam::UserAuth;

// 强认证默认时间 3days
const std::int64_t DEFAULT_STRONG_AUTH_TIMEOUT_MS = 3 * 24 * 60 * 60 * 1000;

StrongAuthManger::StrongAuthManger() {}

StrongAuthManger::~StrongAuthManger() {}

StrongAuthManger::authTimer::authTimer() {}

StrongAuthManger::authTimer::authTimer(bool repeat, uint64_t interval, bool isExact, bool isIdle)
{
    this->repeat = repeat;
    this->interval = interval;
    this->type = TIMER_TYPE_WAKEUP;
    if (isExact) {
        this->type = TIMER_TYPE_WAKEUP + TIMER_TYPE_REALTIME;
    }
    if (isIdle) {
        this->type = TIMER_TYPE_IDLE;
    }
}

StrongAuthManger::authTimer::~authTimer() {}

void StrongAuthManger::authTimer::OnTrigger()
{
    SCLOCK_HILOGI("%{public}d, OnTrigger enter", userId_);
    if (callBack_) {
        callBack_(userId_);
    }
}

void StrongAuthManger::authTimer::SetType(const int &type)
{
    this->type = type;
}

void StrongAuthManger::authTimer::SetRepeat(bool repeat)
{
    this->repeat = repeat;
}

void StrongAuthManger::authTimer::SetInterval(const uint64_t &interval)
{
    this->interval = interval;
}

void StrongAuthManger::authTimer::SetWantAgent(std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent)
{
    this->wantAgent = wantAgent;
}

void StrongAuthManger::authTimer::SetCallbackInfo(const std::function<void(int32_t)> &callBack)
{
    this->callBack_ = callBack;
}

int32_t StrongAuthManger::authTimer::GetUserId()
{
    return userId_;
}

void StrongAuthManger::authTimer::SetUserId(int32_t userId)
{
    userId_ = userId;
}

static void StrongAuthTimerCallback(int32_t userId)
{
    SCLOCK_HILOGI("%{public}s, enter", __FUNCTION__);
    int32_t timerId = StrongAuthManger::GetInstance()->GetTimerId(userId);
    int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::AFTER_TIMEOUT);
    StrongAuthManger::GetInstance()->ResetStrongAuthTimer(userId);
    StrongAuthManger::GetInstance()->SetStrongAuthStat(userId, reasonFlag);
    ScreenLockSystemAbility::GetInstance()->StrongAuthChanged(userId, reasonFlag);
    return;
}

sptr<StrongAuthManger> StrongAuthManger::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new StrongAuthManger;
        }
    }
    return instance_;
}


int32_t StrongAuthManger::GetTimerId(int32_t userId)
{
    int32_t timerId = 0;
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter != strongAuthTimerInfo.end()) {
        timerId = iter->second;
    }
    return timerId;
}

void StrongAuthManger::RegistUserAuthSuccessEventListener()
{
    SCLOCK_HILOGD("RegistUserAuthSuccessEventListener start");
    std::vector<UserIam::UserAuth::AuthType> authTypeList;
    authTypeList.emplace_back(AuthType::PIN);
    authTypeList.emplace_back(AuthType::FACE);
    authTypeList.emplace_back(AuthType::FINGERPRINT);

    if (listener_ == nullptr) {
        sptr<UserIam::UserAuth::AuthEventListenerInterface> wrapper(new (std::nothrow) AuthEventListenerService());
        if (wrapper == nullptr) {
            SCLOCK_HILOGE("get listener failed");
            return;
        }
        listener_ = wrapper;
        int32_t ret = UserIam::UserAuth::UserAuthClientImpl::GetInstance().RegistUserAuthSuccessEventListener(
            authTypeList, listener_);
        SCLOCK_HILOGI("RegistUserAuthSuccessEventListener ret: %{public}d", ret);
    }

    return;
}

void StrongAuthManger::AuthEventListenerService::OnNotifyAuthSuccessEvent(int32_t userId,
    UserIam::UserAuth::AuthType authType, int32_t callerType, std::string &bundleName)
{
    SCLOCK_HILOGI("OnNotifyAuthSuccessEvent: %{public}d, %{public}d, %{public}s, callerType: %{public}d", userId,
        static_cast<int32_t>(authType), bundleName.c_str(), callerType);
    if (authType == AuthType::PIN) {
        StrongAuthManger::GetInstance()->SetStrongAuthStat(userId, static_cast<int32_t>(StrongAuthReasonFlags::NONE));
        StrongAuthManger::GetInstance()->ResetStrongAuthTimer(userId);
    }
    return;
}

void StrongAuthManger::UnRegistUserAuthSuccessEventListener()
{
    if (listener_ != nullptr) {
        int32_t ret =
            UserIam::UserAuth::UserAuthClientImpl::GetInstance().UnRegistUserAuthSuccessEventListener(listener_);
        SCLOCK_HILOGI("UnRegistUserAuthSuccessEventListener ret: %{public}d", ret);
    }
}

void StrongAuthManger::StartStrongAuthTimer(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    int timerId = GetTimerId(userId);
    if (timerId != 0) {
        SCLOCK_HILOGI("StrongAuthTimer exist. userId:%{public}d", userId);
        return;
    }

    std::shared_ptr<authTimer> timer = std::make_shared<authTimer>(true, DEFAULT_STRONG_AUTH_TIMEOUT_MS, true, false);
    timer->SetCallbackInfo(StrongAuthTimerCallback);
    timer->SetUserId(userId);
    timerId = MiscServices::TimeServiceClient::GetInstance()->CreateTimer(timer);
    int64_t currentTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(timerId, currentTime + DEFAULT_STRONG_AUTH_TIMEOUT_MS);
    strongAuthTimerInfo.insert(std::make_pair(userId, timerId));
    return;
}

void StrongAuthManger::ResetStrongAuthTimer(int32_t userId)
{
    int timerId = GetTimerId(userId);
    if (timerId == 0) {
        StartStrongAuthTimer(userId);
        return;
    }
    int64_t currentTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(timerId);
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(timerId, currentTime + DEFAULT_STRONG_AUTH_TIMEOUT_MS);
    return;
}

void StrongAuthManger::DestroyAllStrongAuthTimer()
{
    for (auto iter = strongAuthStateInfo.begin(); iter != strongAuthStateInfo.end(); ++iter) {
        DestroyStrongAuthTimer(iter->first);
    }
    return;
}

void StrongAuthManger::DestroyStrongAuthTimer(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    int timerId = GetTimerId(userId);
    if (timerId == 0) {
        return;
    }
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(timerId);
    MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(timerId);
    strongAuthTimerInfo.erase(userId);
    return;
}

void StrongAuthManger::SetStrongAuthStat(int32_t userId, int32_t reasonFlag)
{
    auto iter = strongAuthStateInfo.find(userId);
    if (iter != strongAuthStateInfo.end()) {
        iter->second = reasonFlag;
        SCLOCK_HILOGI("SetStrongAuthStat, reasonFlag:%{public}u", reasonFlag);
        return;
    }
    strongAuthStateInfo.insert(std::make_pair(userId, reasonFlag));
    return;
}

int32_t StrongAuthManger::GetStrongAuthStat(int32_t userId)
{
    int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::AFTER_BOOT);
    auto iter = strongAuthStateInfo.find(userId);
    if (iter != strongAuthStateInfo.end()) {
        reasonFlag = iter->second;
        SCLOCK_HILOGI("GetStrongAuthStat, reasonFlag:%{public}u", reasonFlag);
    }
    return reasonFlag;
}
} // namespace ScreenLock
} // namespace OHOS