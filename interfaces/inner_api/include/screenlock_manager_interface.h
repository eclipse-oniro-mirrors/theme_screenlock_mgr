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
#include "screenlock_callback_interface.h"
#include "screenlock_common.h"
#include "screenlock_system_ability_interface.h"
#include "screenlock_inner_listener_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockManagerInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.screenlock.ScreenLockManagerInterface");
    virtual int32_t IsLocked(bool &isLocked) = 0;
    virtual bool IsScreenLocked() = 0;
    virtual bool GetSecure() = 0;
    virtual int32_t Unlock(const sptr<ScreenLockCallbackInterface> &listener) = 0;
    virtual int32_t UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener) = 0;
    virtual int32_t Lock(const sptr<ScreenLockCallbackInterface> &listener) = 0;
    virtual int32_t Lock(int32_t userId) = 0;
    virtual int32_t OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener) = 0;
    virtual int32_t SendScreenLockEvent(const std::string &event, int param) = 0;
    virtual int32_t IsScreenLockDisabled(int userId, bool &isDisabled) = 0;
    virtual int32_t SetScreenLockDisabled(bool disable, int userId) = 0;
    virtual int32_t SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken) = 0;
    virtual int32_t GetScreenLockAuthState(int userId, int32_t &authState) = 0;
    virtual int32_t RequestStrongAuth(int reasonFlag, int32_t userId) = 0;
    virtual int32_t GetStrongAuth(int32_t userId, int32_t &reasonFlag) = 0;
    virtual int32_t IsDeviceLocked(int userId, bool &isDeviceLocked) = 0;
    virtual int32_t IsLockedWithUserId(int userId, bool &isLocked) = 0;
    virtual int32_t RegisterInnerListener(const int32_t userId, const ListenType listenType,
                                          const sptr<InnerListenerIf>& listener) = 0;
    virtual int32_t UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
                                            const sptr<InnerListenerIf>& listener) = 0;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_INTERFACE_H