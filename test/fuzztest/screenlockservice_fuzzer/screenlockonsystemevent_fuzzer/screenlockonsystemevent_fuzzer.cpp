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

#define private public
#define protected public
#include "screenlock_system_ability.h"
#include "innerlistenermanager.h"
#undef private
#undef protected
#include "screenlockonsystemevent_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "screenlock_server_ipc_interface_code.h"
#include "screenlock_service_fuzz_utils.h"
#include "screenlock_callback_interface.h"
#include "screenlock_common.h"
#include "system_ability_definition.h"
#include <random>
#include <string>

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
constexpr size_t LENGTH = 1;

bool FuzzIsLocked(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    // bool
    bool isLocked = 0;
    systemAbility->IsLocked(isLocked);

    const int rawValue = 2;
    isLocked = static_cast<bool>(rawData[0] % rawValue);
    systemAbility->IsLocked(isLocked);
    return true;
}

bool FuzzIsLockedWithUserId(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    // bool
    bool isLocked = 0;
    systemAbility->IsLockedWithUserId(userId, isLocked);

    const int rawValue = 2;
    userId = rawData[0];
    isLocked = static_cast<bool>(rawData[0] % rawValue);
    systemAbility->IsLockedWithUserId(userId, isLocked);
    systemAbility->GetSecure();
    return true;
}

bool FuzzSendScreenLockEvent(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    const std::string eventOne = UNLOCK_SCREEN_RESULT;
    int param = 0;
    param = rawData[0];
    systemAbility->SendScreenLockEvent(eventOne, param);

    const std::string eventTwo = SCREEN_DRAWDONE;
    systemAbility->SendScreenLockEvent(eventTwo, param);

    const std::string eventThree = LOCK_SCREEN_RESULT;
    systemAbility->SendScreenLockEvent(eventThree, param);

    param = rawData[0];
    systemAbility->SendScreenLockEvent(eventOne, param);
    systemAbility->SendScreenLockEvent(eventTwo, param);
    systemAbility->SendScreenLockEvent(eventThree, param);
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    systemAbility->OnSystemEvent(listener);
    return true;
}

bool FuzzIsScreenLockDisabled(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool isDisabled = 0;
    systemAbility->IsScreenLockDisabled(userId, isDisabled);

    userId = rawData[0];
    const int rawValue = 2;
    isDisabled = static_cast<bool>(rawData[0] % rawValue);
    systemAbility->IsScreenLockDisabled(userId, isDisabled);
    return true;
}

bool FuzzSetScreenLockDisabled(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool disable = 0;
    systemAbility->SetScreenLockDisabled(disable, userId);

    userId = rawData[0];
    const int rawValue = 2;
    disable = static_cast<bool>(rawData[0] % rawValue);
    systemAbility->SetScreenLockDisabled(disable, userId);
    return true;
}

bool FuzzSetScreenLockAuthState(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int authState = 1;
    int32_t userId = 100;
    std::string authToken = "test";
    systemAbility->SetScreenLockAuthState(authState, userId, authToken);
    systemAbility->GetScreenLockAuthState(userId, authState);

    userId = rawData[0];
    authState = rawData[0];
    const int minValue = 32;
    const int maxValue = 126;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minValue, maxValue);

    int length = dis(gen) % 10 + 1;  // 随机长度1到10

    authToken.clear();

    for (int i = 0; i < length; ++i) {
        char c = static_cast<char>(dis(gen));
        authToken += c;
    }
    systemAbility->SetScreenLockAuthState(authState, userId, authToken);
    systemAbility->GetScreenLockAuthState(userId, authState);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    systemAbility->Lock(listener);
    return true;
}

bool FuzzRequestStrongAuth(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int reasonFlag = 1;
    int32_t userId = 100;
    systemAbility->RequestStrongAuth(reasonFlag, userId);

    userId = rawData[0];
    reasonFlag = rawData[0];
    systemAbility->RequestStrongAuth(reasonFlag, userId);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    systemAbility->UnlockScreen(listener);
    return true;
}

bool FuzzIsDeviceLocked(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool isDisabled = 0;
    systemAbility->IsDeviceLocked(userId, isDisabled);

    userId = rawData[0];
    const int rawValue = 2;
    isDisabled = static_cast<bool>(rawData[0] % rawValue);
    systemAbility->IsDeviceLocked(userId, isDisabled);
    return true;
}

bool FuzzRegisterInnerListener(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    sptr<InnerListenerIf> InnerListenerIfTest1 = nullptr;
    systemAbility->RegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);

    systemAbility->RegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);

    userId = rawData[0];
    systemAbility->RegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);

    systemAbility->RegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);
    return true;
}

bool FuzzSetScreenlocked(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    systemAbility->SetScreenlocked(false, userId);
    systemAbility->SetScreenlocked(true, userId);
    systemAbility->IsScreenLocked();

    userId = rawData[0];
    systemAbility->SetScreenlocked(false, userId);
    systemAbility->IsScreenLocked();
    systemAbility->SetScreenlocked(true, userId);
    return true;
}

bool FuzzStrongAuthChanged(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t reasonFlag = 0;
    systemAbility->StrongAuthChanged(userId, reasonFlag);

    userId = rawData[0];
    systemAbility->StrongAuthChanged(userId, reasonFlag);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    systemAbility->Unlock(listener);
    return true;
}

bool FuzzLock(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    systemAbility->Lock(userId);

    userId = rawData[0];
    systemAbility->Lock(userId);
    return true;
}

bool FuzzOnActiveUser(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t otherUserId = 102;
    systemAbility->OnActiveUser(userId, otherUserId);
    systemAbility->OnRemoveUser(otherUserId);

    userId = rawData[0];
    otherUserId = rawData[0];
    systemAbility->OnActiveUser(userId, otherUserId);
    systemAbility->OnRemoveUser(otherUserId);
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
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::ONSYSTEMEVENT), data, size);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    OHOS::FuzzIsLocked(data, size);
    OHOS::FuzzIsLockedWithUserId(data, size);
    OHOS::FuzzSendScreenLockEvent(data, size);
    OHOS::FuzzIsScreenLockDisabled(data, size);
    OHOS::FuzzSetScreenLockDisabled(data, size);
    OHOS::FuzzSetScreenLockAuthState(data, size);
    OHOS::FuzzRequestStrongAuth(data, size);
    OHOS::FuzzIsDeviceLocked(data, size);
    OHOS::FuzzRegisterInnerListener(data, size);
    OHOS::FuzzSetScreenlocked(data, size);
    OHOS::FuzzStrongAuthChanged(data, size);
    OHOS::FuzzLock(data, size);
    OHOS::FuzzOnActiveUser(data, size);
    return 0;
}