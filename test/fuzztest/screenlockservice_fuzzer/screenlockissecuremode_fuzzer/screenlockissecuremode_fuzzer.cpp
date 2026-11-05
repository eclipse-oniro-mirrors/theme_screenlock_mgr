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

#include "screenlockissecuremode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "fuzzer/FuzzedDataProvider.h"
#include "token_setproc.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
constexpr long long OFFSET = 32;
constexpr long long TOKEN_ID = 718336240uLL;

static void FuzzIsSecureMode(FuzzedDataProvider &fdp)
{
    auto p_systemAbility = ScreenLockSystemAbility::GetInstance();
    if (p_systemAbility == nullptr) {
        return;
    }
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    p_systemAbility->Lock(userId);
    p_systemAbility->GetSecure();
}
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    auto p_systemAbility = ScreenLockSystemAbility::GetInstance();
    if (SetSelfTokenID(OHOS::TOKEN_ID | (1uLL << OHOS::OFFSET)) < 0) {
        return -1;
    }

    if (p_systemAbility == nullptr) {
        return -1;
    }
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::FuzzIsSecureMode(fdp);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    return 0;
}