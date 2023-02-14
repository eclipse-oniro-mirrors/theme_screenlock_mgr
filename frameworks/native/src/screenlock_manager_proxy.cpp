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
#include "screenlock_manager_proxy.h"

#include "hilog/log_cpp.h"
#include "iremote_broker.h"
#include "sclock_log.h"

namespace OHOS {
namespace ScreenLock {
using namespace OHOS::HiviewDFX;

ScreenLockManagerProxy::ScreenLockManagerProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<ScreenLockManagerInterface>(object)
{
}

bool ScreenLockManagerProxy::IsScreenLocked()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    SCLOCK_HILOGD("ScreenLockManagerProxy IsScreenLocked started.");
    bool ret = Remote()->SendRequest(IS_SCREEN_LOCKED, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("IsScreenLocked, ret = %{public}d", ret);
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManagerProxy IsScreenLocked succeeded.");
    bool result = reply.ReadBool();
    return result;
}

bool ScreenLockManagerProxy::GetSecure()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ScreenLockManagerProxy::GetDescriptor());
    SCLOCK_HILOGD("ScreenLockManagerProxy GetSecure started.");
    bool ret = Remote()->SendRequest(IS_SECURE_MODE, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("GetSecure, ret = %{public}d", ret);
        return false;
    }
    SCLOCK_HILOGD("ScreenLockManagerProxy GetSecure succeeded.");
    bool result = reply.ReadBool();
    return result;
}

int32_t ScreenLockManagerProxy::RequestUnlock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    SCLOCK_HILOGD("ScreenLockManagerProxy RequestUnlock started.");
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(REQUEST_UNLOCK, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RequestUnlock, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy RequestUnlock end retCode is %{public}d.", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::RequestLock(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SCLOCK_HILOGE(" Failed to write parcelable ");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    SCLOCK_HILOGD("ScreenLockManagerProxy RequestLock started.");
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(REQUEST_LOCK, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RequestLock, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }

    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy RequestLock end .retCode is %{public}d", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::OnSystemEvent(const sptr<ScreenLockSystemAbilityInterface> &listener)
{
    SCLOCK_HILOGD("ScreenLockManagerProxy::OnSystemEvent");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SCLOCK_HILOGE(" Failed to write parcelable ");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t result = Remote()->SendRequest(ONSYSTEMEVENT, data, reply, option);
    if (result != ERR_NONE) {
        SCLOCK_HILOGE(" ScreenLockManagerProxy::OnSystemEvent fail, result = %{public}d ", result);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t status = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy::OnSystemEvent out status is :%{public}d", status);
    return status;
}

int32_t ScreenLockManagerProxy::SendScreenLockEvent(const std::string &event, int param)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    SCLOCK_HILOGD("ScreenLockManagerProxy SendScreenLockEvent started.");
    data.WriteString(event);
    data.WriteInt32(param);
    int32_t ret = Remote()->SendRequest(SEND_SCREENLOCK_EVENT, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy SendScreenLockEvent, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy SendScreenLockEvent end retCode is %{public}d.", retCode);
    return retCode;
}
} // namespace ScreenLock
} // namespace OHOS