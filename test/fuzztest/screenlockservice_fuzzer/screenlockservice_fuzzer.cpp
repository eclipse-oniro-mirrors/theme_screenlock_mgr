/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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


#include "screenlockservice_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "message_parcel.h"
#include "screenlock_callback.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr size_t THRESHOLD = 14;
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

bool FuzzScreenlockCallback(const uint8_t *rawData, size_t size)
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

bool FuzzScreenlockService(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;

    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);

    return true;
}

bool FuzzScreenlockDisplayPowerEvent(const uint8_t *rawData, size_t size)
{
    uint32_t event = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    uint32_t status = ConvertToUint32(rawData);

    sptr<ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener> displayPowerEventListener_;
    displayPowerEventListener_ = new ScreenLockSystemAbility::ScreenLockDisplayPowerEventListener();
    displayPowerEventListener_->OnDisplayPowerEvent(
        static_cast<DisplayPowerEvent>(event), static_cast<EventStatus>(status));
    return true;
}

bool FuzzScreenlockDump(const uint8_t *rawData, size_t size)
{
    std::vector<std::u16string> args;
    std::string str(reinterpret_cast<const char *>(rawData), size);
    args.push_back(Str8ToStr16(str));
    int fd = 0;
    ScreenLockSystemAbility::GetInstance()->Dump(fd, args);
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
    OHOS::FuzzScreenlockCallback(data, size);
    OHOS::FuzzScreenlockService(data, size);
    OHOS::FuzzScreenlockDisplayPowerEvent(data, size);
    OHOS::FuzzScreenlockDump(data, size);
    return 0;
}