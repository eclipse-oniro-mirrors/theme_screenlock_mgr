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

namespace OHOS {
namespace ScreenLock {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class StateValue {
public:
    StateValue(){};
    ~StateValue(){};

    void Reset();

    void SetScreenlocked(bool isScreenlocked)
    {
        isScreenlocked_ = isScreenlocked;
    };

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

    bool GetScreenlockedState()
    {
        return isScreenlocked_;
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
    std::atomic<bool> isScreenlocked_ = false;
    std::atomic<bool> screenlockEnabled_ = false;
    std::atomic<int32_t> offReason_ = 0;
    std::atomic<int32_t> currentUser_ = 0;
    std::atomic<int32_t> screenState_ = 0;
    std::atomic<int32_t> interactiveState_ = 0;
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
    bool GetSecure() override;
    int32_t Unlock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener) override;
    int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) override;
    int32_t SendScreenLockEvent(const std::string &event, int param) override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;
    void SetScreenlocked(bool isScreenlocked);
    void RegisterDisplayPowerEventListener(int32_t times);
    void ResetFfrtQueue();
    int32_t Lock(int32_t userId) override;
    StateValue &GetState()
    {
        return stateValue_;
    }
    class ScreenLockDisplayPowerEventListener : public Rosen::IDisplayPowerEventListener {
    public:
        void OnDisplayPowerEvent(Rosen::DisplayPowerEvent event, Rosen::EventStatus status) override;
    };

protected:
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    void OnScreenOn(Rosen::EventStatus status);
    void OnScreenOff(Rosen::EventStatus status);
    void OnWakeUp(Rosen::EventStatus status);
    void OnSleep(Rosen::EventStatus status);
    void OnExitAnimation();
    void OnSystemReady();
    void RegisterDumpCommand();
    int32_t Init();
    void InitServiceHandler();
    void LockScreenEvent(int stateResult);
    void UnlockScreenEvent(int stateResult);
    void SystemEventCallBack(const SystemEvent &systemEvent, TraceTaskId traceTaskId = HITRACE_BUTT);
    int32_t UnlockInner(const sptr<ScreenLockCallbackInterface> &listener);
    void PublishEvent(const std::string &eventAction);
    bool IsAppInForeground(int32_t callingPid, uint32_t callingTokenId);
    bool IsSystemApp();
    bool CheckPermission(const std::string &permissionName);
    void NotifyUnlockListener(const int32_t screenLockResult);
    void NotifyDisplayEvent(Rosen::DisplayEvent event);

    ServiceRunningState state_;
    static std::mutex instanceLock_;
    static sptr<ScreenLockSystemAbility> instance_;
    static std::shared_ptr<ffrt::queue> queue_;
    sptr<Rosen::IDisplayPowerEventListener> displayPowerEventListener_;
    std::mutex listenerMutex_;
    sptr<ScreenLockSystemAbilityInterface> systemEventListener_;
    std::mutex unlockListenerMutex_;
    std::vector<sptr<ScreenLockCallbackInterface>> unlockVecListeners_;
    std::mutex lockListenerMutex_;
    std::vector<sptr<ScreenLockCallbackInterface>> lockVecListeners_;
    StateValue stateValue_;
    std::atomic<bool> systemReady_ = false;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICES_H
