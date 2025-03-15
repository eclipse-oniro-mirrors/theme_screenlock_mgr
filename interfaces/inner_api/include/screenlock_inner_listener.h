/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef I_SCREENLOCK_INNER_LISTENER_H
#define I_SCREENLOCK_INNER_LISTENER_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
class InnerListener : public virtual RefBase {
public:
    explicit InnerListener(int32_t userId) : userId_(userId) {};
    virtual ~InnerListener() = default;
    int32_t GetUserId() { return userId_; }
    virtual void OnStateChanged(int32_t userId, int32_t state) = 0;
private:
    int32_t userId_ = static_cast<int32_t>(SpecialUserId::USER_UNDEFINED);
};


class StrongAuthListener : public InnerListener {
public:
    explicit StrongAuthListener(int32_t userId) : InnerListener(userId) {};
    virtual ~StrongAuthListener() = default;
    virtual void OnStrongAuthChanged(int32_t userId, int32_t authenticated) = 0;
    void OnStateChanged(int32_t userId, int32_t state) override
    {
        OnStrongAuthChanged(userId, state);
    }
};

class DeviceLockedListener : public InnerListener {
public:
    explicit DeviceLockedListener(int userId): InnerListener(userId) {};
    virtual ~DeviceLockedListener() = default;
    virtual void OnDeviceLockStateChanged(int userId, bool isDeviceLocked) = 0;
    void OnStateChanged(int32_t userId, int32_t state) override
    {
        if (state == 0) {
            OnDeviceLockStateChanged(userId, false);
        } else {
            OnDeviceLockStateChanged(userId, true);
        }
    }
};
} // namespace ScreenLock
} // namespace OHOS

#endif // I_SCREENLOCK_INNER_LISTENER_H