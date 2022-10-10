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
#include "screenlock_system_ability_proxy.h"

#include "message_parcel.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
ScreenLockSystemAbilityProxy::ScreenLockSystemAbilityProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ScreenLockSystemAbilityInterface>(impl)
{
}

void ScreenLockSystemAbilityProxy::OnCallBack(const SystemEvent &systemEvent)
{
    SCLOCK_HILOGD("ScreenLockSystemAbilityProxy::OnCallBack Start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(ScreenLockSystemAbilityProxy::GetDescriptor())) {
        SCLOCK_HILOGE("write descriptor failed");
        return;
    }
    if (!data.WriteString(systemEvent.eventType_)) {
        SCLOCK_HILOGE("write string failed");
        return;
    }
    if (!data.WriteString(systemEvent.params_)) {
        SCLOCK_HILOGE("write string failed");
        return;
    }
    int error = Remote()->SendRequest(ONCALLBACK, data, reply, option);
    if (error != 0) {
        SCLOCK_HILOGE("SendRequest failed, error %{public}d", error);
    }
    SCLOCK_HILOGD("ScreenLockSystemAbilityProxy::OnCallBack End");
}

void ScreenLockSystemAbilityProxy::SetErrorInfo(const ErrorInfo &errorInfo)
{
}
} // namespace ScreenLock
} // namespace OHOS
