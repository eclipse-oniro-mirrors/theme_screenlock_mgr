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

#include <cinttypes>

#include "ipc_skeleton.h"
#include "parcel.h"

#include "sclock_log.h"
#include "screenlock_common.h"
#include "screenlock_system_ability_interface.h"
#include "screenlock_hisysevent_adapter.h"

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
        case IS_SCREEN_LOCKED:
            return OnIsScreenLocked(data, reply);
        case IS_SECURE_MODE:
            return OnGetSecure(data, reply);
        case REQUEST_UNLOCK:
            OnRequestUnlock(data, reply);
            return 0;
        case SEND_SCREENLOCK_EVENT:
            result = OnSendScreenLockEvent(data, reply);
            break;
        case ON:
            result = OnScreenLockOn(data, reply);
            break;
        case OFF:
            result = OnScreenLockOff(data, reply);
            break;
        case TEST_SET_SCREENLOCKED:
            result = OnTest_SetScreenLocked(data, reply);
            break;
        case TEST_RUNTIME_NOTIFY:
            result = OnTest_RuntimeNotify(data, reply);
            break;
        case TEST_GET_RUNTIME_STATE:
            result = OnTest_GetRuntimeState(data, reply);
            break;
        default:
            SCLOCK_HILOGE("Default value received, check needed.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return result;
}

bool ScreenLockManagerStub::OnIsScreenLocked(Parcel &data, Parcel &reply)
{
    bool result = IsScreenLocked();
    if (!reply.WriteBool(result)) {
        SCLOCK_HILOGE("WriteBool failed");
        return false;
    }
    return true;
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

void ScreenLockManagerStub::OnRequestUnlock(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    SCLOCK_HILOGD("ScreenLockManagerStub::OnRequestUnlock  addr=%{public}p", remote.GetRefPtr());
    if (remote == nullptr) {
        SCLOCK_HILOGD("ScreenLockManagerStub::OnRequestUnlock remote is nullptr");
        if (!reply.WriteInt32(ERR_NONE)) {
            return;
        }
        return;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    SCLOCK_HILOGD("ScreenLockManagerStub::OnRequestUnlock addr=%{public}p", listener.GetRefPtr());
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGD("ScreenLockManagerStub::OnRequestUnlock listener is null");
        return;
    }
    RequestUnlock(listener);
    return;
}

int32_t ScreenLockManagerStub::OnScreenLockOn(MessageParcel &data, MessageParcel &reply)
{
    std::string type = data.ReadString();
    SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOn  type=%{public}s ", type.c_str());
    if (type.empty()) {
        SCLOCK_HILOGE("ScreenLockManagerStub::OnScreenLockOn type is null.");
        return false;
    }
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOn remote is nullptr");
        if (!reply.WriteInt32(ERR_NONE)) {
            return -1;
        }
        return 0;
    }
    sptr<ScreenLockSystemAbilityInterface> listener = iface_cast<ScreenLockSystemAbilityInterface>(remote);
    if (listener.GetRefPtr() == nullptr) {
        SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOn listener is null");
        return -1;
    }
    bool status = On(listener, type);
    int32_t ret = (status == true) ? 0 : -1;
    if (!reply.WriteInt32(ret)) {
        SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOn 4444");
        return -1;
    }
    SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOn out");
    return 0;
}

int32_t ScreenLockManagerStub::OnScreenLockOff(MessageParcel &data, MessageParcel &reply)
{
    SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOff in");
    std::string type = data.ReadString();
    SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOff  type=%{public}s ", type.c_str());
    bool status = Off(type);
    int32_t ret = (status == true) ? 0 : -1;
    if (!reply.WriteInt32(ret)) {
        return -1;
    }
    SCLOCK_HILOGD("ScreenLockManagerStub::OnScreenLockOff out");
    return 0;
}

bool ScreenLockManagerStub::OnSendScreenLockEvent(MessageParcel &data, MessageParcel &reply)
{
    std::string event = data.ReadString();
    int param = data.ReadInt32();
    SCLOCK_HILOGD("event=%{public}s ", event.c_str());
    SCLOCK_HILOGD("param=%{public}d ", param);
    bool flag = SendScreenLockEvent(event, param);
    reply.WriteBool(flag);
    return flag;
}

bool ScreenLockManagerStub::OnTest_SetScreenLocked(MessageParcel &data, MessageParcel &reply)
{
    SCLOCK_HILOGD("ScreenLockManagerStub Test_SetScreenLocked started.");
    bool isScreenlocked = data.ReadBool();
    bool flag = Test_SetScreenLocked(isScreenlocked);
    reply.WriteBool(flag);
    return flag;
}
bool ScreenLockManagerStub::OnTest_RuntimeNotify(MessageParcel &data, MessageParcel &reply)
{
    std::string event = data.ReadString();
    int param = data.ReadInt32();
    SCLOCK_HILOGD("ScreenLockManagerStub OnTest_RuntimeNotify started.  event=%{public}s", event.c_str());
    SCLOCK_HILOGD("ScreenLockManagerStub OnTest_RuntimeNotify started.  param=%{public}d", param);
    bool flag = Test_RuntimeNotify(event, param);
    reply.WriteBool(flag);
    return flag;
}
int32_t ScreenLockManagerStub::OnTest_GetRuntimeState(MessageParcel &data, MessageParcel &reply)
{
    std::string event = data.ReadString();
    int flag = Test_GetRuntimeState(event);
    reply.WriteInt32(flag);
    return flag;
}
} // namespace ScreenLock
} // namespace OHOS