/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "screenlock_system_ability.h"

#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <memory>
#include <mutex>

#include "ability_manager_client.h"
#include "common_event_support.h"
#include "accesstoken_kit.h"
#include "common_event_manager.h"
#include "display_manager.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "parameter.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_get_info_callback.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "user_idm_client.h"
#include "want.h"
#include "window_manager.h"
#include "commeventsubscriber.h"
#include "user_auth_client_callback.h"
#include "user_auth_client_impl.h"
#include "innerlistenermanager.h"
#include "common_helper.h"
#ifndef IS_SO_CROP_H
#include "command.h"
#include "dump_helper.h"
#include "strongauthmanager.h"
#endif // IS_SO_CROP_H
#ifdef SUPPORT_WEAR_PAYMENT_APP
#include "watch_applock_manager.h"
#endif // SUPPORT_WEAR_PAYMENT_APP

using namespace OHOS;
using namespace OHOS::ScreenLock;

namespace OHOS {
namespace ScreenLock {
using namespace std;
using namespace OHOS::HiviewDFX;
using namespace OHOS::Rosen;
using namespace OHOS::UserIam::UserAuth;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::AccountSA;
REGISTER_SYSTEM_ABILITY_BY_ID(ScreenLockSystemAbility, SCREENLOCK_SERVICE_ID, true);
const std::int64_t TIME_OUT_MILLISECONDS = 10000L;
const std::int64_t INIT_INTERVAL = 5000000L;
const std::int64_t DELAY_TIME = 1000000L;
const char IAM_EVENT_KEY[] = "bootevent.useriam.fwkready";
std::mutex ScreenLockSystemAbility::instanceLock_;
std::mutex ScreenLockSystemAbility::queueLock_;
sptr<ScreenLockSystemAbility> ScreenLockSystemAbility::instance_;
constexpr int32_t MAX_RETRY_TIMES = 20;
std::shared_ptr<ffrt::queue> ScreenLockSystemAbility::queue_;

static int32_t GetCurrentActiveOsAccountId()
{
    std::vector<int> activatedOsAccountIds;
    OHOS::ErrCode res = OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (res != OHOS::ERR_OK || (activatedOsAccountIds.size() <= 0)) {
        SCLOCK_HILOGE("QueryActiveOsAccountIds fail. [Res]: %{public}d", res);
        return SCREEN_FAIL;
    }
    int osAccountId = activatedOsAccountIds[0];
    SCLOCK_HILOGI("GetCurrentActiveOsAccountId.[osAccountId]:%{public}d", osAccountId);
    return osAccountId;
}

void UserIamReadyCallback(const char *key, const char *value, void *context)
{
    if (key == nullptr || value == nullptr) {
        SCLOCK_HILOGE("SubscribeUserIamReady key or value is nullptr");
        return;
    }

    if (strcmp(key, IAM_EVENT_KEY) != 0) {
        SCLOCK_HILOGE("event key mismatch");
        return;
    }

    ScreenLockSystemAbility::GetInstance()->UserIamReadyNotify(value);
}

void AccountActive(const int lastUser, const int targetUser)
{
    SCLOCK_HILOGW("OnAccountsChanged.[osAccountId]:%{public}d, [lastId]:%{public}d", targetUser, lastUser);
    ScreenLockSystemAbility::GetInstance()->OnActiveUser(lastUser, targetUser);
    return;
}

void AcccountRemove(const int lastUser, const int targetUser)
{
    SCLOCK_HILOGW("AcccountRemove.[osAccountId]:%{public}d", targetUser);
    ScreenLockSystemAbility::GetInstance()->OnRemoveUser(targetUser);
    return;
}

void AccountUnlocked(const int lastUser, const int targetUser)
{
    SCLOCK_HILOGW("AccountUnlocked.[osAccountId]:%{public}d", targetUser);
#ifndef IS_SO_CROP_H
    StrongAuthManger::GetInstance()->AccountUnlocked(targetUser);
#endif // IS_SO_CROP_H
    return;
}

ScreenLockSystemAbility::ScreenLockSystemAbility(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate),
      state_(ServiceRunningState::STATE_NOT_START)
{
}

ScreenLockSystemAbility::~ScreenLockSystemAbility() {}

sptr<ScreenLockSystemAbility> ScreenLockSystemAbility::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            SCLOCK_HILOGI("ScreenLockSystemAbility create instance.");
            instance_ = new ScreenLockSystemAbility(SCREENLOCK_SERVICE_ID, true);
        }
    }
    return instance_;
}

ScreenLockSystemAbility::AccountSubscriber::AccountSubscriber(const OsAccountSubscribeInfo &subscribeInfo,
    const std::function<void(const int lastUser, const int targetUser)> &callback)
    : OsAccountSubscriber(subscribeInfo), callback_(callback)
{}

void ScreenLockSystemAbility::AccountSubscriber::OnAccountsChanged(const int &id)
{
    callback_(userId_, id);
    userId_ = id;
    return;
}

int32_t ScreenLockSystemAbility::Init()
{
    bool ret = Publish(ScreenLockSystemAbility::GetInstance());
    if (!ret) {
        SCLOCK_HILOGE("Publish ScreenLockSystemAbility failed.");
        return E_SCREENLOCK_PUBLISH_FAIL;
    }
    stateValue_.Reset();
    SCLOCK_HILOGI("Init ScreenLockSystemAbility success.");
    return ERR_OK;
}

void ScreenLockSystemAbility::OnStart()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::Enter OnStart.");
    {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = this;
        }
    }
    {
        std::lock_guard<std::mutex> runningStateLock(runningStateMutex_);
        if (state_ == ServiceRunningState::STATE_RUNNING) {
            SCLOCK_HILOGW("ScreenLockSystemAbility is already running.");
            return;
        }
    }
    InitServiceHandler();
    if (Init() != ERR_OK) {
        auto callback = [=]() { Init(); };
        {
            std::lock_guard<std::mutex> autoLock(queueLock_);
            if (queue_) {
                queue_->submit(callback, ffrt::task_attr().delay(INIT_INTERVAL));
            }
        }
        SCLOCK_HILOGW("ScreenLockSystemAbility Init failed. Try again 5s later");
    }
    AddSystemAbilityListener(DISPLAY_MANAGER_SERVICE_SA_ID);
    AddSystemAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN);
    AddSystemAbilityListener(SUBSYS_USERIAM_SYS_ABILITY_USERIDM);
    AddSystemAbilityListener(SUBSYS_USERIAM_SYS_ABILITY_USERAUTH);
    RegisterDumpCommand();
    return;
}

void ScreenLockSystemAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    SCLOCK_HILOGI("OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
    if (systemAbilityId == DISPLAY_MANAGER_SERVICE_SA_ID) {
        int times = 0;
        std::unique_lock<std::mutex> autoLock(instanceLock_);
        if (displayPowerEventListener_ == nullptr) {
            displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
        }
        autoLock.unlock();
        RegisterDisplayPowerEventListener(times);
    }
    if (systemAbilityId == SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN) {
        InitUserId();
    }
#ifndef IS_SO_CROP_H
    if (systemAbilityId == SUBSYS_USERIAM_SYS_ABILITY_USERIDM) {
        StrongAuthManger::GetInstance()->RegistIamEventListener();
    }

    if (systemAbilityId == SUBSYS_USERIAM_SYS_ABILITY_USERAUTH) {
        StrongAuthManger::GetInstance()->RegistAuthEventListener();
    }
#endif // IS_SO_CROP_H
}

void ScreenLockSystemAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
#ifndef IS_SO_CROP_H
    if (systemAbilityId == SUBSYS_USERIAM_SYS_ABILITY_USERIDM) {
        StrongAuthManger::GetInstance()->UnRegistIamEventListener();
    }

    if (systemAbilityId == SUBSYS_USERIAM_SYS_ABILITY_USERAUTH) {
        StrongAuthManger::GetInstance()->UnRegistAuthEventListener();
    }
#endif // IS_SO_CROP_H
}

void ScreenLockSystemAbility::RegisterDisplayPowerEventListener(int32_t times)
{
    times++;
    systemReady_ =
        (DisplayManager::GetInstance().RegisterDisplayPowerEventListener(displayPowerEventListener_) == DMError::DM_OK);
    if (systemReady_ == false && times <= MAX_RETRY_TIMES) {
        SCLOCK_HILOGW("RegisterDisplayPowerEventListener failed");
        auto callback = [this, times]() { RegisterDisplayPowerEventListener(times); };
        {
            std::lock_guard<std::mutex> autoLock(queueLock_);
            if (queue_) {
                queue_->submit(callback, ffrt::task_attr().delay(DELAY_TIME));
            }
        }
    } else if (systemReady_) {
        std::lock_guard<std::mutex> runningStateLock(runningStateMutex_);
        state_ = ServiceRunningState::STATE_RUNNING;
        SCLOCK_HILOGI("systemReady_ is true");
    }
    SCLOCK_HILOGI("RegisterDisplayPowerEventListener, times:%{public}d", times);
}

void ScreenLockSystemAbility::InitServiceHandler()
{
    std::lock_guard<std::mutex> autoLock(queueLock_);
    if (queue_ != nullptr) {
        SCLOCK_HILOGI("InitServiceHandler already init.");
        return;
    }
    queue_ = std::make_shared<ffrt::queue>("ScreenLockSystemAbility");
    SCLOCK_HILOGI("InitServiceHandler succeeded.");
}

void ScreenLockSystemAbility::OnRemoveUser(const int32_t userId)
{
    std::unique_lock<std::mutex> authLock(authStateMutex_);
    auto authIter = authStateInfo.find(userId);
    if (authIter != authStateInfo.end()) {
        authStateInfo.erase(authIter);
        SCLOCK_HILOGI("OnRemoveUser authStateInfo, userId: %{public}d", userId);
    } else {
        SCLOCK_HILOGI("OnRemoveUser authStateInfo user not exit, userId: %{public}d", userId);
    }
    authLock.unlock();

    std::lock_guard<std::mutex> screenStateLock(screenLockMutex_);
    auto lockIter = isScreenlockedMap_.find(userId);
    if (lockIter != isScreenlockedMap_.end()) {
        isScreenlockedMap_.erase(lockIter);
        SCLOCK_HILOGI("OnRemoveUser isScreenlockedMap, userId: %{public}d", userId);
    } else {
        SCLOCK_HILOGI("OnRemoveUser screenStateLock user not exit, userId: %{public}d", userId);
    }
}

void ScreenLockSystemAbility::OnActiveUser(const int lastUser, const int targetUser)
{
#ifndef IS_SO_CROP_H
    StrongAuthManger::GetInstance()->GetCredInfo(targetUser);
    // StrongAuthManger::GetInstance()->StartStrongAuthTimer(id);
#endif // IS_SO_CROP_H
    ScreenLockSystemAbility::GetInstance()->AuthStateInit(targetUser);
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    if (preferencesUtil->ObtainBool(std::to_string(targetUser), false)) {
        return;
    }
    preferencesUtil->SaveBool(std::to_string(targetUser), false);
    preferencesUtil->Refresh();
    return;
}

void ScreenLockSystemAbility::InitUserId()
{
    std::unique_lock<std::mutex> lock(accountSubscriberMutex_);
    accountSubscribers_[AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::ACTIVATED] =
        SubscribeAcccount(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::ACTIVATED, AccountActive);
    accountSubscribers_[AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::REMOVED] =
        SubscribeAcccount(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::REMOVED, AcccountRemove);
#ifndef IS_SO_CROP_H
    accountSubscribers_[AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::UNLOCKED] =
        SubscribeAcccount(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE::UNLOCKED, AccountUnlocked);
#endif // IS_SO_CROP_H
    lock.unlock();
    
    Singleton<CommeventMgr>::GetInstance().SubscribeEvent();
    SubscribeUserIamReady();

    int userId = GetCurrentActiveOsAccountId();
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return;
    }
    if (preferencesUtil->ObtainBool(std::to_string(userId), false)) {
        return;
    }
    preferencesUtil->SaveBool(std::to_string(userId), false);
    preferencesUtil->Refresh();
    return;
}

void ScreenLockSystemAbility::OnStop()
{
    SCLOCK_HILOGI("OnStop started.");
    {
        std::lock_guard<std::mutex> runningStateLock(runningStateMutex_);
        if (state_ != ServiceRunningState::STATE_RUNNING) {
            return;
        }
        state_ = ServiceRunningState::STATE_NOT_START;
    }
    {
        std::lock_guard<std::mutex> autoLock(queueLock_);
        queue_ = nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        instance_ = nullptr;
        if (displayPowerEventListener_ != nullptr) {
            DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(displayPowerEventListener_);
        }
    }
#ifndef IS_SO_CROP_H
    StrongAuthManger::GetInstance()->UnRegistIamEventListener();
    StrongAuthManger::GetInstance()->DestroyAllStrongAuthTimer();
#endif // IS_SO_CROP_H
    std::unique_lock<std::mutex> lock(accountSubscriberMutex_);
    for (auto iter = accountSubscribers_.begin(); iter != accountSubscribers_.end(); ++iter) {
        int ret = OsAccountManager::UnsubscribeOsAccount(iter->second);
        if (ret != SUCCESS) {
            SCLOCK_HILOGE(
                "unsubscribe os account failed, code=%{public}d, type=%{public}d", ret, static_cast<int>(iter->first));
        }
    }
    lock.unlock();
    RemoveSubscribeUserIamReady();
    SCLOCK_HILOGI("OnStop end.");
}

std::shared_ptr<ScreenLockSystemAbility::AccountSubscriber> ScreenLockSystemAbility::SubscribeAcccount(
    AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE subscribeType,
    const std::function<void(const int lastUser, const int targetUser)> &callback)
{
    AccountSA::OsAccountSubscribeInfo subscribeInfoActivate;
    subscribeInfoActivate.SetOsAccountSubscribeType(subscribeType);
    auto accountSubscriber = std::make_shared<AccountSubscriber>(subscribeInfoActivate, callback);

    int32_t ret = AccountSA::OsAccountManager::SubscribeOsAccount(accountSubscriber);
    if (ret != ERR_OK) {
        SCLOCK_HILOGE("SubscribeOsAccount activate failed.[ret]:%{public}d", ret);
    }
    return accountSubscriber;
}

sptr<ScreenLockSystemAbility> ScreenLockSystemAbility::getScreenLockSystemAbility()
{
    std::lock_guard<std::mutex> autoLock(instanceLock_);
    return sptr<ScreenLockSystemAbility>(instance_);
}

void ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener::OnDisplayPowerEvent(DisplayPowerEvent event,
                                                                                       EventStatus status)
{
    SCLOCK_HILOGI("OnDisplayPowerEvent event=%{public}d,status= %{public}d", static_cast<int>(event),
        static_cast<int>(status));
    sptr<ScreenLockSystemAbility> curInstance = getScreenLockSystemAbility();
    if (curInstance == nullptr) {
        SCLOCK_HILOGE("ScreenLockDisplayPowerEventListener instance_ nullptr");
        return;
    }
    switch (event) {
        case DisplayPowerEvent::WAKE_UP:
            curInstance->OnWakeUp(status);
            break;
        case DisplayPowerEvent::SLEEP:
            curInstance->OnSleep(status);
            break;
        case DisplayPowerEvent::DISPLAY_ON:
            curInstance->OnScreenOn(status);
            break;
        case DisplayPowerEvent::DISPLAY_OFF:
            curInstance->OnScreenOff(status);
            break;
        case DisplayPowerEvent::DESKTOP_READY:
            curInstance->OnExitAnimation();
            break;
        default:
            break;
    }
}

void ScreenLockSystemAbility::OnScreenOff(EventStatus status)
{
    SystemEvent systemEvent(BEGIN_SCREEN_OFF);
    if (status == EventStatus::BEGIN) {
        stateValue_.SetScreenState(static_cast<int32_t>(ScreenState::SCREEN_STATE_BEGIN_OFF));
    } else if (status == EventStatus::END) {
        stateValue_.SetScreenState(static_cast<int32_t>(ScreenState::SCREEN_STATE_END_OFF));
        systemEvent.eventType_ = END_SCREEN_OFF;
    }
    SystemEventCallBack(systemEvent);
}

void ScreenLockSystemAbility::OnScreenOn(EventStatus status)
{
    SystemEvent systemEvent(BEGIN_SCREEN_ON);
    if (status == EventStatus::BEGIN) {
        stateValue_.SetScreenState(static_cast<int32_t>(ScreenState::SCREEN_STATE_BEGIN_ON));
    } else if (status == EventStatus::END) {
        stateValue_.SetScreenState(static_cast<int32_t>(ScreenState::SCREEN_STATE_END_ON));
        systemEvent.eventType_ = END_SCREEN_ON;
    }
    SystemEventCallBack(systemEvent);
}

void ScreenLockSystemAbility::OnSystemReady()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnSystemReady started.");
    bool isExitFlag = false;
    int tryTime = 50;
    int minTryTime = 0;
    while (!isExitFlag && (tryTime > minTryTime)) {
        listenerMutex_.lock();
        if (systemEventListener_ != nullptr && systemReady_) {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnSystemReady started1.");
            SystemEvent systemEvent(SYSTEM_READY);
            systemEventListener_->OnCallBack(systemEvent);
            isExitFlag = true;
            listenerMutex_.unlock();
        } else {
            listenerMutex_.unlock();
            SCLOCK_HILOGE("ScreenLockSystemAbility OnSystemReady type not found., tryTime = %{public}d", tryTime);
            sleep(1);
        }
        --tryTime;
    }
}

void ScreenLockSystemAbility::OnWakeUp(EventStatus status)
{
    SystemEvent systemEvent(BEGIN_WAKEUP);
    if (status == EventStatus::BEGIN) {
        stateValue_.SetInteractiveState(static_cast<int32_t>(InteractiveState::INTERACTIVE_STATE_BEGIN_WAKEUP));
    } else if (status == EventStatus::END) {
        stateValue_.SetInteractiveState(static_cast<int32_t>(InteractiveState::INTERACTIVE_STATE_END_WAKEUP));
        systemEvent.eventType_ = END_WAKEUP;
    }
    SystemEventCallBack(systemEvent);
}

void ScreenLockSystemAbility::OnSleep(EventStatus status)
{
    SystemEvent systemEvent(BEGIN_SLEEP);
    if (status == EventStatus::BEGIN) {
        stateValue_.SetInteractiveState(static_cast<int32_t>(InteractiveState::INTERACTIVE_STATE_BEGIN_SLEEP));
    } else if (status == EventStatus::END) {
        stateValue_.SetInteractiveState(static_cast<int32_t>(InteractiveState::INTERACTIVE_STATE_END_SLEEP));
        systemEvent.eventType_ = END_SLEEP;
    }
    SystemEventCallBack(systemEvent);
}

void ScreenLockSystemAbility::OnExitAnimation()
{
    SystemEvent systemEvent(EXIT_ANIMATION);
    SystemEventCallBack(systemEvent);
}

void ScreenLockSystemAbility::StrongAuthChanged(int32_t userId, int32_t reasonFlag)
{
    if (stateValue_.GetCurrentUser() != userId) {
        return;
    }
    SystemEvent systemEvent(STRONG_AUTH_CHANGED);
    systemEvent.userId_ = userId;
    systemEvent.params_ = std::to_string(reasonFlag);
    SystemEventCallBack(systemEvent);
    SCLOCK_HILOGI("StrongAuthChanged: userId: %{public}d, reasonFlag:%{public}d", userId, reasonFlag);
}

int32_t ScreenLockSystemAbility::UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener)
{
    StartAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen begin", HITRACE_UNLOCKSCREEN);
    return UnlockInner(listener);
}

int32_t ScreenLockSystemAbility::Unlock(const sptr<ScreenLockCallbackInterface> &listener)
{
    StartAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen begin", HITRACE_UNLOCKSCREEN);
    if (!IsSystemApp()) {
        FinishAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen end, Calling app is not system app", HITRACE_UNLOCKSCREEN);
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    return UnlockInner(listener);
}

int32_t ScreenLockSystemAbility::UnlockInner(const sptr<ScreenLockCallbackInterface> &listener)
{
    {
        std::lock_guard<std::mutex> runningStateLock(runningStateMutex_);
        if (state_ != ServiceRunningState::STATE_RUNNING) {
            SCLOCK_HILOGI("UnlockScreen restart.");
        }
    }
    AccessTokenID callerTokenId = IPCSkeleton::GetCallingTokenID();
    // check whether the page of app request unlock is the focus page
    bool hasPermission = CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK");
    SCLOCK_HILOGE("hasPermission: %{public}d.", hasPermission);
    if (AccessTokenKit::GetTokenTypeFlag(callerTokenId) != TOKEN_NATIVE &&
        !IsAppInForeground(IPCSkeleton::GetCallingPid(), callerTokenId) && !hasPermission) {
        FinishAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen end, Unfocused", HITRACE_UNLOCKSCREEN);
        SCLOCK_HILOGE("UnlockScreen  Unfocused.");
        return E_SCREENLOCK_NOT_FOCUS_APP;
    }
#ifdef SUPPORT_WEAR_PAYMENT_APP
    int32_t userId = GetUserIdFromCallingUid();
    bool isScreenLocked = true;
    {
        std::lock_guard<std::mutex> slm(screenLockMutex_);
        auto iter = isScreenlockedMap_.find(userId);
        isScreenLocked = iter != isScreenlockedMap_.end() ? iter->second : true;
    }
    if (!isScreenLocked && WatchAppLockManager::GetInstance().IsPaymentApp()) {
        auto watchUnlockResult = WatchAppLockManager::GetInstance().unlockScreen(IsScreenLocked());
        if (watchUnlockResult != E_SCREENLOCK_OK) {
            FinishAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen end, watch", HITRACE_UNLOCKSCREEN);
            return watchUnlockResult;
        }
    }
#endif // SUPPORT_WEAR_PAYMENT_APP
    printCallerPid("UnlockInner");
    unlockListenerMutex_.lock();
    unlockVecListeners_.push_back(listener);
    unlockVecUserIds_.push_back(GetUserIdFromCallingUid());
    unlockListenerMutex_.unlock();
    SystemEvent systemEvent(UNLOCKSCREEN);
    SystemEventCallBack(systemEvent, HITRACE_UNLOCKSCREEN);
    FinishAsyncTrace(HITRACE_TAG_MISC, "UnlockScreen end", HITRACE_UNLOCKSCREEN);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::Lock(const sptr<ScreenLockCallbackInterface> &listener)
{
    if (!IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK_INNER")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }
    if (IsScreenLocked()) {
        SCLOCK_HILOGI("Currently in a locked screen state");
    }
    printCallerPid("Lock listener");
    lockListenerMutex_.lock();
    lockVecListeners_.push_back(listener);
    lockListenerMutex_.unlock();

    SystemEvent systemEvent(LOCKSCREEN);
    SystemEventCallBack(systemEvent, HITRACE_LOCKSCREEN);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::Lock(int32_t userId)
{
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK_INNER")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }
    if (IsScreenLocked()) {
        SCLOCK_HILOGI("Currently in a locked screen state");
    }
    printCallerPid("Lock userId");
    SystemEvent systemEvent(LOCKSCREEN);
    SystemEventCallBack(systemEvent, HITRACE_LOCKSCREEN);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::IsLocked(bool &isLocked)
{
    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_HAP && !IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    isLocked = IsScreenLocked();
    return E_SCREENLOCK_OK;
}

bool ScreenLockSystemAbility::IsScreenLocked()
{
    int32_t userId = stateValue_.GetCurrentUser();
    if (userId == USER_NULL || userId == 0) {
        userId = GetUserIdFromCallingUid();
    }
    std::lock_guard<std::mutex> slm(screenLockMutex_);
    auto iter = isScreenlockedMap_.find(userId);
#ifdef SUPPORT_WEAR_PAYMENT_APP
    if (WatchAppLockManager::GetInstance().IsPaymentApp()) {
        bool isScreenLocked = iter != isScreenlockedMap_.end() ? iter->second : true;
        return WatchAppLockManager::GetInstance().IsScreenLocked(isScreenLocked);
    }
#endif // SUPPORT_WEAR_PAYMENT_APP
    if (iter != isScreenlockedMap_.end()) {
        return iter->second;
    } else {
        SCLOCK_HILOGE("The IsScreenLocked is not set. userId=%{public}d, default screenLocked: true", userId);
        return true;
    }
}

int32_t ScreenLockSystemAbility::IsLockedWithUserId(int32_t userId, bool &isLocked)
{
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    std::lock_guard<std::mutex> slm(screenLockMutex_);
    auto iter = isScreenlockedMap_.find(userId);
    if (iter != isScreenlockedMap_.end()) {
        isLocked = iter->second;
        return E_SCREENLOCK_OK;
    } else {
        isLocked = true;
        SCLOCK_HILOGE("IsLockedWithUserId userId is not set. userId=%{public}d, default screenLocked: true", userId);
        return E_SCREENLOCK_USER_ID_INVALID;
    }
}

bool ScreenLockSystemAbility::GetSecure()
{
    {
        std::lock_guard<std::mutex> runningStateLock(runningStateMutex_);
        if (state_ != ServiceRunningState::STATE_RUNNING) {
            SCLOCK_HILOGI("ScreenLockSystemAbility GetSecure restart.");
        }
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility GetSecure started.");
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("ScreenLockSystemAbility::GetSecure callingUid=%{public}d", callingUid);
    int userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (userId == 0) {
        AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
    }
    SCLOCK_HILOGD("userId=%{public}d", userId);
    auto getInfoCallback = std::make_shared<ScreenLockGetInfoCallback>();
    int32_t result = UserIdmClient::GetInstance().GetCredentialInfo(userId, AuthType::PIN, getInfoCallback);
    SCLOCK_HILOGI("GetCredentialInfo AuthType::PIN result = %{public}d", result);
#ifdef SUPPORT_WEAR_PAYMENT_APP
    if (WatchAppLockManager::GetInstance().IsPaymentApp()) {
        return WatchAppLockManager::GetInstance().isSecureMode();
    }
#endif // SUPPORT_WEAR_PAYMENT_APP
    if (result == static_cast<int32_t>(ResultCode::SUCCESS)) {
        return true;
    }
    return false;
}

int32_t ScreenLockSystemAbility::OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    if (!IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK_INNER")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }
    std::uniuqe_lock<std::mutex> lck(listenerMutex_);
    systemEventListener_ = listener;
    lck.unlock();
    stateValue_.Reset();
    auto callback = [this]() { OnSystemReady(); };
    std::uniuqe_lock<std::mutex> queueLock(queueLock_);
    if (queue_ != nullptr) {
        queue_->submit(callback);
    }
    queueLock.unlock();
    int32_t userId = GetUserIdFromCallingUid();
    stateValue_.SetCurrentUser(userId);
    SCLOCK_HILOGI("ScreenLockSystemAbility::OnSystemEvent end.");
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::SendScreenLockEvent(const std::string &event, int param)
{
    SCLOCK_HILOGI("SendScreenLockEvent event=%{public}s ,param=%{public}d", event.c_str(), param);
    if (!IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK_INNER")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }
    int stateResult = param;
    if (event == UNLOCK_SCREEN_RESULT) {
        UnlockScreenEvent(stateResult);
    } else if (event == SCREEN_DRAWDONE) {
        NotifyDisplayEvent(DisplayEvent::KEYGUARD_DRAWN);
    } else if (event == LOCK_SCREEN_RESULT) {
        LockScreenEvent(stateResult);
    }
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::IsScreenLockDisabled(int userId, bool &isDisabled)
{
    SCLOCK_HILOGI("IsScreenLockDisabled userId=%{public}d", userId);
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        SCLOCK_HILOGE("no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    isDisabled = preferencesUtil->ObtainBool(std::to_string(userId), false);
    SCLOCK_HILOGI("IsScreenLockDisabled isDisabled=%{public}d", isDisabled);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::SetScreenLockDisabled(bool disable, int userId)
{
    SCLOCK_HILOGI("SetScreenLockDisabled disable=%{public}d ,param=%{public}d", disable, userId);
    if (GetCurrentActiveOsAccountId() != userId) {
        SCLOCK_HILOGE("it's not currentAccountId userId=%{public}d", userId);
        return SCREEN_FAIL;
    }
    if (GetSecure() == true) {
        SCLOCK_HILOGE("The screen lock password has been set.");
        return SCREEN_FAIL;
    }
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        SCLOCK_HILOGE("no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        SCLOCK_HILOGE("preferencesUtil is nullptr!");
        return E_SCREENLOCK_NULLPTR;
    }
    preferencesUtil->SaveBool(std::to_string(userId), disable);
    preferencesUtil->Refresh();
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken)
{
    SCLOCK_HILOGI("SetScreenLockAuthState authState=%{public}d ,userId=%{public}d", authState, userId);
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    
    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK_INNER")) {
        SCLOCK_HILOGE("no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    std::unique_lock<std::mutex> lock(authStateMutex_);
    auto iter = authStateInfo.find(userId);
    if (iter != authStateInfo.end()) {
        bool nextState = GetDeviceLockedStateByAuth(authState);
        bool curState = GetDeviceLockedStateByAuth(iter->second);
        if (nextState != curState) {
            InnerListenerManager::GetInstance()->OnDeviceLockStateChanged(userId, static_cast<int32_t>(nextState));
        }
        iter->second = authState;
        return E_SCREENLOCK_OK;
    }
    authStateInfo.insert(std::make_pair(userId, authState));
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::GetScreenLockAuthState(int userId, int32_t &authState)
{
    SCLOCK_HILOGD("GetScreenLockAuthState userId=%{public}d", userId);
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }

    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        SCLOCK_HILOGE("no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    std::unique_lock<std::mutex> lock(authStateMutex_);
    auto iter = authStateInfo.find(userId);
    if (iter != authStateInfo.end()) {
        authState = iter->second;
        return E_SCREENLOCK_OK;
    }
    authState = static_cast<int32_t>(AuthState::UNAUTH);
    SCLOCK_HILOGI("The authentication status is not set. userId=%{public}d", userId);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::RequestStrongAuth(int reasonFlag, int32_t userId)
{
#ifdef IS_SO_CROP_H
    return E_SCREENLOCK_OK;
#else
    SCLOCK_HILOGI("RequestStrongAuth reasonFlag=%{public}d ,userId=%{public}d", reasonFlag, userId);
    printCallerPid("RequestStrongAuth");
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }

    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        SCLOCK_HILOGE("no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    StrongAuthManger::GetInstance()->SetStrongAuthStat(userId, reasonFlag);
    return E_SCREENLOCK_OK;
#endif // IS_SO_CROP_H
}

int32_t ScreenLockSystemAbility::GetStrongAuth(int userId, int32_t &reasonFlag)
{
#ifdef IS_SO_CROP_H
    reasonFlag = 0;
    return E_SCREENLOCK_OK;
#else
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }

    if (!CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        SCLOCK_HILOGE("GetStrongAuth no permission: userId=%{public}d", userId);
        return E_SCREENLOCK_NO_PERMISSION;
    }
    reasonFlag = StrongAuthManger::GetInstance()->GetStrongAuthStat(userId);
    SCLOCK_HILOGI("GetStrongAuth userId=%{public}d, reasonFlag=%{public}d", userId, reasonFlag);
    return E_SCREENLOCK_OK;
#endif // IS_SO_CROP_H
}

int32_t ScreenLockSystemAbility::RegisterInnerListener(const int32_t userId, const ListenType listenType,
                                                       const sptr<InnerListenerIf> &listener)
{
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }

    if (listenType == ListenType::STRONG_AUTH && !CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }

    return InnerListenerManager::GetInstance()->RegisterInnerListener(userId, listenType, listener);
}

int32_t ScreenLockSystemAbility::UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
                                                         const sptr<InnerListenerIf> &listener)
{
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }

    if (listenType == ListenType::STRONG_AUTH && !CheckPermission("ohos.permission.ACCESS_SCREEN_LOCK")) {
        return E_SCREENLOCK_NO_PERMISSION;
    }

    return InnerListenerManager::GetInstance()->UnRegisterInnerListener(listenType, listener);
}

void ScreenLockSystemAbility::SetScreenlocked(bool isScreenlocked, const int32_t userId)
{
    SCLOCK_HILOGI("SetScreenlocked state:%{public}d, userId:%{public}d", isScreenlocked, userId);
    std::lock_guard<std::mutex> slm(screenLockMutex_);
    auto iter = isScreenlockedMap_.find(userId);
    if (iter != isScreenlockedMap_.end()) {
        iter->second = isScreenlocked;
    } else {
        isScreenlockedMap_.insert(std::make_pair(userId, isScreenlocked));
    }
}

int32_t ScreenLockSystemAbility::IsDeviceLocked(int userId, bool &isDeviceLocked)
{
    if (CheckSystemPermission()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    std::unique_lock<std::mutex> lock(authStateMutex_);
    auto iter = authStateInfo.find(userId);
    if (iter != authStateInfo.end()) {
        int32_t authState = iter->second;
        isDeviceLocked = GetDeviceLockedStateByAuth(authState);
        SCLOCK_HILOGI("IsDeviceLocked. userId=%{public}d, isDeviceLocked=%{public}d", userId, isDeviceLocked);
        return E_SCREENLOCK_OK;
    } else {
        isDeviceLocked = true;
        SCLOCK_HILOGI("user not set. userId=%{public}d, isDeviceLocked=%{public}d", userId, isDeviceLocked);
        return E_SCREENLOCK_USER_ID_INVALID;
    }
}

void StateValue::Reset()
{
    screenlockEnabled_ = true;
    currentUser_ = USER_NULL;
}

int ScreenLockSystemAbility::Dump(int fd, const std::vector<std::u16string> &args)
{
#ifdef IS_SO_CROP_H
    return ERR_OK;
#else
    int uid = static_cast<int>(IPCSkeleton::GetCallingUid());
    const int maxUid = 10000;
    if (uid > maxUid) {
        return 0;
    }

    std::vector<std::string> argsStr;
    for (auto item : args) {
        argsStr.emplace_back(Str16ToStr8(item));
    }

    DumpHelper::GetInstance().Dispatch(fd, argsStr);
    return ERR_OK;
#endif  // IS_SO_CROP_H
}

void ScreenLockSystemAbility::RegisterDumpCommand()
{
#ifdef IS_SO_CROP_H
    return;
#else
    auto cmd = std::make_shared<Command>(std::vector<std::string>{ "-all" }, "dump all screenlock information",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            AppendPrintOtherInfo(output);
            std::unique_lock<std::mutex> authStateLock(authStateMutex_);
            for (auto iter = authStateInfo.begin(); iter != authStateInfo.end(); iter++) {
                int32_t userId = iter->first;
                bool deviceLocked = GetDeviceLockedStateByAuth(iter->second);
                string temp_deviceLocked = "";
                deviceLocked ? temp_deviceLocked = "true" : temp_deviceLocked = "false";
                string temp_userId = std::to_string(static_cast<int>(userId));
                string temp_authState = std::to_string(static_cast<int>(iter->second));
                output.append(" * deviceLocked  \t\t" + temp_deviceLocked + "\t\t" + temp_userId + "\n");
                output.append(" * authState  \t\t" + temp_authState + "\t\t" + temp_userId + "\n");
            }
            authStateLock.unlock();

            std::vector<int32_t> userIdArray;
            {
                std::lock_guard<std::mutex> screenLockedLock(screenLockMutex_);
                for (auto iter = isScreenlockedMap_.begin(); iter != isScreenlockedMap_.end(); iter++) {
                    int32_t userId = iter->first;
                    userIdArray.push_back(userId);
                    bool isLocked = iter->second;
                    string temp_screenLocked = "";
                    isLocked ? temp_screenLocked = "true" : temp_screenLocked = "false";
                    string temp_userId = std::to_string(static_cast<int>(userId));
                    output.append(" * screenLocked  \t\t" + temp_screenLocked + "\t\t" + temp_userId + "\n");
                }
            }

            for (auto iter = userIdArray.begin(); iter != userIdArray.end(); ++iter) {
                auto reasonFlag = StrongAuthManger::GetInstance()->GetStrongAuthStat(*iter);
                auto timeTrigger = StrongAuthManger::GetInstance()->GetStrongAuthTimeTrigger(*iter);
                string temp_userId = std::to_string(static_cast<int>(*iter));
                string  temp_reasonFlag = std::to_string(static_cast<int>(reasonFlag));
                string temp_timerTrigger = std::to_string(static_cast<int>(timeTrigger));
                output.append(
                    " * strongAuth  \t\t" + temp_userId + "\t\t" + temp_reasonFlag + "\t\t" + temp_timerTrigger + "\n");
            }
            return true;
        });
    DumpHelper::GetInstance().RegisterCommand(cmd);
#endif // IS_SO_CROP_H
}

void ScreenLockSystemAbility::AppendPrintOtherInfo(std::string &output)
{
    bool screenState = stateValue_.GetScreenState();
    int32_t offReason = stateValue_.GetOffReason();
    int32_t interactiveState = stateValue_.GetInteractiveState();
    string temp_screenState = "";
    screenState ? temp_screenState = "true" : temp_screenState = "false";
    output.append("\n Screenlock system state\\tValue\\t\\tDescription\n")
        .append(" * screenState  \t\t" + temp_screenState + "\t\tscreen on / off status\n")
        .append(" * offReason  \t\t\t" + std::to_string(offReason) + "\t\tscreen failure reason\n")
        .append(" * interactiveState \t\t" + std::to_string(interactiveState) + "\t\tscreen interaction status\n");
}

void ScreenLockSystemAbility::PublishEvent(const std::string &eventAction, const int32_t userId)
{
    AAFwk::Want want;
    want.SetAction(eventAction);
    want.SetParam("userId", userId);
    EventFwk::CommonEventData commonData(want);
    bool ret = EventFwk::CommonEventManager::PublishCommonEvent(commonData);
    SCLOCK_HILOGD("Publish event result is:%{public}d", ret);
}

void ScreenLockSystemAbility::LockScreenEvent(int stateResult)
{
    SCLOCK_HILOGD("ScreenLockSystemAbility LockScreenEvent stateResult:%{public}d", stateResult);
    int32_t userId = 0;
    if (stateResult == ScreenChange::SCREEN_SUCC) {
        userId = GetUserIdFromCallingUid();
        SetScreenlocked(true, userId);
    }
    std::lock_guard<std::mutex> autoLock(lockListenerMutex_);
    if (lockVecListeners_.size()) {
        auto callback = [this, stateResult]() {
            std::lock_guard<std::mutex> guard(lockListenerMutex_);
            for (size_t i = 0; i < lockVecListeners_.size(); i++) {
                lockVecListeners_[i]->OnCallBack(stateResult);
            }
            lockVecListeners_.clear();
        };
        ffrt::submit(callback);
    }
    if (stateResult == ScreenChange::SCREEN_SUCC) {
        PublishEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED, userId);
    }
}

void ScreenLockSystemAbility::UnlockScreenEvent(int stateResult)
{
    SCLOCK_HILOGD("ScreenLockSystemAbility UnlockScreenEvent stateResult:%{public}d", stateResult);
    if (stateResult == ScreenChange::ALREADY_UNLOCKED) {
        NotifyUnlockListener(ScreenChange::SCREEN_SUCC);
        return;
    }
    if (stateResult == ScreenChange::EARLY_SUCCESS) {
        SetScreenlocked(false, GetUserIdFromCallingUid());
        return;
    }
    if (stateResult == ScreenChange::SCREEN_SUCC) {
        int32_t userId = GetUserIdFromCallingUid();
        SetScreenlocked(false, userId);
        NotifyDisplayEvent(DisplayEvent::UNLOCK);
        PublishEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED, userId);
    }

    if (stateResult != ScreenChange::SCREEN_FAIL) {
        NotifyUnlockListener(stateResult);
    }
}

void ScreenLockSystemAbility::SystemEventCallBack(const SystemEvent &systemEvent, TraceTaskId traceTaskId)
{
    SCLOCK_HILOGI("eventType is %{public}s, params is %{public}s", systemEvent.eventType_.c_str(),
                  systemEvent.params_.c_str());
    {
        std::lock_guard<std::mutex> lck(listenerMutex_);
        if (systemEventListener_ == nullptr) {
            SCLOCK_HILOGE("systemEventListener_ is nullptr.");
            return;
        }
    }

    if (traceTaskId != HITRACE_BUTT) {
        StartAsyncTrace(
            HITRACE_TAG_MISC, "ScreenLockSystemAbility::" + systemEvent.eventType_ + "begin callback", traceTaskId);
    }
    {
        std::lock_guard<std::mutex> lck(listenerMutex_);
        if (systemEventListener_ != nullptr) {
            systemEventListener_->OnCallBack(systemEvent);
        }
    }
    if (traceTaskId != HITRACE_BUTT) {
        FinishAsyncTrace(
            HITRACE_TAG_MISC, "ScreenLockSystemAbility::" + systemEvent.eventType_ + "end callback", traceTaskId);
    }
}

void ScreenLockSystemAbility::NotifyUnlockListener(const int32_t screenLockResult)
{
    int curUserId = GetUserIdFromCallingUid();
    std::lock_guard<std::mutex> autoLock(unlockListenerMutex_);
    if (unlockVecListeners_.size()) {
        auto callback = [this, screenLockResult, curUserId]() {
            std::lock_guard<std::mutex> guard(unlockListenerMutex_);
            for (size_t i = 0; i < unlockVecListeners_.size(); i++) {
                unlockVecListeners_[i]->OnCallBack(unlockVecUserIds_[i] == curUserId ? screenLockResult :
                    ScreenChange::SCREEN_FAIL);
            }
            unlockVecListeners_.clear();
            unlockVecUserIds_.clear();
        };
        ffrt::submit(callback);
    }
}

void ScreenLockSystemAbility::NotifyDisplayEvent(DisplayEvent event)
{
    std::lock_guard<std::mutex> autoLock(queueLock_);
    if (queue_ == nullptr) {
        SCLOCK_HILOGE("NotifyDisplayEvent queue_ is nullptr.");
        return;
    }
    auto callback = [event]() { DisplayManager::GetInstance().NotifyDisplayEvent(event); };
    queue_->submit(callback);
}

void ScreenLockSystemAbility::ResetFfrtQueue()
{
    std::lock_guard<std::mutex> autoLock(queueLock_);
    queue_.reset();
}

bool ScreenLockSystemAbility::IsAppInForeground(int32_t callingPid, uint32_t callingTokenId)
{
#ifdef CONFIG_FACTORY_MODE
    return true;
#endif
    FocusChangeInfo focusInfo;
    WindowManager::GetInstance().GetFocusWindowInfo(focusInfo);
    if (callingPid == focusInfo.pid_) {
        return true;
    }
    bool isFocused = false;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = AAFwk::AbilityManagerClient::GetInstance()->CheckUIExtensionIsFocused(callingTokenId, isFocused);
    IPCSkeleton::SetCallingIdentity(identity);
    SCLOCK_HILOGI("tokenId:%{public}d check result:%{public}d, isFocused:%{public}d", callingTokenId, ret, isFocused);
    return ret == ERR_OK && isFocused;
}

bool ScreenLockSystemAbility::IsSystemApp()
{
    return TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID());
}

bool ScreenLockSystemAbility::CheckPermission(const std::string &permissionName)
{
    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    int result = AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (result != PERMISSION_GRANTED) {
        SCLOCK_HILOGE("check permission failed.");
        return false;
    }
    return true;
}

bool ScreenLockSystemAbility::GetDeviceLockedStateByAuth(int32_t authState)
{
    int32_t authBoundary = static_cast<int32_t>(AuthState::AUTHED_BY_CREDENTIAL);
    if (authState >= authBoundary) {
        return false;
    }
    return true;
}

void ScreenLockSystemAbility::AuthStateInit(const int32_t userId)
{
    std::unique_lock<std::mutex> authLock(authStateMutex_);
    auto authIter = authStateInfo.find(userId);
    if (authIter == authStateInfo.end()) {
        authStateInfo.insert(std::make_pair(userId, static_cast<int32_t>(AuthState::UNAUTH)));
    }
    authLock.unlock();

    std::lock_guard<std::mutex> screenStateLock(screenLockMutex_);
    auto lockIter = isScreenlockedMap_.find(userId);
    if (lockIter == isScreenlockedMap_.end()) {
        isScreenlockedMap_.insert(std::make_pair(userId, true));
    }
}

void ScreenLockSystemAbility::SubscribeUserIamReady()
{
    int ret = WatchParameter(IAM_EVENT_KEY, UserIamReadyCallback, nullptr);
    SCLOCK_HILOGW("SubscribeUserIamReady WatchParameter ret=%{public}d", ret);
}

void ScreenLockSystemAbility::RemoveSubscribeUserIamReady()
{
    int ret = RemoveParameterWatcher(IAM_EVENT_KEY, UserIamReadyCallback, nullptr);
    SCLOCK_HILOGW("RemoveParameterWatcher ret=%{public}d", ret);
}

void ScreenLockSystemAbility::UserIamReadyNotify(const char *value)
{
    SCLOCK_HILOGW("SubscribeUserIamReady state=%{public}s", value);
    SystemEvent systemEvent(USERIAM_READY);
    systemEvent.params_ = value;
    SystemEventCallBack(systemEvent);
}

bool ScreenLockSystemAbility::CheckSystemPermission()
{
    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    return !IsSystemApp() && tokenType != TOKEN_NATIVE;
}

void ScreenLockSystemAbility::printCallerPid(std::string invokeName)
{
    auto callerPid = IPCSkeleton::GetCallingPid();
    SCLOCK_HILOGI("%{public}s callerPid:%{public}d", invokeName.c_str(), callerPid);
}

#ifdef SUPPORT_WEAR_PAYMENT_APP
int32_t ScreenLockSystemAbility::IsLockedWatch(bool &isLocked)
{
    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_HAP && !IsSystemApp()) {
        SCLOCK_HILOGI("calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    bool isScreenLocked = IsScreenLocked();
    isLocked = isScreenLocked || WatchAppLockManager::GetInstance().IsScreenLocked(isScreenLocked);
    SCLOCK_HILOGI("isLocked:%{public}d", isLocked);
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockSystemAbility::UnlockWatch(const sptr<ScreenLockCallbackInterface> &listener)
{
    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_HAP && !IsSystemApp()) {
        SCLOCK_HILOGI("calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    bool isScreenLocked = IsScreenLocked();
    if (isScreenLocked) {
        SCLOCK_HILOGI("device lock");
        UnlockScreen(listener);
    } else {
        SCLOCK_HILOGI("app lock");
        bool isAppLocked = WatchAppLockManager::GetInstance().IsScreenLocked(isScreenLocked);
        WatchAppLockManager::GetInstance().unlockScreen(isAppLocked);
    }
    return E_SCREENLOCK_OK;
}
#endif // SUPPORT_WEAR_PAYMENT_APP
} // namespace ScreenLock
} // namespace OHOS