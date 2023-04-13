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

#include "screenlock_manager_stub.h"

#include <string>

#include "parcel.h"
#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_system_ability_interface.h"

namespace OHOS {
namespace ScreenLock {
using namespace OHOS::HiviewDFX;

int32_t ScreenLockManagerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    SCLOCK_HILOGD("OnRemoteRequest started, code = %{public}d", code);
    int32_t result = -1;
    auto descriptorToken = data.ReadInterfaceToken();
    if (descriptorToken != GetDescriptor()) {
        SCLOCK_HILOGE("Remote descriptor not the same as local descriptor.");
        return E_SCREENLOCK_TRANSACT_ERROR;
    }
    switch (code) {
        case IS_LOCKED:
            return OnIsLocked(data, reply);
        case IS_SCREEN_LOCKED:
            return OnIsScreenLocked(data, reply);
        case IS_SECURE_MODE:
            return OnGetSecure(data, reply);
        case UNLOCK:
            return OnUnlock(data, reply);
        case UNLOCK_SCREEN:
            return OnUnlockScreen(data, reply);
        case LOCK:
            return OnLock(data, reply);
        case SEND_SCREENLOCK_EVENT:
            return OnSendScreenLockEvent(data, reply);
        case ONSYSTEMEVENT:
            return OnScreenLockOn(data, reply);
        default:
            SCLOCK_HILOGE("Default value received, check needed.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return result;
}

int32_t ScreenLockManagerStub::OnIsLocked(Parcel &data, Parcel &reply)
{
    bool isLocked = false;
    int32_t ret = IsLocked(isLocked);
    reply.WriteInt32(ret);
    if (ret == E_SCREENLOCK_OK) {
        reply.WriteBool(isLocked);
    }
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnIsScreenLocked(Parcel &data, Parcel &reply)
{
    bool isScreenLocked = IsScreenLocked();
    reply.WriteBool(isScreenLocked);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnGetSecure(Parcel &data, Parcel &reply)
{
    bool result = GetSecure();
    reply.WriteBool(result);
    SCLOCK_HILOGD("GetSecure result = %{public}d", result);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnUnlock(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("remote is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("listener is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = Unlock(listener);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnUnlockScreen(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("remote is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("listener is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = UnlockScreen(listener);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnLock(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub remote is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub listener is null");
        return ERR_INVALID_DATA;
    }
    int32_t status = Lock(listener);
    reply.WriteInt32(status);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnScreenLockOn(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub remote is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub listener is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = OnSystemEvent(listener);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnSendScreenLockEvent(MessageParcel &data, MessageParcel &reply)
{
    std::string event = data.ReadString();
    int param = data.ReadInt32();
    SCLOCK_HILOGD("event=%{public}s ", event.c_str());
    SCLOCK_HILOGD("param=%{public}d ", param);
    int32_t retCode = SendScreenLockEvent(event, param);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}
} // namespace ScreenLock
} // namespace OHOS