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

#include "screenlockutils_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "preferences_util.h"
#include <string>
#include <fuzzer/FuzzedDataProvider.h>
#include "commeventsubscriber.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr size_t LENGTH = 1;

const std::string AUTH_PIN = "1";
const std::string HAS_CREDENTIAL = "1";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";
const std::string USER_CREDENTIAL_UPDATED_NONE = "USER_CREDENTIAL_UPDATED_NONE";

bool FuzzUnSubscribeEvent(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
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

bool FuzzSaveString(FuzzedDataProvider &fdp)
{
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    // string
    std::string stringlVal = "test";
    preferencesUtil->SaveString(std::to_string(userId), stringlVal);
    preferencesUtil->ObtainString(std::to_string(userId), stringlVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();

    userId = fdp.ConsumeIntegralInRange<int32_t>(1, 100);
    stringlVal = fdp.ConsumeRandomLengthString(10);
    preferencesUtil->SaveString(std::to_string(userId), stringlVal);
    preferencesUtil->ObtainString(std::to_string(userId), stringlVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveInt(FuzzedDataProvider &fdp)
{
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    // int
    int defaulVal = 1;
    preferencesUtil->SaveInt(std::to_string(userId), defaulVal);
    preferencesUtil->ObtainInt(std::to_string(userId), defaulVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();

    userId = fdp.ConsumeIntegralInRange<int32_t>(1, 100);
    defaulVal = fdp.ConsumeIntegralInRange<int32_t>(1, 100);
    preferencesUtil->SaveInt(std::to_string(userId), defaulVal);
    preferencesUtil->ObtainInt(std::to_string(userId), defaulVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveBool(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    // bool
    bool boolVal = 0;
    preferencesUtil->SaveBool(std::to_string(userId), boolVal);
    preferencesUtil->ObtainBool(std::to_string(userId), boolVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();

    userId = rawData[0];
    const int rawValue = 2;
    boolVal = static_cast<bool>(rawData[0] % rawValue);
    preferencesUtil->SaveBool(std::to_string(userId), boolVal);
    preferencesUtil->ObtainBool(std::to_string(userId), boolVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveLong(FuzzedDataProvider &fdp)
{
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    // long
    int64_t longVal = 101;
    preferencesUtil->SaveLong(std::to_string(userId), longVal);
    preferencesUtil->ObtainLong(std::to_string(userId), longVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();

    userId = fdp.ConsumeIntegralInRange<int32_t>(1, 100);
    longVal = static_cast<int64_t>(fdp.ConsumeIntegralInRange<int32_t>(1, 200));
    preferencesUtil->SaveLong(std::to_string(userId), longVal);
    preferencesUtil->ObtainLong(std::to_string(userId), longVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveFloat(FuzzedDataProvider &fdp)
{
    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    // float
    float floatVal = 1.0;
    preferencesUtil->SaveFloat(std::to_string(userId), floatVal);
    preferencesUtil->ObtainFloat(std::to_string(userId), floatVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();

    userId = fdp.ConsumeIntegralInRange<int32_t>(1, 100);
    floatVal = static_cast<float>(fdp.ConsumeIntegralInRange<int32_t>(1, 100));
    preferencesUtil->SaveFloat(std::to_string(userId), floatVal);
    preferencesUtil->ObtainFloat(std::to_string(userId), floatVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzScreenlockUtils(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = 100;

    bool result = preferencesUtil->IsExistKey(std::to_string(userId));
    preferencesUtil->RemoveAll();
    preferencesUtil->RefreshSync();

    userId = rawData[0];
    result = preferencesUtil->IsExistKey(std::to_string(userId));
    preferencesUtil->RemoveAll();
    preferencesUtil->RefreshSync();
    return true;
}

bool FuzzObtainBool(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = rawData[0];
    // bool
    bool boolVal = 0;
    preferencesUtil->ObtainBool(std::to_string(userId), boolVal);
    return true;
}

bool FuzzRemoveKey(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto preferencesUtil = DelayedSingleton<PreferencesUtil>::GetInstance();
    if (preferencesUtil == nullptr) {
        return false;
    }

    int userId = rawData[0];
    preferencesUtil->RemoveKey(std::to_string(userId));
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
    OHOS::FuzzUnSubscribeEvent(data, size);
    OHOS::FuzzScreenlockUtils(data, size);
    FuzzedDataProvider fdp(data, size);
    OHOS::FuzzSaveString(fdp);
    OHOS::FuzzSaveInt(fdp);
    OHOS::FuzzSaveBool(data, size);
    OHOS::FuzzSaveLong(fdp);
    OHOS::FuzzSaveFloat(fdp);
    OHOS::FuzzObtainBool(data, size);
    OHOS::FuzzRemoveKey(data, size);
    return 0;
}