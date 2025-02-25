/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "screenlock_strongauth_listener_stub.h"

#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
ScreenLockStrongAuthListenerStub::~ScreenLockStrongAuthListenerStub()
{
}

int32_t ScreenLockStrongAuthListenerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    auto descriptorToken = data.ReadInterfaceToken();
    if (descriptorToken != GetDescriptor()) {
        SCLOCK_HILOGE("Remote descriptor not the same as local descriptor.");
        return E_SCREENLOCK_TRANSACT_ERROR;
    }
    switch (code) {
        case ON_STRONGAUTH_CHANGED: {
            OnStrongAuthChangedStub(data, reply);
            break;
        }
        default: {
            return OHOS::UNKNOWN_TRANSACTION;
        }
    }
    return OHOS::NO_ERROR;
}


int32_t ScreenLockStrongAuthListenerStub::OnStrongAuthChangedStub(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = 0;
    if (!data.ReadInt32(userId)) {
        SCLOCK_HILOGE("failed to read userId");
        return ERR_INVALID_DATA;
    }
    bool strongAuth = false;
    if (!data.ReadBool(strongAuth)) {
        SCLOCK_HILOGE("failed to read strongAuth");
        return ERR_INVALID_DATA;
    }

    OnStrongAuthChanged(userId, strongAuth);
    return 0;
}

void ScreenLockStrongAuthListenerStub::OnStrongAuthChanged(int32_t userId, int32_t strongAuth)
{
    SCLOCK_HILOGI("called");
    return;
}

} // namespace ScreenLock
} // namespace OHOS