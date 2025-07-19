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
#ifndef IS_SO_CROP_H
#include <cinttypes>
#include "strongauthmanager.h"
#include "screenlock_common.h"
#include "sclock_log.h"
#include "screenlock_system_ability.h"
#include "user_auth_client_callback.h"
#include "user_idm_client.h"
#include "os_account_manager.h"
#include "innerlistenermanager.h"
#include "syspara/parameters.h"

namespace OHOS {
namespace ScreenLock {
std::mutex StrongAuthManger::instanceLock_;
sptr<StrongAuthManger> StrongAuthManger::instance_;
using namespace OHOS::UserIam::UserAuth;
using namespace OHOS::AccountSA;

StrongAuthManger::StrongAuthManger() {}

StrongAuthManger::~StrongAuthManger() {}

StrongAuthManger::authTimer::authTimer()
{
    userId_ = 0;
}

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
    userId_ = 0;
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
    int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::AFTER_TIMEOUT);
    StrongAuthManger::GetInstance()->SetStrongAuthStat(userId, reasonFlag);
    return;
}

static bool IsOsAccountUnlocked(int32_t osAccountId)
{
    bool isUnlocked = false;
    OHOS::ErrCode res = OHOS::AccountSA::OsAccountManager::IsOsAccountVerified(osAccountId, isUnlocked);
    if (res != OHOS::ERR_OK) {
        SCLOCK_HILOGE(" Check account verify status failed, res: %{public}d, accountId: %{public}d", res, osAccountId);
        return false;
    }
    SCLOCK_HILOGI(" account verified status: %{public}d, accountId: %{public}d", isUnlocked, osAccountId);
    return isUnlocked;
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

uint64_t StrongAuthManger::GetTimerId(int32_t userId)
{
    uint64_t timerId = 0;
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter != strongAuthTimerInfo.end()) {
        timerId = iter->second.timerId;
    }
    SCLOCK_HILOGI("GetTimerId, timerId: %{public}d", static_cast<int32_t>(timerId));
    return timerId;
}

void StrongAuthManger::RegistIamEventListener()
{
    SCLOCK_HILOGD("RegistIamEventListener start");
    if (OHOS::system::GetDeviceType() == "2in1") {
        SCLOCK_HILOGD("2in1 device no need to registCredChangeListener");
        return;
    }

    std::vector<UserIam::UserAuth::AuthType> authTypeList;
    authTypeList.emplace_back(AuthType::PIN);
    authTypeList.emplace_back(AuthType::FACE);
    authTypeList.emplace_back(AuthType::FINGERPRINT);

    if (credChangeListener_ == nullptr) {
        credChangeListener_ = std::make_shared<CredChangeListenerService>();
    }
    int32_t ret = UserIam::UserAuth::UserIdmClient::GetInstance().RegistCredChangeEventListener(
        authTypeList, credChangeListener_);
    SCLOCK_HILOGI("RegistCredChangeEventListener ret: %{public}d", ret);
}

void StrongAuthManger::RegistAuthEventListener()
{
    SCLOCK_HILOGD("RegistAuthEventListener start");
    std::vector<UserIam::UserAuth::AuthType> authTypeList;
    authTypeList.emplace_back(AuthType::PIN);
    authTypeList.emplace_back(AuthType::FACE);
    authTypeList.emplace_back(AuthType::FINGERPRINT);

    if (authSuccessListener_ == nullptr) {
        authSuccessListener_ = std::make_shared<AuthEventListenerService>();
    }
    int32_t ret = UserIam::UserAuth::UserAuthClient::GetInstance().RegistUserAuthSuccessEventListener(
        authTypeList, authSuccessListener_);
    SCLOCK_HILOGI("RegistUserAuthSuccessEventListener ret: %{public}d", ret);
}

void StrongAuthManger::AuthEventListenerService::OnNotifyAuthSuccessEvent(int32_t userId,
    UserIam::UserAuth::AuthType authType, int32_t callerType, const std::string &bundleName)
{
    SCLOCK_HILOGI("OnNotifyAuthSuccessEvent: %{public}d, %{public}d, %{public}s, callerType: %{public}d", userId,
        static_cast<int32_t>(authType), bundleName.c_str(), callerType);
    if (authType == AuthType::PIN) {
        StrongAuthManger::GetInstance()->SetStrongAuthStat(userId, static_cast<int32_t>(StrongAuthReasonFlags::NONE));
        int64_t triggerPeriod = StrongAuthManger::GetInstance()->GetStrongAuthTriggerPeriod(userId);
        StrongAuthManger::GetInstance()->ResetStrongAuthTimer(userId, triggerPeriod);
    }
    return;
}

void StrongAuthManger::CredChangeListenerService::OnNotifyCredChangeEvent(int32_t userId,
    UserIam::UserAuth::AuthType authType, UserIam::UserAuth::CredChangeEventType eventType, uint64_t credentialId)
{
    SCLOCK_HILOGI("OnNotifyCredChangeEvent: %{public}d, %{public}d, %{public}d, %{public}u", userId,
        static_cast<int32_t>(authType), eventType, static_cast<uint16_t>(credentialId));
    
    if (OHOS::system::GetDeviceType() == "2in1") {
        // PC只有无密码到有密码需要创建定时器
        if (authType == AuthType::PIN && eventType == ADD_CRED &&
            StrongAuthManger::GetInstance()->IsUserExitInStrongAuthInfo(userId) &&
            !StrongAuthManger::GetInstance()->IsUserHasStrongAuthTimer(userId)) {
            SCLOCK_HILOGD("2in1 device only deal add cred");
            StrongAuthManger::GetInstance()->StartStrongAuthTimer(userId);
        }
        return;
    }

    //新用户应该走切换重启创建定时器
    if (authType == AuthType::PIN && StrongAuthManger::GetInstance()->IsUserExitInStrongAuthInfo(userId)) {
        if (eventType == ADD_CRED || eventType == UPDATE_CRED) {
            StrongAuthManger::GetInstance()->ResetStrongAuthTimer(userId, CRED_CHANGE_FIRST_STRONG_AUTH_TIMEOUT_MS);
        } else if (eventType == DEL_USER) {
            StrongAuthManger::GetInstance()->SetStrongAuthStat(
                userId, static_cast<int32_t>(StrongAuthReasonFlags::NONE));
            StrongAuthManger::GetInstance()->DestroyStrongAuthTimer(userId);
        } else if (eventType == ENFORCE_DEL_USER) {
            StrongAuthManger::GetInstance()->DestroyStrongAuthStateInfo(userId);
            StrongAuthManger::GetInstance()->DestroyStrongAuthTimer(userId);
        }
    }
    return;
}

void StrongAuthManger::UnRegistIamEventListener()
{
    if (credChangeListener_ != nullptr) {
        int32_t ret = UserIam::UserAuth::
            UserIdmClient::GetInstance().UnRegistCredChangeEventListener(credChangeListener_);
        credChangeListener_ = nullptr;
        SCLOCK_HILOGI("UnRegistCredChangeEventListener ret: %{public}d", ret);
    }
}

void StrongAuthManger::UnRegistAuthEventListener()
{
    if (authSuccessListener_ != nullptr) {
        int32_t ret = UserIam::UserAuth::
            UserAuthClient::GetInstance().UnRegistUserAuthSuccessEventListener(authSuccessListener_);
        authSuccessListener_ = nullptr;
        SCLOCK_HILOGI("UnRegistUserAuthSuccessEventListener ret: %{public}d", ret);
    }
}

void StrongAuthManger::StartStrongAuthTimer(int32_t userId)
{
    StartStrongAuthTimer(userId, DEFAULT_STRONG_AUTH_TIMEOUT_MS);
}

void StrongAuthManger::StartStrongAuthTimer(int32_t userId, int64_t triggerPeriod)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    uint64_t timerId = GetTimerId(userId);
    if (timerId != 0) {
        SCLOCK_HILOGI("StrongAuthTimer exist. userId:%{public}d", userId);
        return;
    }

    SCLOCK_HILOGI("StartStrongAuthTimer triggerPeriod:%{public}lld", triggerPeriod);
    std::shared_ptr<authTimer> timer = std::make_shared<authTimer>(true, DEFAULT_STRONG_AUTH_TIMEOUT_MS, true, false);
    timer->SetCallbackInfo(StrongAuthTimerCallback);
    timer->SetUserId(userId);
    timerId = MiscServices::TimeServiceClient::GetInstance()->CreateTimer(timer);
    int64_t currentTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(timerId, currentTime + triggerPeriod);
    TimerInfo timerInfo = {
        .timerId = timerId,
        .triggerPeriod = triggerPeriod,
        .credChangeTimerStamp = currentTime,
    };
    strongAuthTimerInfo.insert(std::make_pair(userId, timerInfo));
    return;
}

void StrongAuthManger::ResetStrongAuthTimer(int32_t userId, int64_t triggerPeriod)
{
    SCLOCK_HILOGI("ResetStrongAuthTimer triggerPeriod:%{public}" PRId64, triggerPeriod);
    uint64_t timerId = GetTimerId(userId);
    if (timerId == 0) {
        StartStrongAuthTimer(userId, triggerPeriod);
        return;
    }
    int64_t currentTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(timerId);
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(timerId, currentTime + triggerPeriod);
    SetCredChangeTriggerPeriod(userId, triggerPeriod);
    return;
}

void StrongAuthManger::SetCredChangeTriggerPeriod(int32_t userId, int64_t triggerPeriod)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter == strongAuthTimerInfo.end()) {
        SCLOCK_HILOGI("SetCredChangeTriggerPeriod userId:%{public}d not exit", userId);
        return;
    }
    iter->second.triggerPeriod = triggerPeriod;
    iter->second.credChangeTimerStamp = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    return;
}

int64_t StrongAuthManger::GetStrongAuthTriggerPeriod(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter == strongAuthTimerInfo.end()) {
        SCLOCK_HILOGI("GetStrongAuthTriggerPeriod userId:%{public}d not exit", userId);
        return DEFAULT_STRONG_AUTH_TIMEOUT_MS;
    }
    int64_t currentTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    if (iter->second.triggerPeriod == CRED_CHANGE_FIRST_STRONG_AUTH_TIMEOUT_MS) {
        if (currentTime - iter->second.credChangeTimerStamp > CRED_CHANGE_FIRST_STRONG_AUTH_TIMEOUT_MS) {
            iter->second.triggerPeriod = CRED_CHANGE_SECOND_STRONG_AUTH_TIMEOUT_MS;
            iter->second.credChangeTimerStamp = currentTime;
            return iter->second.triggerPeriod;
        }
        return iter->second.triggerPeriod;
    }

    if (iter->second.triggerPeriod == CRED_CHANGE_SECOND_STRONG_AUTH_TIMEOUT_MS) {
        if (currentTime - iter->second.credChangeTimerStamp >
            CRED_CHANGE_SECOND_STRONG_AUTH_TIMEOUT_MS) {
            iter->second.triggerPeriod = DEFAULT_STRONG_AUTH_TIMEOUT_MS;
            return iter->second.triggerPeriod;
        }
        return iter->second.triggerPeriod;
    }
    return iter->second.triggerPeriod;
}

void StrongAuthManger::DestroyAllStrongAuthTimer()
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    for (auto iter = strongAuthTimerInfo.begin(); iter != strongAuthTimerInfo.end();) {
        uint64_t timerId = GetTimerId(iter->first);
        if (timerId != 0) {
            MiscServices::TimeServiceClient::GetInstance()->StopTimer(timerId);
            MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(timerId);
            iter = strongAuthTimerInfo.erase(iter);
        } else {
            ++iter;
        }
    }
    return;
}

void StrongAuthManger::DestroyStrongAuthTimer(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    uint64_t timerId = GetTimerId(userId);
    if (timerId == 0) {
        return;
    }
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(timerId);
    MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(timerId);
    strongAuthTimerInfo.erase(userId);
    SCLOCK_HILOGW("DestroyStrongAuthTimer lenth: %{public}d", static_cast<int32_t>(strongAuthTimerInfo.size()));
    return;
}

void StrongAuthManger::SetStrongAuthStat(int32_t userId, int32_t reasonFlag)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthStateInfo.find(userId);
    if (iter == strongAuthStateInfo.end()) {
        SCLOCK_HILOGI("SetStrongAuthStat fail, user not exit in strongAuthStateInfo");
        strongAuthStateInfo.insert(std::make_pair(userId, reasonFlag));
        NotifyStrongAuthChange(userId, reasonFlag);
        return;
    }

    auto oldFlag = iter->second;
    if (oldFlag == static_cast<int32_t>(StrongAuthReasonFlags::AFTER_BOOT) &&
        reasonFlag != static_cast<int32_t>(StrongAuthReasonFlags::NONE)) {
        // 重启强认证状态优先级最高
        SCLOCK_HILOGE("still after_boot, oldFlag:%{public}u, reasonFlag:%{public}u", oldFlag, reasonFlag);
        return;
    }

    iter->second = reasonFlag;
    lock.unlock();
    NotifyStrongAuthChange(userId, reasonFlag);
    SCLOCK_HILOGI("SetStrongAuthStat, oldFlag:%{public}u, reasonFlag:%{public}u", oldFlag, reasonFlag);
    return;
}

int32_t StrongAuthManger::GetStrongAuthStat(int32_t userId)
{
    std::lock_guard<std::mutex> lock(strongAuthTimerMutex);
    int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::AFTER_BOOT);
    auto iter = strongAuthStateInfo.find(userId);
    if (IsOsAccountUnlocked(userId) && iter != strongAuthStateInfo.end()) {
        reasonFlag = iter->second;
        SCLOCK_HILOGI("GetStrongAuthStat, reasonFlag:%{public}u", reasonFlag);
    }
    return reasonFlag;
}

int32_t StrongAuthManger::GetStrongAuthTimeTrigger(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter != strongAuthTimerInfo.end()) {
        return iter->second.triggerPeriod;
    }
    return 0;
}

void StrongAuthManger::AccountUnlocked(int32_t userId)
{
    NotifyStrongAuthChange(userId, static_cast<int32_t>(StrongAuthReasonFlags::NONE));
}

void StrongAuthManger::DestroyStrongAuthStateInfo(int32_t userId)
{
    std::lock_guard<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthStateInfo.find(userId);
    if (iter != strongAuthStateInfo.end()) {
        strongAuthStateInfo.erase(userId);
    }
    return;
}

void StrongAuthManger::InitStrongAuthStat(int32_t userId, int32_t reasonFlag)
{
    std::lock_guard<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthStateInfo.find(userId);
    if (iter == strongAuthStateInfo.end()) {
        strongAuthStateInfo.insert(std::make_pair(userId, reasonFlag));
        SCLOCK_HILOGW("InitStrongAuthStat, userId:%{public}u, reasonFlag:%{public}u", userId, reasonFlag);
        NotifyStrongAuthChange(userId, reasonFlag);
    }
    return;
}

bool StrongAuthManger::GetCredInfo(int32_t userId)
{
    std::unique_lock<std::mutex> lock(strongAuthTimerMutex);
    if (strongAuthStateInfo.find(userId) != strongAuthStateInfo.end()) {
        return true;
    }
    lock.unlock();

    auto getInfoCallback = std::make_shared<StrongAuthManger::StrongAuthGetSecurity>(userId);
    int32_t result = UserIdmClient::GetInstance().GetCredentialInfo(userId, AuthType::PIN, getInfoCallback);
    if (result != static_cast<int32_t>(ResultCode::SUCCESS)) {
        SCLOCK_HILOGE("GetCredentialInfo AuthType::PIN result = %{public}d", result);
    }
    return result;
}

void StrongAuthManger::StrongAuthGetSecurity::OnCredentialInfo(
    int32_t result, const std::vector<UserIam::UserAuth::CredentialInfo> &infoList)
{
    if (infoList.size() > 0 ||
        (result != UserIam::UserAuth::SUCCESS && result != UserIam::UserAuth::NOT_ENROLLED)) {
        SCLOCK_HILOGD("infoList.size() = %{public}zu", infoList.size());
        int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::AFTER_BOOT);
        StrongAuthManger::GetInstance()->InitStrongAuthStat(userId_, reasonFlag);
        // 重启后为72小时
        StrongAuthManger::GetInstance()->StartStrongAuthTimer(userId_);
    } else {
        int32_t reasonFlag = static_cast<int32_t>(StrongAuthReasonFlags::NONE);
        StrongAuthManger::GetInstance()->InitStrongAuthStat(userId_, reasonFlag);
    }
    return;
}

bool StrongAuthManger::IsUserExitInStrongAuthInfo(int32_t userId)
{
    std::lock_guard<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthStateInfo.find(userId);
    if (iter != strongAuthStateInfo.end()) {
        return true;
    }
    return false;
}

bool StrongAuthManger::IsUserHasStrongAuthTimer(int32_t userId)
{
    std::lock_guard<std::mutex> lock(strongAuthTimerMutex);
    auto iter = strongAuthTimerInfo.find(userId);
    if (iter != strongAuthTimerInfo.end()) {
        return true;
    }
    return false;
}

void StrongAuthManger::NotifyStrongAuthChange(int32_t userId, int32_t reasonFlag)
{
    if (reasonFlag == static_cast<int32_t>(StrongAuthReasonFlags::NONE)) {
        if (IsOsAccountUnlocked(userId)) {
            ScreenLockSystemAbility::GetInstance()->StrongAuthChanged(userId, reasonFlag);
            InnerListenerManager::GetInstance()->OnStrongAuthChanged(userId, reasonFlag);
        }
    } else {
        ScreenLockSystemAbility::GetInstance()->StrongAuthChanged(userId, reasonFlag);
        InnerListenerManager::GetInstance()->OnStrongAuthChanged(userId, reasonFlag);
    }
}
} // namespace ScreenLock
} // namespace OHOS
#endif // IS_SO_CROP_H