/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screenlockgetauthstate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "commeventsubscriber.h"
#include "screenlock_server_ipc_interface_code.h"
#include "screenlock_service_fuzz_utils.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t LENGTH = 1;
constexpr int32_t THRESHOLD = 4;
const std::string AUTH_PIN = "1";
const std::string HAS_CREDENTIAL = "1";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";
const std::string USER_CREDENTIAL_UPDATED_NONE = "USER_CREDENTIAL_UPDATED_NONE";

bool FuzzSubscribeEvent(const uint8_t *rawData, size_t size)
{
    if (size >= LENGTH) {
        return false;
    }

    AAFwk::Want want;
    want.SetAction(USER_CREDENTIAL_UPDATED_EVENT);
    want.SetParam("userId", 0);
    want.SetParam("authType", AUTH_PIN);
    want.SetParam("credentialCount", HAS_CREDENTIAL);

    Singleton<CommeventMgr>::GetInstance().SubscribeEvent();
    Singleton<CommeventMgr>::GetInstance().UnSubscribeEvent();
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);

    want.SetAction(USER_CREDENTIAL_UPDATED_NONE);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);

    want.SetParam("userId", rawData[0]);
    Singleton<CommeventMgr>::GetInstance().OnReceiveEvent(want);
    return true;
}

}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::ScreenlockServiceFuzzUtils::OnRemoteRequestTest(
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_SCREENLOCK_AUTHSTATE), data, size);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    OHOS::FuzzSubscribeEvent(data, size);
    return 0;
}