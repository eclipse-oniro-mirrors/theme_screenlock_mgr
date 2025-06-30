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

#ifndef SERVICES_INCLUDE_SCLOCK_SERVICES_H
#define SERVICES_INCLUDE_SCLOCK_SERVICES_H

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include "dm_common.h"
#include "event_handler.h"
#include "ffrt.h"
#include "iremote_object.h"
#include "screenlock_callback_interface.h"
#include "screenlock_manager_stub.h"
#include "screenlock_system_ability_interface.h"
#include "system_ability.h"
#include "visibility.h"
#include "os_account_manager.h"
#include "preferences_util.h"
#include "os_account_subscribe_info.h"

namespace OHOS {
namespace ScreenLock {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class StateValue {
public:
    StateValue(){};
    ~StateValue(){};

    void Reset();

    void SetScreenlockEnabled(bool screenlockEnabled)
    {
        screenlockEnabled_ = screenlockEnabled;
    };

    void SetScreenState(int32_t screenState)
    {
        screenState_ = screenState;
    };

    void SetOffReason(int32_t offReason)
    {
        offReason_ = offReason;
    };

    void SetCurrentUser(int32_t currentUser)
    {
        currentUser_ = currentUser;
    };

    void SetInteractiveState(int32_t interactiveState)
    {
        interactiveState_ = interactiveState;
    };

    bool GetScreenlockEnabled()
    {
        return screenlockEnabled_;
    };

    int32_t GetScreenState()
    {
        return screenState_;
    };

    int32_t GetOffReason()
    {
        return offReason_;
    };

    int32_t GetCurrentUser()
    {
        return currentUser_;
    };

    int32_t GetInteractiveState()
    {
        return interactiveState_;
    };

private:
    std::atomic<bool> screenlockEnabled_ { false };
    std::atomic<int32_t> offReason_ {0};
    std::atomic<int32_t> currentUser_ {0};
    std::atomic<int32_t> screenState_ {0};
    std::atomic<int32_t> interactiveState_ {0};
};

enum class ScreenState : int32_t {
    SCREEN_STATE_BEGIN_OFF = 0,
    SCREEN_STATE_END_OFF = 1,
    SCREEN_STATE_BEGIN_ON = 2,
    SCREEN_STATE_END_ON = 3,
};

enum class InteractiveState : int32_t {
    INTERACTIVE_STATE_END_SLEEP = 0,
    INTERACTIVE_STATE_BEGIN_WAKEUP = 1,
    INTERACTIVE_STATE_END_WAKEUP = 2,
    INTERACTIVE_STATE_BEGIN_SLEEP = 3,
};

enum class AuthState : int32_t {
    UNAUTH = 0,
    PRE_AUTHED_BY_CREDENTIAL = 1,
    PRE_AUTHED_BY_FINGERPRINT = 2,
    PRE_AUTHED_BY_FACE = 3,
    AUTHED_BY_CREDENTIAL = 4,
    AUTHED_BY_FINGERPRINT = 5,
    AUTHED_BY_FACE = 6,
};

class ScreenLockSystemAbility : public SystemAbility, public ScreenLockManagerStub {
    DECLARE_SYSTEM_ABILITY(ScreenLockSystemAbility);

public:
    DISALLOW_COPY_AND_MOVE(ScreenLockSystemAbility);
    ScreenLockSystemAbility(int32_t systemAbilityId, bool runOnCreate);
    ScreenLockSystemAbility();
    ~ScreenLockSystemAbility() override;
    SCREENLOCK_API static sptr<ScreenLockSystemAbility> GetInstance();
    int32_t IsLocked(bool &isLocked) override;
    bool IsScreenLocked() override;
    int32_t IsLockedWithUserId(int32_t userId, bool &isLocked) override;
    bool GetSecure() override;
    int32_t Unlock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t SendScreenLockEvent(const std::string &event, int param) override;
    int32_t IsScreenLockDisabled(int userId, bool &isDisabled) override;
    int32_t SetScreenLockDisabled(bool disable, int userId) override;
    int32_t SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken) override;
    int32_t GetScreenLockAuthState(int userId, int32_t &authState) override;
    int32_t RequestStrongAuth(int reasonFlag, int32_t userId) override;
    int32_t GetStrongAuth(int userId, int32_t &reasonFlag) override;
    int32_t IsDeviceLocked(int userId, bool &isDeviceLocked) override;
    int32_t RegisterInnerListener(const int32_t userId, const ListenType listenType,
                                  const sptr<InnerListenerIf>& listener) override;
    int32_t UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
                                         const sptr<InnerListenerIf>& listener) override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;
    void SetScreenlocked(bool isScreenlocked, const int32_t userId);
    void RegisterDisplayPowerEventListener(int32_t times);
    void ResetFfrtQueue();
    void StrongAuthChanged(int32_t userId, int32_t reasonFlag);
    int32_t Lock(int32_t userId) override;
    void UserIamReadyNotify(const char *value);
    void OnActiveUser(const int lastUser, const int targetUser);
    void OnRemoveUser(const int userId);
    StateValue &GetState()
    {
        return stateValue_;
    }
    class ScreenLockDisplayPowerEventListener : public Rosen::IDisplayPowerEventListener {
    public:
        void OnDisplayPowerEvent(Rosen::DisplayPowerEvent event, Rosen::EventStatus status) override;
    };

    class AccountSubscriber : public AccountSA::OsAccountSubscriber {
    public:
        explicit AccountSubscriber(const AccountSA::OsAccountSubscribeInfo &subscribeInfo,
            const std::function<void(const int lastUser, const int targetUser)> &callback);
        ~AccountSubscriber() override = default;
        void OnAccountsChanged(const int &id) override;

    private:
        int userId_{-1};
        std::function<void(const int lastUser, const int targetUser)> callback_;
    };

protected:
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    void OnScreenOn(Rosen::EventStatus status);
    void OnScreenOff(Rosen::EventStatus status);
    void OnWakeUp(Rosen::EventStatus status);
    void OnSleep(Rosen::EventStatus status);
    void OnExitAnimation();
    void OnSystemReady();
    void RegisterDumpCommand();
    int32_t Init();
    void InitUserId();
    void InitServiceHandler();
    void LockScreenEvent(int stateResult);
    void UnlockScreenEvent(int stateResult);
    void SystemEventCallBack(const SystemEvent &systemEvent, TraceTaskId traceTaskId = HITRACE_BUTT);
    int32_t UnlockInner(const sptr<ScreenLockCallbackInterface> &listener);
    void PublishEvent(const std::string &eventAction, const int32_t userId);
    bool IsAppInForeground(int32_t callingPid, uint32_t callingTokenId);
    bool IsSystemApp();
    bool CheckPermission(const std::string &permissionName);
    void NotifyUnlockListener(const int32_t screenLockResult);
    void NotifyDisplayEvent(Rosen::DisplayEvent event);
    bool GetDeviceLockedStateByAuth(int authState);
    std::shared_ptr<AccountSubscriber> SubscribeAcccount(AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE subscribeType,
        const std::function<void(const int lastUser, const int targetUser)> &callback);
    void AuthStateInit(const int userId);
    void SubscribeUserIamReady();
    void RemoveSubscribeUserIamReady();
    bool CheckSystemPermission();

    ServiceRunningState state_;
    static std::mutex instanceLock_;
    static sptr<ScreenLockSystemAbility> instance_;
    static std::shared_ptr<ffrt::queue> queue_;
    std::map<AccountSA::OS_ACCOUNT_SUBSCRIBE_TYPE, std::shared_ptr<AccountSubscriber>> accountSubscribers_;
    std::mutex accountSubscriberMutex_;
    sptr<Rosen::IDisplayPowerEventListener> displayPowerEventListener_;
    std::mutex listenerMutex_;
    sptr<ScreenLockSystemAbilityInterface> systemEventListener_;
    std::mutex unlockListenerMutex_;
    std::vector<sptr<ScreenLockCallbackInterface>> unlockVecListeners_;
    std::vector<int> unlockVecUserIds_;
    std::mutex lockListenerMutex_;
    std::vector<sptr<ScreenLockCallbackInterface>> lockVecListeners_;
    StateValue stateValue_;
    std::atomic<bool> systemReady_ = false;
    std::map<int32_t, int32_t> authStateInfo;
    std::mutex authStateMutex_;
    std::map<int32_t, bool> isScreenlockedMap_;
    std::mutex screenLockMutex_;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICES_H
