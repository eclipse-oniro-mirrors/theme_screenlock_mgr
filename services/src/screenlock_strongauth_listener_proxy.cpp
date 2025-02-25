/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "screenlock_strongauth_listener_proxy.h"

#include "message_parcel.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
ScreenLockStrongAuthListenerProxy::ScreenLockStrongAuthListenerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<StrongAuthListenerInterface>(impl)
{
}

bool ScreenLockStrongAuthListenerProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SCLOCK_HILOGE("get remote failed");
        return false;
    }
    MessageOption option(MessageOption::TF_ASYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::NO_ERROR) {
        SCLOCK_HILOGE("send request failed, result = %{public}d", result);
        return false;
    }
    return true;
}

void ScreenLockStrongAuthListenerProxy::OnStrongAuthChanged(int32_t userId, int32_t strongAuth)
{
    SCLOCK_HILOGI("ScreenLockStrongAuthListenerProxy::OnStrongAuthChanged Start");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(ScreenLockStrongAuthListenerProxy::GetDescriptor())) {
        SCLOCK_HILOGE("write descriptor failed");
        return;
    }
    if (!data.WriteInt32(userId)) {
        SCLOCK_HILOGE("write userId failed");
        return;
    }
    if (!data.WriteBool(strongAuth)) {
        SCLOCK_HILOGE("write strongAuth failed");
        return;
    }
    bool ret = SendRequest(ON_STRONGAUTH_CHANGED, data, reply);
    if (!ret) {
        SCLOCK_HILOGE("send request failed");
        return;
    }
    SCLOCK_HILOGI("ScreenLockStrongAuthListenerProxy::OnStrongAuthChanged end");
}
} // namespace ScreenLock
} // namespace OHOS