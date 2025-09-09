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
#endif  // IS_SO_CROP_H

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr size_t LENGTH = 1;

bool FuzzStartStrongAuthTimer(const uint8_t *rawData, size_t size)
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
    int64_t triggerPeriod = static_cast<bool>(rawData[0] % 2);
    authmanager->StartStrongAuthTimer(userId, triggerPeriod);
    userId = rawData[0];
    authmanager->StartStrongAuthTimer(userId, triggerPeriod);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzGetTimerId(const uint8_t *rawData, size_t size)
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
    authmanager->GetTimerId(userId);
    authmanager->RegistIamEventListener();
    userId = rawData[0];
    authmanager->GetTimerId(userId);
    authmanager->RegistIamEventListener();
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzResetStrongAuthTimer(const uint8_t *rawData, size_t size)
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
    int64_t triggerPeriod = static_cast<bool>(rawData[0] % 2);
    authmanager->ResetStrongAuthTimer(userId, triggerPeriod);
    userId = rawData[0];
    authmanager->ResetStrongAuthTimer(userId, triggerPeriod);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzDestroyStrongAuthTimer(const uint8_t *rawData, size_t size)
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
    authmanager->DestroyStrongAuthTimer(userId);
    userId = rawData[0];
    authmanager->DestroyStrongAuthTimer(userId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzDestroyAllStrongAuthTimer(const uint8_t *rawData, size_t size)
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
    int64_t triggerPeriod = static_cast<bool>(rawData[0] % 2);
    authmanager->SetCredChangeTriggerPeriod(userId, triggerPeriod);
    authmanager->DestroyAllStrongAuthTimer();
    userId = rawData[0];
    authmanager->SetCredChangeTriggerPeriod(userId, triggerPeriod);
    authmanager->DestroyAllStrongAuthTimer();
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzGetStrongAuthStat(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }
    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->GetStrongAuthStat(invalidUserId);
    authmanager->UnRegistIamEventListener();
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzDestroyStrongAuthStateInfo(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = 100;
    authmanager->DestroyStrongAuthStateInfo(invalidUserId);
    invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->DestroyStrongAuthStateInfo(invalidUserId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzIsUserExitInStrongAuthInfo(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->IsUserExitInStrongAuthInfo(invalidUserId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzIsUserHasStrongAuthTimer(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->IsUserHasStrongAuthTimer(invalidUserId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzGetStrongAuthTimeTrigger(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->GetStrongAuthTimeTrigger(invalidUserId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzGetStrongAuthTriggerPeriod(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->GetStrongAuthTriggerPeriod(invalidUserId);
#endif  // IS_SO_CROP_H
    return true;
}

bool FuzzGetCredInfo(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

#ifndef IS_SO_CROP_H
    auto authmanager = DelayedSingleton<StrongAuthManger>::GetInstance();
    if (authmanager == nullptr) {
        return false;
    }

    int64_t timeInterval = 1000;
    int32_t invalidUserId = rawData[0];
    StrongAuthManger::authTimer timer(true, timeInterval, true, true);
    authmanager->GetCredInfo(invalidUserId);
#endif  // IS_SO_CROP_H
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
    OHOS::FuzzStartStrongAuthTimer(data, size);
    OHOS::FuzzGetTimerId(data, size);
    OHOS::FuzzResetStrongAuthTimer(data, size);
    OHOS::FuzzDestroyStrongAuthTimer(data, size);
    OHOS::FuzzDestroyAllStrongAuthTimer(data, size);
    OHOS::FuzzGetStrongAuthStat(data, size);
    OHOS::FuzzDestroyStrongAuthStateInfo(data, size);
    OHOS::FuzzIsUserExitInStrongAuthInfo(data, size);
    OHOS::FuzzIsUserHasStrongAuthTimer(data, size);
    OHOS::FuzzGetStrongAuthTimeTrigger(data, size);
    OHOS::FuzzGetStrongAuthTriggerPeriod(data, size);
    OHOS::FuzzGetCredInfo(data, size);
    return 0;
}