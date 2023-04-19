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
    virtual int32_t IsLocked(bool &isLocked) = 0;
    virtual bool IsScreenLocked() = 0;
    virtual bool GetSecure() = 0;
    virtual int32_t Unlock(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual int32_t UnlockScreen(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual int32_t Lock(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual int32_t SendScreenLockEvent(const std::string &event, int param) = 0;
};

enum {
    // before api version 9
    IS_SCREEN_LOCKED = 0,
    IS_SECURE_MODE,
    UNLOCK_SCREEN,

    // since 9, with exception or system app verification
    LOCK,
    ONSYSTEMEVENT,
    SEND_SCREENLOCK_EVENT,
    IS_LOCKED,
    UNLOCK,
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_INTERFACE_H