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
#include "screenlock_system_ability.h"

#include <cerrno>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#include "command.h"
#include "core_service_client.h"
#include "display_manager.h"
#include "dump_helper.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "sclock_log.h"
#include "screenlock_bundlename.h"
#include "screenlock_common.h"
#include "screenlock_get_info_callback.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "useridm_client.h"

namespace OHOS {
namespace ScreenLock {
using namespace std;
using namespace OHOS::HiviewDFX;
using namespace OHOS::Rosen;
using namespace OHOS::UserIAM::UserIDM;
using namespace OHOS::Telephony;
REGISTER_SYSTEM_ABILITY_BY_ID(ScreenLockSystemAbility, SCREENLOCK_SERVICE_ID, true);
const std::int64_t INIT_INTERVAL = 5000L;
const std::int64_t INTERVAL_ZERO = 0L;
std::mutex ScreenLockSystemAbility::instanceLock_;
sptr<ScreenLockSystemAbility> ScreenLockSystemAbility::instance_;
std::shared_ptr<AppExecFwk::EventHandler> ScreenLockSystemAbility::serviceHandler_;

ScreenLockSystemAbility::ScreenLockSystemAbility(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate), state_(ServiceRunningState::STATE_NOT_START)
{
}

ScreenLockSystemAbility::~ScreenLockSystemAbility()
{
    SCLOCK_HILOGD("~ScreenLockSystemAbility state_  is %{public}d.", static_cast<int>(state_));
}

sptr<ScreenLockSystemAbility> ScreenLockSystemAbility::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> autoLock(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = new ScreenLockSystemAbility(SCREENLOCK_SERVICE_ID, true);
            SCLOCK_HILOGE("ScreenLockSystemAbility instance_ create,addr=%{public}p", instance_.GetRefPtr());
        }
    }
    return instance_;
}

int32_t ScreenLockSystemAbility::Init()
{
    bool ret = Publish(ScreenLockSystemAbility::GetInstance());
    if (!ret) {
        SCLOCK_HILOGE("ScreenLockSystemAbility Publish failed.");
        return E_SCREENLOCK_PUBLISH_FAIL;
    }
    SCLOCK_HILOGD("state_  is %{public}d.", static_cast<int>(state_));
    stateValue_.Reset();
    SCLOCK_HILOGI("Init ScreenLockSystemAbility success.");
    return ERR_OK;
}

void ScreenLockSystemAbility::OnStart()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::Enter OnStart.");
    if (instance_ == nullptr) {
        instance_ = this;
    }
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        SCLOCK_HILOGI("ScreenLockSystemAbility is already running.");
        return;
    }
    InitServiceHandler();
    if (Init() != ERR_OK) {
        auto callback = [=]() { Init(); };
        serviceHandler_->PostTask(callback, INIT_INTERVAL);
        SCLOCK_HILOGE("ScreenLockSystemAbility Init failed. Try again 5s later");
        return;
    }
    if (focusChangedListener_ == nullptr) {
        focusChangedListener_ = new ScreenLockSystemAbility::ScreenLockFocusChangedListener();
    }
    if (displayPowerEventListener_ == nullptr) {
        displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    }
    WindowManager::GetInstance().RegisterFocusChangedListener(focusChangedListener_);
    int trytime = 3;
    int minTimeValue = 0;
    while (trytime > minTimeValue) {
        flag_ = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(displayPowerEventListener_);
        if (flag_) {
            SCLOCK_HILOGI("ScreenLockSystemAbility RegisterDisplayPowerEventListener success.");
            break;
        } else {
            SCLOCK_HILOGI("ScreenLockSystemAbility RegisterDisplayPowerEventListener fail.");
        }
        --trytime;
        sleep(1);
    }
    if (flag_) {
        state_ = ServiceRunningState::STATE_RUNNING;
        auto callback = [=]() { OnSystemReady(); };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
    ScreenlockDump();
    return;
}

void ScreenLockSystemAbility::InitServiceHandler()
{
    SCLOCK_HILOGI("InitServiceHandler started.");
    if (serviceHandler_ != nullptr) {
        SCLOCK_HILOGI("InitServiceHandler already init.");
        return;
    }
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("ScreenLockSystemAbility");
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    SCLOCK_HILOGI("InitServiceHandler succeeded.");
}

void ScreenLockSystemAbility::OnStop()
{
    SCLOCK_HILOGI("OnStop started.");
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        return;
    }
    serviceHandler_ = nullptr;
    instance_ = nullptr;
    state_ = ServiceRunningState::STATE_NOT_START;
    WindowManager::GetInstance().UnregisterFocusChangedListener(focusChangedListener_);
    DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(displayPowerEventListener_);
    SCLOCK_HILOGI("OnStop end.");
}

void ScreenLockSystemAbility::ScreenLockFocusChangedListener::OnFocused(
    const sptr<Rosen::FocusChangeInfo> &focusChangeInfo)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::ScreenLockFocusChangedListener OnFocused");
    instance_->isFoucs_ = true;
}

void ScreenLockSystemAbility::ScreenLockFocusChangedListener::OnUnfocused(
    const sptr<Rosen::FocusChangeInfo> &focusChangeInfo)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::ScreenLockFocusChangedListener OnUnfocused.");
    instance_->isFoucs_ = false;
}

void ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener::OnDisplayPowerEvent(
    DisplayPowerEvent event, EventStatus status)
{
    SCLOCK_HILOGI("OnDisplayPowerEvent event=%{public}d", static_cast<int>(event));
    SCLOCK_HILOGI("OnDisplayPowerEvent status= %{public}d", static_cast<int>(status));
    if (status == EventStatus::BEGIN) {
        if (event == DisplayPowerEvent::WAKE_UP) {
            instance_->OnBeginWakeUp();
        } else if (event == DisplayPowerEvent::SLEEP) {
            instance_->OnBeginSleep(0);
        } else if (event == DisplayPowerEvent::DISPLAY_ON) {
            instance_->OnBeginScreenOn();
        } else if (event == DisplayPowerEvent::DISPLAY_OFF) {
            instance_->OnBeginScreenOff();
        } else if (event == DisplayPowerEvent::DESKTOP_READY) {
            instance_->OnExitAnimation();
        }
    } else if (status == EventStatus::END) {
        if (event == DisplayPowerEvent::WAKE_UP) {
            instance_->OnEndWakeUp();
        } else if (event == DisplayPowerEvent::SLEEP) {
            instance_->OnEndSleep(0, 0);
        } else if (event == DisplayPowerEvent::DISPLAY_ON) {
            instance_->OnEndScreenOn();
        } else if (event == DisplayPowerEvent::DISPLAY_OFF) {
            instance_->OnEndScreenOff();
        }
    }
}

void ScreenLockSystemAbility::OnBeginScreenOff()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOff started.");
    stateValue_.SetScreenState(static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_OFF));
    std::string type = BEGIN_SCREEN_OFF;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOff started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOff started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnEndScreenOff()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOff started.");
    stateValue_.SetScreenState(static_cast<int>(ScreenState::SCREEN_STATE_END_OFF));
    std::string type = END_SCREEN_OFF;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOff started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOff started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnBeginScreenOn()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOn started.");
    stateValue_.SetScreenState(static_cast<int>(ScreenState::SCREEN_STATE_BEGIN_ON));
    std::string type = BEGIN_SCREEN_ON;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOn started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginScreenOn started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnSystemReady()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnSystemReady started.");
    std::string type = SYSTEM_READY;
    bool isExitFlag = false;
    int tryTime = 20;
    int minTryTime = 0;
    while (!isExitFlag && (tryTime > minTryTime)) {
        auto iter = registeredListeners_.find(type);
        if (iter != registeredListeners_.end()) {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnSystemReady started1.");
            iter->second->OnCallBack(type);
            isExitFlag = true;
        } else {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnSystemReady type not found., flag_ = %{public}d", flag_);
            sleep(1);
        }
        --tryTime;
    }
}

void ScreenLockSystemAbility::OnEndScreenOn()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOn started.");
    stateValue_.SetScreenState(static_cast<int>(ScreenState::SCREEN_STATE_END_ON));
    std::string type = END_SCREEN_ON;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOn started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnEndScreenOn started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnBeginWakeUp()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginWakeUp started.");
    stateValue_.SetInteractiveState(static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_WAKEUP));
    std::string type = BEGIN_WAKEUP;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginWakeUp started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginWakeUp started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnEndWakeUp()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnEndWakeUp started.");
    stateValue_.SetInteractiveState(static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_WAKEUP));
    std::string type = END_WAKEUP;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnEndWakeUp started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnEndWakeUp started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnBeginSleep(const int why)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginSleep started.");
    stateValue_.SetOffReason(why);
    stateValue_.SetInteractiveState(static_cast<int>(InteractiveState::INTERACTIVE_STATE_BEGIN_SLEEP));
    std::string type = BEGIN_SLEEP;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginSleep started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnBeginSleep started2.");
            iter->second->OnCallBack(type, why);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnEndSleep(const int why, const int isTriggered)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnEndSleep started.");
    stateValue_.SetInteractiveState(static_cast<int>(InteractiveState::INTERACTIVE_STATE_END_SLEEP));
    std::string type = END_SLEEP;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility OnEndSleep started1.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnEndSleep started2.");
            iter->second->OnCallBack(type, why);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnChangeUser(const int newUserId)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnChangeUser started. newUserId---->%{public}d", newUserId);
    const int minUserId = 0;
    const int maxUserID = 999999999;
    if (newUserId < minUserId || newUserId >= maxUserID) {
        SCLOCK_HILOGI("ScreenLockSystemAbility newUserId invalid.");
        return;
    }
    stateValue_.SetCurrentUser(newUserId);
    std::string type = CHANGE_USER;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        auto callback = [=]() {
            iter->second->OnCallBack(type, newUserId);
            SCLOCK_HILOGI("ScreenLockSystemAbility OnChangeUser OnCallBack. newUserId---->%{public}d", newUserId);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnScreenlockEnabled(bool enabled)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnScreenlockEnabled started.");
    stateValue_.SetScreenlockEnabled(enabled);
    std::string type = SCREENLOCK_ENABLED;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility iter exist.");
        auto callback = [=]() { iter->second->OnCallBack(type, enabled); };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::OnExitAnimation()
{
    SCLOCK_HILOGI("ScreenLockSystemAbility OnExitAnimation started.");
    std::string type = EXIT_ANIMATION;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGI("ScreenLockSystemAbility iter exist.");
        auto callback = [=]() {
            SCLOCK_HILOGI("ScreenLockSystemAbility OnExitAnimation started2.");
            iter->second->OnCallBack(type);
        };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

void ScreenLockSystemAbility::RequestUnlock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    StartAsyncTrace(HITRACE_TAG_MISC, "Services_UnlockScreen start", HITTACE_UNSCREENLOCK_SECOND);
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        SCLOCK_HILOGI("ScreenLockSystemAbility RequestUnlock restart.");
        OnStart();
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility RequestUnlock started.");
    // check whether the page of app request unlock is the focus page
    std::lock_guard<std::mutex> guard(lock_);
    if (instance_->isFoucs_) {
        SCLOCK_HILOGI("ScreenLockSystemAbility RequestUnlock  Unfocused.");
        return;
    }
    unlockVecListeners_.push_back(listener);
    SCLOCK_HILOGI("ScreenLockSystemAbility RequestUnlock listener= %{public}p", listener.GetRefPtr());
    std::string type = UNLOCKSCREEN;
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        auto callback = [=]() { iter->second->OnCallBack(type); };
        serviceHandler_->PostTask(callback, INTERVAL_ZERO);
    }
}

bool ScreenLockSystemAbility::IsScreenLocked()
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        SCLOCK_HILOGI("ScreenLockSystemAbility IsScreenLocked restart.");
        OnStart();
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility IsScreenLocked started.");
    std::lock_guard<std::mutex> guard(lock_);
    bool screnLockState = stateValue_.GetScreenlockedState();
    SCLOCK_HILOGI("IsScreenLocked screnLockState = %{public}d", screnLockState);
    return screnLockState;
}

bool ScreenLockSystemAbility::GetSecure()
{
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        SCLOCK_HILOGI("ScreenLockSystemAbility GetSecure restart.");
        OnStart();
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility GetSecure started.");
    int32_t slotId = DelayedRefSingleton<Telephony::CoreServiceClient>::GetInstance().GetPrimarySlotId();
    int32_t simState =
        DelayedRefSingleton<Telephony::CoreServiceClient>::GetInstance().GetSimState(static_cast<int>(slotId));
    const int32_t simLockStatus = 2;
    if (simState == simLockStatus) {
        return true;
    }
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("ScreenLockSystemAbility::GetSecure callingUid=%{public}d", callingUid);
    int userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    SCLOCK_HILOGD("userId=%{public}d", userId);
    auto getInfoCallback = std::make_shared<ScreenLockGetInfoCallback>();
    int32_t result = UserIDMClient::GetInstance().GetAuthInfo(userId, AuthType::PIN, getInfoCallback);
    SCLOCK_HILOGI("GetAuthInfo AuthType::PIN result = %{public}d", result);
    if (result == static_cast<int32_t>(IDMResultCode::SUCCESS)) {
        std::vector<OHOS::UserIAM::UserIDM::CredentialInfo> pinInfo;
        getInfoCallback->OnGetInfo(pinInfo);
        if (pinInfo.size()) {
            SCLOCK_HILOGI("pinInfo.size() = %{public}zu", pinInfo.size());
            return true;
        }
    }
    result = UserIDMClient::GetInstance().GetAuthInfo(userId, AuthType::FACE, getInfoCallback);
    SCLOCK_HILOGI("GetAuthInfo AuthType::FACE result = %{public}d", result);
    if (result == static_cast<int32_t>(IDMResultCode::SUCCESS)) {
        std::vector<OHOS::UserIAM::UserIDM::CredentialInfo> faceInfo;
        getInfoCallback->OnGetInfo(faceInfo);
        if (faceInfo.size()) {
            SCLOCK_HILOGI("faceInfo.size() = %{public}zu", faceInfo.size());
            return true;
        }
    }
    return false;
}

bool ScreenLockSystemAbility::On(const sptr<ScreenLockSystemAbilityInterface> &listener, const std::string &type)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::On started. type=%{public}s", type.c_str());
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("ScreenLockSystemAbility::On callingUid=%{public}d", callingUid);
    std::string bundleName = "";
    if (!ScreenLockBundleName::GetBundleNameByUid(callingUid, bundleName)) {
        return false;
    }
    SCLOCK_HILOGD("ScreenLockSystemAbility::On bundleName=%{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        SCLOCK_HILOGE("ScreenLockSystemAbility::On calling app is null");
        return false;
    }
    if (bundleName != BUNDLE_NAME) {
        SCLOCK_HILOGE("ScreenLockSystemAbility::On calling app is not Screenlock APP");
        return false;
    }
    auto iter = registeredListeners_.find(type);
    if (iter == registeredListeners_.end()) {
        std::lock_guard<std::mutex> lck(listenerMapMutex_);
        const auto temp = registeredListeners_.insert({ type, listener });
        if (!temp.second) {
            SCLOCK_HILOGE("ScreenLockSystemAbility::On insert type=%{public}s object fail.", type.c_str());
            return false;
        }
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility::On end.");
    return true;
}

bool ScreenLockSystemAbility::Off(const std::string &type)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility::Off started. type=%{public}s", type.c_str());
    int callingUid = IPCSkeleton::GetCallingUid();
    SCLOCK_HILOGD("ScreenLockSystemAbility::Off callingUid=%{public}d", callingUid);
    std::string bundleName = "";
    if (!ScreenLockBundleName::GetBundleNameByUid(callingUid, bundleName)) {
        return false;
    }
    SCLOCK_HILOGD("ScreenLockSystemAbility::Off bundleName=%{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        SCLOCK_HILOGE("ScreenLockSystemAbility::Off calling app is null");
        return false;
    }
    if (bundleName != BUNDLE_NAME) {
        SCLOCK_HILOGE("ScreenLockSystemAbility::Off calling app is not Screenlock APP");
        return false;
    }
    SCLOCK_HILOGI("ScreenLockSystemAbility::Off started.");
    auto iter = registeredListeners_.find(type);
    if (iter != registeredListeners_.end()) {
        SCLOCK_HILOGE("ScreenLockSystemAbility::Off delete type=%{public}s object message.", type.c_str());
        std::lock_guard<std::mutex> lck(listenerMapMutex_);
        registeredListeners_.erase(iter);
    }
    return true;
}

bool ScreenLockSystemAbility::SendScreenLockEvent(const std::string &event, int param)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility SendScreenLockEvent started.");
    int callingUid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    if (!ScreenLockBundleName::GetBundleNameByUid(callingUid, bundleName)) {
        return false;
    }
    if (bundleName.empty()) {
        SCLOCK_HILOGE("SendScreenLockEvent calling app is null");
        return false;
    }
    if (bundleName != BUNDLE_NAME) {
        SCLOCK_HILOGE("SendScreenLockEvent calling app is not Screenlock APP");
        return false;
    }
    SCLOCK_HILOGD("event=%{public}s ,param=%{public}d", event.c_str(), param);
    int stateResult = param;
    if (event == UNLOCK_SCREEN_RESULT) {
        if (stateResult == UNLOCKSCREEN_SUCC) {
            SetScreenlocked(false);
            DisplayManager::GetInstance().NotifyDisplayEvent(DisplayEvent::UNLOCK);
        } else if (stateResult == UNLOCKSCREEN_FAIL || stateResult == UNLOCKSCREEN_CANCEL) {
            SetScreenlocked(true);
        }
        lock_.lock();
        if (unlockVecListeners_.size()) {
            auto callback = [=]() {
                for (size_t i = 0; i < unlockVecListeners_.size(); i++) {
                    std::string type = "";
                    unlockVecListeners_[i]->OnCallBack(type, stateResult);
                }
                unlockVecListeners_.clear();
            };
            serviceHandler_->PostTask(callback, INTERVAL_ZERO);
        }
        lock_.unlock();
    } else if (event == SCREEN_DRAWDONE) {
        DisplayManager::GetInstance().NotifyDisplayEvent(DisplayEvent::KEYGUARD_DRAWN);
    }
    return true;
}

void ScreenLockSystemAbility::SetScreenlocked(bool isScreenlocked)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility SetScreenlocked started.");
    std::lock_guard<std::mutex> guard(lock_);
    stateValue_.SetScreenlocked(isScreenlocked);
}

void StateValue::Reset()
{
    isScreenlocked_ = true;
    screenlockEnabled_ = true;
    currentUser_ = USER_NULL;
}

bool ScreenLockSystemAbility::Test_SetScreenLocked(bool isScreenlocked)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility Test_SetScreenLocked started.");
    stateValue_.SetScreenlocked(isScreenlocked);
    return true;
}

bool ScreenLockSystemAbility::Test_RuntimeNotify(const std::string &event, int param)
{
    SCLOCK_HILOGI("Test_RuntimeNotify event=%{public}s,param=%{public}d", event.c_str(), param);
    if (event == BEGIN_WAKEUP) {
        OnBeginWakeUp();
    } else if (event == END_WAKEUP) {
        OnEndWakeUp();
    } else if (event == BEGIN_SCREEN_ON) {
        OnBeginScreenOn();
    } else if (event == END_SCREEN_ON) {
        OnEndScreenOn();
    } else if (event == BEGIN_SLEEP) {
        OnBeginSleep(param);
    } else if (event == END_SLEEP) {
        OnEndSleep(param, false);
    } else if (event == BEGIN_SCREEN_OFF) {
        OnBeginScreenOff();
    } else if (event == END_SCREEN_OFF) {
        OnEndScreenOff();
    } else if (event == CHANGE_USER) {
        if (param < 0) {
            return false;
        }
        OnChangeUser(param);
    } else if (event == SCREENLOCK_ENABLED) {
        OnScreenlockEnabled((param == 0) ? (false) : (true));
    } else if (event == EXIT_ANIMATION) {
        OnExitAnimation();
    } else {
        return false;
    }
    return true;
}

int ScreenLockSystemAbility::Test_GetRuntimeState(const std::string &event)
{
    SCLOCK_HILOGI("ScreenLockSystemAbility Test_GetRuntimeState started.");
    if (event == BEGIN_WAKEUP || event == END_WAKEUP || event == BEGIN_SLEEP || event == END_SLEEP) {
        return stateValue_.GetInteractiveState();
    } else if (event == BEGIN_SCREEN_ON || event == END_SCREEN_ON || event == BEGIN_SCREEN_OFF ||
               event == END_SCREEN_OFF) {
        return stateValue_.GetScreenState();
    } else if (event == CHANGE_USER) {
        return stateValue_.GetCurrentUser();
    } else if (event == SCREENLOCK_ENABLED) {
        return stateValue_.GetScreenlockEnabled() ? 1 : 0;
    }
    return ARGV_NORMAL;
}

void ScreenLockSystemAbility::OnDump()
{
    std::lock_guard<std::mutex> guard(lock_);
    struct tm *timeNow = nullptr;
    time_t second = time(0);
    if (second > 0) {
        timeNow = localtime(&second);
        if (timeNow != nullptr) {
            SCLOCK_HILOGI(
                "ScreenLockSystemAbility dump time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
                timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
                timeNow->tm_min, timeNow->tm_sec);
        }
    } else {
        SCLOCK_HILOGI("ScreenLockSystemAbility dump, time(0) is nullptr");
    }
}

int ScreenLockSystemAbility::Dump(int fd, const std::vector<std::u16string> &args)
{
    int uid = static_cast<int>(IPCSkeleton::GetCallingUid());
    const int maxUid = 10000;
    if (uid > maxUid) {
        return 0;
    }

    std::vector<std::string> argsStr;
    for (auto item : args) {
        argsStr.emplace_back(Str16ToStr8(item));
    }

    DumpHelper::GetInstance().Dump(fd, argsStr);
    return ERR_OK;
}

void ScreenLockSystemAbility::ScreenlockDump()
{
    auto cmd = std::make_shared<Command>(std::vector<std::string> ({ "-all" }), "Show all",
        [this](const std::vector<std::string> &input, std::string &output) -> bool {
            bool screenLocked = stateValue_.GetScreenlockedState();
            bool screenState = stateValue_.GetScreenState();
            int32_t offReason = stateValue_.GetOffReason();
            int32_t interactiveState = stateValue_.GetInteractiveState();
            string temp_screenLocked = "";
            screenLocked ? temp_screenLocked = "true" : temp_screenLocked = "false";
            string temp_screenState = "";
            screenState ? temp_screenState = "true" : temp_screenState = "false";
            output.append("\n - screenlock system state\tvalue\t\tdescription\n")
                .append(" * screenLocked  \t\t" + temp_screenLocked + "\t\twhether there is lock screen status\n")
                .append(" * screenState  \t\t" + temp_screenState + "\t\tscreen on / off status\n")
                .append(" * offReason  \t\t\t" + std::to_string(offReason) + "\t\tscreen failure reason\n")
                .append(" * interactiveState \t\t" + std::to_string(interactiveState) +
                        "\t\tscreen interaction status\n");
            return true;
        });
    DumpHelper::GetInstance().AddCmdProcess(*cmd);
}
} // namespace ScreenLock
} // namespace OHOS
