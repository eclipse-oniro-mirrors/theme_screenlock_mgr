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

#ifndef SERVICES_INCLUDE_SCLOCK_SERVICE_INTERFACE_H
#define SERVICES_INCLUDE_SCLOCK_SERVICE_INTERFACE_H

#include <string>

#include "iremote_broker.h"

#include "screenlock_common.h"
#include "screenlock_system_ability_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockManagerInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.screenlock.ScreenLockManagerInterface");
    virtual bool IsScreenLocked() = 0;
    virtual bool GetSecure() = 0;
    virtual void RequestUnlock(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual bool On(const sptr<ScreenLockSystemAbilityInterface> &listener, const std::string &type) = 0;
    virtual bool Off(const std::string &type) = 0;
    virtual bool SendScreenLockEvent(const std::string &event, int param) = 0;
    virtual bool Test_SetScreenLocked(const bool isScreenlocked) = 0;
    virtual bool Test_RuntimeNotify(const std::string &event, int param) = 0;
    virtual int Test_GetRuntimeState(const std::string &event) = 0;
};

enum {
    IS_SCREEN_LOCKED = 0,
    IS_SECURE_MODE = 1,
    REQUEST_UNLOCK = 2,
    ON = 3,
    OFF = 4,
    SEND_SCREENLOCK_EVENT = 5,
    TEST_SET_SCREENLOCKED = 6,
    TEST_RUNTIME_NOTIFY = 7,
    TEST_GET_RUNTIME_STATE = 8,
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_INTERFACE_H