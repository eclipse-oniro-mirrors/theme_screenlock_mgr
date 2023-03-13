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
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "ability_manager_client.h"
#include "screenlock_appinfo.h"

namespace OHOS {
namespace ScreenLock {
using namespace OHOS::HiviewDFX;
using namespace OHOS::Security::AccessToken;

int32_t ScreenLockManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
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
        case REQUEST_UNLOCK:
            OnRequestUnlock(data, reply);
            return 0;
        case REQUEST_UNLOCK_SCREEN:
            OnRequestUnlockScreen(data, reply);
            return 0;
        case REQUEST_LOCK:
            OnRequestLock(data, reply);
            return 0;
        case SEND_SCREENLOCK_EVENT:
            result = OnSendScreenLockEvent(data, reply);
            break;
        case ONSYSTEMEVENT:
            result = OnScreenLockOn(data, reply);
            break;
        default:
            SCLOCK_HILOGE("Default value received, check needed.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return result;
}

int32_t ScreenLockManagerStub::OnIsLocked(Parcel &data, Parcel &reply)
{
    if (!IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        return E_SCREENLOCK_NOT_SYSTEM_APP;
    }
    return OnIsScreenLocked(data, reply);
}

int32_t ScreenLockManagerStub::OnIsScreenLocked(Parcel &data, Parcel &reply)
{
    bool isLocked = false;
    int32_t result = IsScreenLocked(isLocked);
    reply.WriteBool(isLocked);
    if (!reply.WriteInt32(result)) {
        SCLOCK_HILOGE("Write failed");
        return E_SCREENLOCK_WRITE_PARCEL_ERROR;
    }
    return E_SCREENLOCK_OK;
}

bool ScreenLockManagerStub::OnGetSecure(Parcel &data, Parcel &reply)
{
    bool result = GetSecure();
    if (!reply.WriteBool(result)) {
        SCLOCK_HILOGE("WriteBool failed");
        return false;
    }
    return true;
}

#ifdef OHOS_TEST_FLAG
bool ScreenLockManagerStub::IsAppInForeground(uint32_t tokenId)
{
    return true;
}

bool ScreenLockManagerStub::IsSystemApp()
{
    return true;
}

#else
bool ScreenLockManagerStub::IsAppInForeground(uint32_t tokenId)
{
    using namespace OHOS::AAFwk;
    AppInfo appInfo;
    auto ret = ScreenLockAppInfo::GetAppInfoByToken(tokenId, appInfo);
    if (!ret || appInfo.bundleName.empty()) {
        SCLOCK_HILOGI("get bundle name by token failed");
        return false;
    }
    auto elementName = AbilityManagerClient::GetInstance()->GetTopAbility();
    SCLOCK_HILOGD(" TopelementName:%{public}s, elementName.GetBundleName:%{public}s",
        elementName.GetBundleName().c_str(), appInfo.bundleName.c_str());
    return elementName.GetBundleName() == appInfo.bundleName;
}

bool ScreenLockManagerStub::IsSystemApp()
{
    return = TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID());
}
#endif

void ScreenLockManagerStub::OnRequestUnlockInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("remote is nullptr");
        reply.WriteInt32(E_SCREENLOCK_NULLPTR);
        return;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("listener is null");
        reply.WriteInt32(E_SCREENLOCK_NULLPTR);
        return;
    }
    int32_t status = RequestUnlock(listener);
    reply.WriteInt32(status);
}

void ScreenLockManagerStub::OnRequestUnlock(MessageParcel &data, MessageParcel &reply)
{
    SCLOCK_HILOGD("RequestUnlock started.");
    if (!IsSystemApp()) {
        SCLOCK_HILOGE("Calling app is not system app");
        reply.WriteInt32(E_SCREENLOCK_NOT_SYSTEM_APP);
        return;
    }
    OnRequestUnlockInner(data, reply);
}

void ScreenLockManagerStub::OnRequestUnlockScreen(MessageParcel &data, MessageParcel &reply)
{
    SCLOCK_HILOGD("RequestUnlockScreen started.");
    if (!IsAppInForeground(IPCSkeleton::GetCallingTokenID())) {
        SCLOCK_HILOGE("RequestUnlockScreen  Unfocused.");
        reply.WriteInt32(E_SCREENLOCK_NO_PERMISSION);
        return;
    }
    OnRequestUnlockInner(data, reply);
}

void ScreenLockManagerStub::OnRequestLock(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub remote is nullptr");
        reply.WriteInt32(E_SCREENLOCK_NULLPTR);
        return;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub listener is null");
        reply.WriteInt32(E_SCREENLOCK_NULLPTR);
        return;
    }
    int32_t status = RequestLock(listener);
    reply.WriteInt32(status);
}

int32_t ScreenLockManagerStub::OnScreenLockOn(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub remote is nullptr");
        if (!reply.WriteInt32(E_SCREENLOCK_NULLPTR)) {
            return -1;
        }
        return 0;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGE("ScreenLockManagerStub listener is null");
        if (!reply.WriteInt32(E_SCREENLOCK_NULLPTR)) {
            return -1;
        }
        return 0;
    }
    int32_t ret = OnSystemEvent(listener);
    if (!reply.WriteInt32(ret)) {
        SCLOCK_HILOGE("ScreenLockManagerStub write int32 failed.");
        return -1;
    }
    return ret;
}

int32_t ScreenLockManagerStub::OnSendScreenLockEvent(MessageParcel &data, MessageParcel &reply)
{
    std::string event = data.ReadString();
    int param = data.ReadInt32();
    SCLOCK_HILOGD("event=%{public}s ", event.c_str());
    SCLOCK_HILOGD("param=%{public}d ", param);
    int32_t retCode = SendScreenLockEvent(event, param);
    reply.WriteInt32(retCode);
    return retCode;
}
} // namespace ScreenLock
} // namespace OHOS