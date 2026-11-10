/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * miscservices under the License is miscservices on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screenlockdump_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "fuzzer/FuzzedDataProvider.h"
#include "message_parcel.h"
#include "screenlock_callback.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
const std::u16string SCREENLOCK_SYSTEMABILITY_INTERFACE_TOKEN = u"OHOS.ScreenLock.ScreenLockSystemAbilityInterface";

bool FuzzScreenlockCallback(FuzzedDataProvider &fdp, const uint8_t *rawData, size_t size)
{
    uint32_t code = fdp.ConsumeIntegral<uint32_t>();

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

bool FuzzScreenlockDisplayPowerEvent(FuzzedDataProvider &fdp)
{
    auto event = static_cast<DisplayPowerEvent>(fdp.ConsumeIntegral<uint8_t>());
    auto status = static_cast<EventStatus>(fdp.ConsumeIntegral<uint8_t>());

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
    FuzzedDataProvider fdp(data, size);
    OHOS::FuzzScreenlockCallback(fdp, data, size);
    OHOS::FuzzScreenlockDisplayPowerEvent(fdp);
    OHOS::FuzzScreenlockDump(data, size);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    return 0;
}