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

#include "ipc_skeleton.h"
#include "parcel.h"
#include "sclock_log.h"
#include "screenlock_callback_interface.h"
#include "screenlock_common.h"
#include "screenlock_server_ipc_interface_code.h"
#include "screenlock_system_ability_interface.h"

namespace OHOS {
namespace ScreenLock {
using namespace OHOS::HiviewDFX;
ScreenLockManagerStub::ScreenLockManagerStub()
{
    InitHandleMap();
}

void ScreenLockManagerStub::InitHandleMap()
{
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_LOCKED)] =
        &ScreenLockManagerStub::OnIsLocked;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREEN_LOCKED)] =
        &ScreenLockManagerStub::OnIsScreenLocked;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SECURE_MODE)] =
        &ScreenLockManagerStub::OnGetSecure;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK)] = &ScreenLockManagerStub::OnUnlock;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK_SCREEN)] =
        &ScreenLockManagerStub::OnUnlockScreen;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK)] = &ScreenLockManagerStub::OnLock;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SEND_SCREENLOCK_EVENT)] =
        &ScreenLockManagerStub::OnSendScreenLockEvent;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::ONSYSTEMEVENT)] =
        &ScreenLockManagerStub::OnScreenLockOn;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK_SCREEN)] =
        &ScreenLockManagerStub::OnLockScreen;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREENLOCK_DISABLED)] =
        &ScreenLockManagerStub::OnIsScreenLockDisabled;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_DISABLED)] =
        &ScreenLockManagerStub::OnSetScreenLockDisabled;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_AUTHSTATE)] =
        &ScreenLockManagerStub::OnSetScreenLockAuthState;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_SCREENLOCK_AUTHSTATE)] =
        &ScreenLockManagerStub::OnGetScreenLockAuthState;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REQUEST_STRONG_AUTHSTATE)] =
        &ScreenLockManagerStub::OnRequestStrongAuth;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_STRONG_AUTHSTATE)] =
        &ScreenLockManagerStub::OnGetStrongAuth;
    handleFuncMap[static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_DEVICE_LOCKED)] =
        &ScreenLockManagerStub::OnIsDeviceLocked;
}

int32_t ScreenLockManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option) __attribute__((no_sanitize("cfi")))
{
    SCLOCK_HILOGD("OnRemoteRequest started, code = %{public}d", code);
    auto descriptorToken = data.ReadInterfaceToken();
    if (descriptorToken != GetDescriptor()) {
        SCLOCK_HILOGE("Remote descriptor not the same as local descriptor.");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto itFunc = handleFuncMap.find(code);
    if (itFunc != handleFuncMap.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            (this->*requestFunc)(data, reply);
            return ERR_NONE;
        }
    }

    SCLOCK_HILOGI("Default value received, check needed.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t ScreenLockManagerStub::OnIsLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isLocked = false;
    int32_t ret = IsLocked(isLocked);
    reply.WriteInt32(ret);
    if (ret == E_SCREENLOCK_OK) {
        reply.WriteBool(isLocked);
    }
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnIsScreenLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isScreenLocked = IsScreenLocked();
    reply.WriteBool(isScreenLocked);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnGetSecure(MessageParcel &data, MessageParcel &reply)
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
    sptr<ScreenLockCallbackInterface> listener = iface_cast<ScreenLockCallbackInterface>(remote);
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
    sptr<ScreenLockCallbackInterface> listener = iface_cast<ScreenLockCallbackInterface>(remote);
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
    sptr<ScreenLockCallbackInterface> listener = iface_cast<ScreenLockCallbackInterface>(remote);
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
    SCLOCK_HILOGD("event=%{public}s, param=%{public}d", event.c_str(), param);
    int32_t retCode = SendScreenLockEvent(event, param);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnIsScreenLockDisabled(MessageParcel &data, MessageParcel &reply)
{
    bool isDisabled = false;
    int userId = data.ReadInt32();
    SCLOCK_HILOGD("userId=%{public}d", userId);
    int32_t retCode = IsScreenLockDisabled(userId, isDisabled);
    reply.WriteInt32(retCode);
    if (retCode == E_SCREENLOCK_OK) {
        reply.WriteBool(isDisabled);
    }
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnSetScreenLockDisabled(MessageParcel &data, MessageParcel &reply)
{
    bool disable = data.ReadBool();
    int userId = data.ReadInt32();
    SCLOCK_HILOGD("disable=%{public}d, userId=%{public}d", disable, userId);
    int32_t retCode = SetScreenLockDisabled(disable, userId);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnSetScreenLockAuthState(MessageParcel &data, MessageParcel &reply)
{
    int32_t authState = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    std::string authToken = data.ReadString();
    int32_t retCode = SetScreenLockAuthState(authState, userId, authToken);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnGetScreenLockAuthState(MessageParcel &data, MessageParcel &reply)
{
    int32_t authState = -1;
    int32_t userId = data.ReadInt32();
    SCLOCK_HILOGD("userId=%{public}d", userId);
    int32_t retCode = GetScreenLockAuthState(userId, authState);
    reply.WriteInt32(retCode);
    if (retCode == E_SCREENLOCK_OK) {
        reply.WriteInt32(authState);
    }
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnRequestStrongAuth(MessageParcel &data, MessageParcel &reply)
{
    int32_t reasonFlag = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    SCLOCK_HILOGD("OnRequestStrongAuth. reasonFlag=%{public}d", reasonFlag);
    int32_t retCode = RequestStrongAuth(reasonFlag, userId);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnGetStrongAuth(MessageParcel &data, MessageParcel &reply)
{
    int32_t reasonFlag = -1;
    int32_t userId = data.ReadInt32();
    int32_t retCode = GetStrongAuth(userId, reasonFlag);
    SCLOCK_HILOGI("userId=%{public}d, reasonFlag=%{public}d", userId, reasonFlag);
    reply.WriteInt32(retCode);
    if (retCode == E_SCREENLOCK_OK) {
        reply.WriteInt32(reasonFlag);
    }
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnLockScreen(MessageParcel &data, MessageParcel &reply)
{
    int32_t useId = data.ReadInt32();
    int32_t retCode = Lock(useId);
    reply.WriteInt32(retCode);
    return ERR_NONE;
}

int32_t ScreenLockManagerStub::OnIsDeviceLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isDeviceLocked = false;
    int32_t userId = data.ReadInt32();
    SCLOCK_HILOGD("userId=%{public}d", userId);
    int32_t retCode = IsDeviceLocked(userId, isDeviceLocked);
    reply.WriteInt32(retCode);
    if (retCode == E_SCREENLOCK_OK) {
        reply.WriteBool(isDeviceLocked);
    }
    return ERR_NONE;
}
} // namespace ScreenLock
} // namespace OHOS