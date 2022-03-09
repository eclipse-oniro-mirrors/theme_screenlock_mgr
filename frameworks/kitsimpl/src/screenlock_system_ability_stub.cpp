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

#include "screenlock_system_ability_stub.h"

#include "sclock_log.h"
#include "screenlock_common.h"

namespace OHOS {
namespace ScreenLock {
void ScreenLockSystemAbilityStub::OnCallBack(const std::string &event, bool result)
{
}

void ScreenLockSystemAbilityStub::OnCallBack(const std::string &event)
{
}

void ScreenLockSystemAbilityStub::OnCallBack(const std::string &event, int result)
{
}

int32_t ScreenLockSystemAbilityStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    auto descriptorToken = data.ReadInterfaceToken();
    if (descriptorToken != GetDescriptor()) {
        SCLOCK_HILOGE("Remote descriptor not the same as local descriptor.");
        return E_SCREENLOCK_TRANSACT_ERROR;
    }
    SCLOCK_HILOGD("ScreenLockSystemAbilityStub  code----》%{public}u", code);
    switch (code) {
        case ONCALLBACK_BOOL: {
            std::string type = data.ReadString();
            bool result = data.ReadBool();
            SCLOCK_HILOGD("ONCALLBACK_BOOL type----》%{public}s, result----》%{public}d", type.c_str(), result);
            OnCallBack(type, result);
            break;
        }
        case ONCALLBACK_VOID: {
            std::string type = data.ReadString();
            SCLOCK_HILOGD("ScreenLockSystemAbilityStub  ONCALLBACK_VOID type----》%{public}s", type.c_str());
            OnCallBack(type);
            break;
        }
        case ONCALLBACK_INT: {
            std::string type = data.ReadString();
            int result = data.ReadInt32();
            SCLOCK_HILOGD("ONCALLBACK_INT type----》%{public}s,result----》%{public}d", type.c_str(), result);
            OnCallBack(type, result);
            break;
        }
        default: {
            return OHOS::UNKNOWN_TRANSACTION;
        }
    }
    return OHOS::NO_ERROR;
}
} // namespace ScreenLock
} // namespace OHOS
