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
#include <random>
#include <string>

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr size_t LENGTH = 1;

int32_t RandNum(int32_t min, int32_t max)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int32_t> randomNum(min, max);
    return randomNum(engine);
}

bool FuzzSaveString(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

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

    userId = rawData[0];
    const int minValue = 32;
    const int maxValue = 126;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minValue, maxValue);

    int length = dis(gen) % 10 + 1;  // 随机长度1到10

    stringlVal.clear();

    for (int i = 0; i < length; ++i) {
        char c = static_cast<char>(dis(gen));
        stringlVal += c;
    }
    preferencesUtil->SaveString(std::to_string(userId), stringlVal);
    preferencesUtil->ObtainString(std::to_string(userId), stringlVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveInt(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

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

    userId = rawData[0];
    const int minValue = 1;
    const int maxValue = 100;
    defaulVal = RandNum(minValue, maxValue);
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
    boolVal = (rawData[0] % 2) != 0;
    preferencesUtil->SaveBool(std::to_string(userId), boolVal);
    preferencesUtil->ObtainBool(std::to_string(userId), boolVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveLong(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

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

    userId = rawData[0];
    const int minValue = 1;
    const int maxValue = 200;
    longVal = static_cast<int64_t>(RandNum(minValue, maxValue));
    preferencesUtil->SaveLong(std::to_string(userId), longVal);
    preferencesUtil->ObtainLong(std::to_string(userId), longVal);
    preferencesUtil->RemoveKey(std::to_string(userId));
    preferencesUtil->Refresh();
    return true;
}

bool FuzzSaveFloat(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

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

    userId = rawData[0];
    const int minValue = 1;
    const int maxValue = 100;
    floatVal = static_cast<float>(RandNum(minValue, maxValue));
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
    OHOS::FuzzScreenlockUtils(data, size);
    OHOS::FuzzSaveString(data, size);
    OHOS::FuzzSaveInt(data, size);
    OHOS::FuzzSaveBool(data, size);
    OHOS::FuzzSaveLong(data, size);
    OHOS::FuzzSaveFloat(data, size);
    OHOS::FuzzObtainBool(data, size);
    OHOS::FuzzRemoveKey(data, size);
    return 0;
}