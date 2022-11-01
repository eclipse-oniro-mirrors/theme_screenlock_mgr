/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screenlockunlockcallback_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "message_parcel.h"
#include "screenlock_callback.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr int32_t OFFSET = 4;
const std::u16string SCREENLOCK_SYSTEMABILITY_INTERFACE_TOKEN = u"OHOS.ScreenLock.ScreenLockSystemAbilityInterface";
const std::u16string SCREENLOCK_MANAGER_INTERFACE_TOKEN = u"ohos.screenlock.ScreenLockManagerInterface";

uint32_t ConvertToUint32(const uint8_t *ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    uint32_t bigvar = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
    return bigvar;
}

bool FuzzScreenlockUnlockCallback(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;

    EventListener mEventListener;
    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_SYSTEMABILITY_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr<ScreenlockCallback> mScreenlock = new ScreenlockCallback(mEventListener);
    mScreenlock->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockIsScreenLock(const uint8_t *rawData, size_t size)
{
    uint32_t code = IS_SCREEN_LOCKED;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockIsScreenMode(const uint8_t *rawData, size_t size)
{
    uint32_t code = IS_SECURE_MODE;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockRequestUnlock(const uint8_t *rawData, size_t size)
{
    uint32_t code = REQUEST_UNLOCK;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockRequestlock(const uint8_t *rawData, size_t size)
{
    uint32_t code = REQUEST_LOCK;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockSendScreenlockEvent(const uint8_t *rawData, size_t size)
{
    uint32_t code = SEND_SCREENLOCK_EVENT;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockOnSystemEvent(const uint8_t *rawData, size_t size)
{
    uint32_t code = ONSYSTEMEVENT;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzScreenlockUnlockCallback(data, size);
    OHOS::FuzzScreenlockIsScreenLock(data, size);
    OHOS::FuzzScreenlockIsScreenMode(data, size);
    OHOS::FuzzScreenlockRequestUnlock(data, size);
    OHOS::FuzzScreenlockRequestlock(data, size);
    OHOS::FuzzScreenlockSendScreenlockEvent(data, size);
    OHOS::FuzzScreenlockOnSystemEvent(data, size);
    return 0;
}