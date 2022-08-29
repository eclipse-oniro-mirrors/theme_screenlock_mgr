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

#ifndef SERVICES_INCLUDE_SCLOCK_EVENT_HANDLER_H
#define SERVICES_INCLUDE_SCLOCK_EVENT_HANDLER_H

#include <string>

#include "screenlock_system_ability.h"

namespace OHOS {
namespace ScreenLock {
using namespace std;
class SystemEvent {
public:
    SystemEvent() {};
    ~SystemEvent() {};

    int eventCode_ = 0;
    std::string eventName_;
    int eventParams1_ = 0;
    int eventParams2_ = 0;
    int eventParams3_ = 0;
};
class SystemEventHandler {
public:
    SystemEventHandler() {};
    ~SystemEventHandler() {};
    bool SendSystemEvent(const SystemEvent event)
    {
        switch (event.eventCode_) {
            case ON_BEGIN_WAKE_UP:
                ScreenLockSystemAbility::GetInstance()->OnBeginWakeUp();
                return true;
            case ON_END_WAKE_UP:
                ScreenLockSystemAbility::GetInstance()->OnEndWakeUp();
                return true;
            case ON_BEGIN_SCREEN_ON:
                ScreenLockSystemAbility::GetInstance()->OnBeginScreenOn();
                return true;
            case ON_END_SCREEN_ON:
                ScreenLockSystemAbility::GetInstance()->OnEndScreenOn();
                return true;
            case ON_BEGIN_SLEEP:
                ScreenLockSystemAbility::GetInstance()->OnBeginSleep(event.eventParams1_);
                return true;
            case ON_END_SLEEP:
                ScreenLockSystemAbility::GetInstance()->OnEndSleep(event.eventParams1_, event.eventParams2_);
                return true;
            case ON_BEGIN_SCREEN_OFF:
                ScreenLockSystemAbility::GetInstance()->OnBeginScreenOff();
                return true;
            case ON_END_SCREEN_OFF:
                ScreenLockSystemAbility::GetInstance()->OnEndScreenOff();
                return true;
            case ON_CHANGE_USER:
                ScreenLockSystemAbility::GetInstance()->OnChangeUser(event.eventParams1_);
                return true;
            case ON_SCREENLOCK_ENABLED:
                ScreenLockSystemAbility::GetInstance()->OnScreenlockEnabled(event.eventParams1_);
                return true;
            case ON_EXIT_ANIMATION:
                ScreenLockSystemAbility::GetInstance()->OnExitAnimation();
                return true;
            case REQUEST_UNLOCK:
                ScreenLockSystemAbility::GetInstance()->RequestUnlock();
                return true;
            case REQUEST_LOCK:
                ScreenLockSystemAbility::GetInstance()->RequestLock();
                return true;
            default:
                SCLOCK_HILOGE("SystemEventHandler::SendSystemEvent is no matching code .");
                return false;
        }
    }

private:
    enum {
        ON_BEGIN_WAKE_UP = 0,
        ON_END_WAKE_UP = 1,
        ON_BEGIN_SCREEN_ON = 2,
        ON_END_SCREEN_ON = 3,
        ON_BEGIN_SLEEP = 4,
        ON_END_SLEEP = 5,
        ON_BEGIN_SCREEN_OFF = 6,
        ON_END_SCREEN_OFF = 7,
        ON_CHANGE_USER = 8,
        ON_SCREENLOCK_ENABLED = 9,
        ON_EXIT_ANIMATION = 10,
        REQUEST_UNLOCK = 11,
        REQUEST_LOCK = 12,
    };
};
} // namespace ScreenLock
} // namespace OHOS
#endif