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

#include "screenlockauthmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>
#ifndef IS_SO_CROP_H
#define private public
#define protected public
#include "strongauthmanager.h"
#undef private
#undef protected

using namespace OHOS::ScreenLock;
#else
using namespace OHOS;
#endif // IS_SO_CROP_H

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr size_t LENGTH = 1;

bool FuzzScreenlockAuthManager(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int64_t triggerPeriod = 1;
    int64_t timeInterval = 1000;
    authmanager->RegistIamEventListener();
    authmanager->StartStrongAuthTimer(userId);
    authmanager->GetTimerId(userId);
    authmanager->ResetStrongAuthTimer(userId, triggerPeriod);
    authmanager->DestroyStrongAuthTimer(userId);
    authmanager->DestroyAllStrongAuthTimer();
    authmanager->UnRegistIamEventListener();
    int32_t invalidUserId = 102;
    authmanager->SetStrongAuthStat(invalidUserId, 0);
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->GetStrongAuthStat(invalidUserId);
    authmanager->DestroyStrongAuthStateInfo(invalidUserId);
    authmanager->InitStrongAuthStat(invalidUserId, 0);
    authmanager->IsUserExitInStrongAuthInfo(invalidUserId);
    authmanager->ResetStrongAuthTimer(invalidUserId, CRED_CHANGE_SECOND_STRONG_AUTH_TIMEOUT_MS);
    authmanager->IsUserHasStrongAuthTimer(invalidUserId);
    authmanager->GetStrongAuthTimeTrigger(invalidUserId);
    authmanager->GetStrongAuthTriggerPeriod(invalidUserId);
    authmanager->DestroyStrongAuthStateInfo(invalidUserId);
    authmanager->DestroyStrongAuthTimer(invalidUserId);
    authmanager->RegistIamEventListener();
    authmanager->UnRegistIamEventListener();
    authmanager->GetCredInfo(invalidUserId);
#endif // IS_SO_CROP_H
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
    OHOS::FuzzScreenlockAuthManager(data, size);
    return 0;
}