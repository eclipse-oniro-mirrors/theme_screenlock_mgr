/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "innerListener_fuzz_utils.h"

#include "ipc_object_stub.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
void StrongAuthListenerTest::OnStrongAuthChanged(int32_t userId, int32_t authenticated)
{
    lastUserId_ = userId;
    lastAuthenticated_ = authenticated;
    SCLOCK_HILOGI("StrongAuthListenerTest OnStrongAuthChanged userId:%{public}d authenticated:%{public}d", userId,
        authenticated);
}

void DeviceLockedListenerTest::OnDeviceLockStateChanged(int userId, bool isDeviceLocked)
{
    lastUserId_ = userId;
    lastIsDeviceLocked_ = isDeviceLocked;
    SCLOCK_HILOGI("DeviceLockedListenerTest OnDeviceLockStateChanged userId:%{public}d isDeviceLocked:%{public}d",
        userId, static_cast<int32_t>(isDeviceLocked));
}

int32_t InnerListenerIfTest::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    lastCode_ = code;
    SCLOCK_HILOGI("InnerListenerIfTest OnRemoteRequest code:%{public}u", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

void InnerListenerIfTest::OnStateChanged(int32_t userId, int32_t state)
{
    lastUserId_ = userId;
    lastState_ = state;
    SCLOCK_HILOGI("InnerListenerIfTest OnStateChanged userId:%{public}d state:%{public}d", userId, state);
}
}
}