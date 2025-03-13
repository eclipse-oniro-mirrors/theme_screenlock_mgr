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
#include "screenlock_server_ipc_interface_code.h"

namespace OHOS {
namespace ScreenLock {
using namespace OHOS::HiviewDFX;

ScreenLockManagerProxy::ScreenLockManagerProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<ScreenLockManagerInterface>(object)
{
}

int32_t ScreenLockManagerProxy::IsScreenLockedInner(MessageParcel &reply, uint32_t command)
{
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SCLOCK_HILOGE(" Failed to write parcelable ");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(command, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("IsScreenLocked, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockManagerProxy::IsLocked(bool &isLocked)
{
    MessageParcel reply;
    int32_t ret = IsScreenLockedInner(reply, static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_LOCKED));
    if (ret != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("IsLocked, ret = %{public}d", ret);
        return ret;
    }
    int errCode = reply.ReadInt32();
    if (errCode != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("IsLocked, errCode = %{public}d", errCode);
        return errCode;
    }
    isLocked = reply.ReadBool();
    return E_SCREENLOCK_OK;
}

bool ScreenLockManagerProxy::IsScreenLocked()
{
    MessageParcel reply;
    int32_t ret = IsScreenLockedInner(reply, static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREEN_LOCKED));
    if (ret != E_SCREENLOCK_OK) {
        return false;
    }
    return reply.ReadBool();
}

int32_t ScreenLockManagerProxy::IsLockedWithUserId(int userId, bool &isLocked)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_USER_SCREEN_LOCKED), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy IsLockedWithUserId, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    isLocked = reply.ReadBool();
    SCLOCK_HILOGD("IsLockedWithUserId end retCode is %{public}d, %{public}d.", retCode, isLocked);
    return retCode;
}

bool ScreenLockManagerProxy::GetSecure()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ScreenLockManagerProxy::GetDescriptor());
    SCLOCK_HILOGD("ScreenLockManagerProxy GetSecure started.");
    bool ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SECURE_MODE), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("GetSecure, ret = %{public}d", ret);
        return false;
    }
    return reply.ReadBool();
}

int32_t ScreenLockManagerProxy::UnlockInner(
    MessageParcel &reply, int32_t command, const sptr<ScreenLockCallbackInterface> &listener)
{
    MessageParcel data;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    SCLOCK_HILOGD("started.");
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(command, data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RequestUnlock, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    return E_SCREENLOCK_OK;
}

int32_t ScreenLockManagerProxy::Unlock(const sptr<ScreenLockCallbackInterface> &listener)
{
    MessageParcel reply;
    int ret = UnlockInner(reply, static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK), listener);
    if (ret != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("Unlock, ret = %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t ScreenLockManagerProxy::UnlockScreen(const sptr<ScreenLockCallbackInterface> &listener)
{
    MessageParcel reply;
    int ret = UnlockInner(reply, static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNLOCK_SCREEN), listener);
    if (ret != E_SCREENLOCK_OK) {
        SCLOCK_HILOGE("Unlock, ret = %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t ScreenLockManagerProxy::Lock(const sptr<ScreenLockCallbackInterface> &listener)
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
    int32_t ret =
        Remote()->SendRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RequestLock, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("Lock retCode = %{public}d", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::Lock(int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        SCLOCK_HILOGE(" Failed to write parcelable ");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    if (!data.WriteInt32(userId)) {
        SCLOCK_HILOGE(" Failed to write userId");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::LOCK_SCREEN), data,
        reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RequestLock failed, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("Lock retCode = %{public}d", retCode);
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
    int32_t result = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::ONSYSTEMEVENT), data, reply, option);
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
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SEND_SCREENLOCK_EVENT), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy SendScreenLockEvent, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy SendScreenLockEvent end retCode is %{public}d.", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::IsScreenLockDisabled(int userId, bool &isDisabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);
    SCLOCK_HILOGD("ScreenLockManagerProxy IsScreenLockDisabled started.");
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_SCREENLOCK_DISABLED), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("IsScreenLockDisabled SendRequest failed, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != E_SCREENLOCK_OK) {
        return retCode;
    }
    isDisabled = reply.ReadBool();
    SCLOCK_HILOGD("IsScreenLockDisabled end retCode is %{public}d, %{public}d.", retCode, isDisabled);
    return retCode;
}

int32_t ScreenLockManagerProxy::SetScreenLockDisabled(bool disable, int userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteBool(disable);
    data.WriteInt32(userId);
    SCLOCK_HILOGD("ScreenLockManagerProxy SetScreenLockDisabled started.");
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_DISABLED), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("SetScreenLockDisabled SendRequest failed, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("IsScreenLockDisabled end retCode is %{public}d, %{public}d.", retCode, disable);
    return retCode;
}

int32_t ScreenLockManagerProxy::SetScreenLockAuthState(int authState, int32_t userId, std::string &authToken)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(authState);
    data.WriteInt32(userId);
    data.WriteString(authToken);

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::SET_SCREENLOCK_AUTHSTATE), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy SetScreenLockAuthState, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy SetScreenLockAuthState end retCode is %{public}d.", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::GetScreenLockAuthState(int userId, int32_t &authState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_SCREENLOCK_AUTHSTATE), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy GetScreenLockAuthState, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    authState = reply.ReadInt32();
    SCLOCK_HILOGD("GetScreenLockAuthState end retCode is %{public}d, %{public}d.", retCode, authState);
    return retCode;
}

int32_t ScreenLockManagerProxy::RequestStrongAuth(int reasonFlag, int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(reasonFlag);
    data.WriteInt32(userId);

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REQUEST_STRONG_AUTHSTATE), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy RequestStrongAuth, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerProxy RequestStrongAuth end retCode is %{public}d.", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::GetStrongAuth(int userId, int32_t &reasonFlag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_STRONG_AUTHSTATE), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy GetStrongAuth, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    reasonFlag = reply.ReadInt32();
    SCLOCK_HILOGD("GetStrongAuth end retCode is %{public}d, %{public}d.", retCode, reasonFlag);
    return retCode;
}

int32_t ScreenLockManagerProxy::IsDeviceLocked(int userId, bool &isDeviceLocked)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);

    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::IS_DEVICE_LOCKED), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("ScreenLockManagerProxy IsDeviceLocked, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    isDeviceLocked = reply.ReadBool();
    SCLOCK_HILOGD("IsDeviceLocked end retCode is %{public}d, %{public}d.", retCode, isDeviceLocked);
    return retCode;
}

int32_t ScreenLockManagerProxy::RegisterInnerListener(const int32_t userId, const ListenType listenType,
                                                      const sptr<InnerListenerIf>& listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    SCLOCK_HILOGD("RegisterInnerListener started.");
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);
    data.WriteInt32(static_cast<int32_t>(listenType));
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::REGISTER_INNER_LISTENER), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("RegisterInnerListener, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("RegisterInnerListener end retCode is %{public}d.", retCode);
    return retCode;
}

int32_t ScreenLockManagerProxy::UnRegisterInnerListener(const int32_t userId, const ListenType listenType,
                                                        const sptr<InnerListenerIf>& listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    SCLOCK_HILOGD("UnRegisterInnerListener started.");
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(userId);
    data.WriteInt32(static_cast<int32_t>(listenType));
    if (listener == nullptr) {
        SCLOCK_HILOGE("listener is nullptr");
        return E_SCREENLOCK_NULLPTR;
    }
    if (!data.WriteRemoteObject(listener->AsObject().GetRefPtr())) {
        SCLOCK_HILOGE("write parcel failed.");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    int32_t ret = Remote()->SendRequest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::UNREGISTER_INNER_LISTENER), data, reply, option);
    if (ret != ERR_NONE) {
        SCLOCK_HILOGE("UnRegisterInnerListener, ret = %{public}d", ret);
        return E_SCREENLOCK_SENDREQUEST_FAILED;
    }
    int32_t retCode = reply.ReadInt32();
    SCLOCK_HILOGD("UnRegisterInnerListener end retCode is %{public}d.", retCode);
    return retCode;
}
} // namespace ScreenLock
} // namespace OHOS