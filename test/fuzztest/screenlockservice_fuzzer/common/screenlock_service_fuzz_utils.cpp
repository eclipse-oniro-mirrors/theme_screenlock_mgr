/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screenlock_service_fuzz_utils.h"

#include "message_parcel.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
const std::u16string SCREENLOCK_MANAGER_INTERFACE_TOKEN = u"ohos.screenlock.ScreenLockManagerInterface";

void ScreenlockServiceFuzzUtils::OnRemoteRequestTest(uint32_t code, const uint8_t *rawData, size_t size)
{
    MessageParcel data;
    data.WriteInterfaceToken(SCREENLOCK_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    ScreenLockSystemAbility::GetInstance()->OnRemoteRequest(code, data, reply, option);
}
} // namespace OHOS