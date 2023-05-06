/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "screenlock_callback_proxy.h"

#include "message_parcel.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
ScreenLockCallbackProxy::ScreenLockCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ScreenLockCallbackInterface>(impl)
{
}

void ScreenLockCallbackProxy::OnCallBack(int32_t screenLockResult)
{
    SCLOCK_HILOGD("ScreenLockCallbackProxy::OnCallBack  screenLockResult Start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SCLOCK_HILOGE("write descriptor failed");
        return;
    }
    if (!data.WriteInt32(screenLockResult)) {
        SCLOCK_HILOGE("write screenLockResult failed");
        return;
    }
    int32_t errorCode = Remote()->SendRequest(ON_CALLBACK, data, reply, option);
    if (errorCode != 0) {
        SCLOCK_HILOGE("SendRequest failed, errorCode: %{public}d", errorCode);
    }
}
} // namespace ScreenLock
} // namespace OHOS
