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
#include "screenlock_system_ability_interface.h"
#include "screenlock_common.h"
#include "system_ability_definition.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;

class ScreenLockCallbackTest : public IRemoteStub<ScreenLockCallbackInterface> {
public:
    void OnCallBack(int32_t screenLockResult) override {}
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
};

class ScreenlockSystemAbilityCallbackTest : public IRemoteStub<ScreenLockSystemAbilityInterface> {
public:
    void OnCallBack(const SystemEvent &systemEvent) override {}
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
};

bool FuzzIsLocked(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    // bool
    bool isLocked = 0;
    systemAbility->IsLocked(isLocked);

    isLocked = fdp.ConsumeBool();
    systemAbility->IsLocked(isLocked);
    return true;
}

bool FuzzIsLockedWithUserId(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    // bool
    bool isLocked = 0;
    systemAbility->IsLockedWithUserId(userId, isLocked);

    userId = fdp.ConsumeIntegral<int32_t>();
    isLocked = fdp.ConsumeBool();
    systemAbility->IsLockedWithUserId(userId, isLocked);
    systemAbility->GetSecure();
    return true;
}

bool FuzzSendScreenLockEvent(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    const std::string eventOne = UNLOCK_SCREEN_RESULT;
    int param = 0;
    systemAbility->SendScreenLockEvent(eventOne, param);

    const std::string eventTwo = SCREEN_DRAWDONE;
    systemAbility->SendScreenLockEvent(eventTwo, param);

    const std::string eventThree = LOCK_SCREEN_RESULT;
    systemAbility->SendScreenLockEvent(eventThree, param);

    param = fdp.ConsumeIntegral<int32_t>();
    systemAbility->SendScreenLockEvent(eventOne, param);
    systemAbility->SendScreenLockEvent(eventTwo, param);
    systemAbility->SendScreenLockEvent(eventThree, param);
    sptr<ScreenLockSystemAbilityInterface> listener = nullptr;
    if (fdp.ConsumeBool()) {
        listener = new ScreenlockSystemAbilityCallbackTest();
    }
    systemAbility->OnSystemEvent(listener);
    return true;
}

bool FuzzIsScreenLockDisabled(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool isDisabled = 0;
    systemAbility->IsScreenLockDisabled(userId, isDisabled);

    userId = fdp.ConsumeIntegral<int32_t>();
    isDisabled = fdp.ConsumeBool();
    systemAbility->IsScreenLockDisabled(userId, isDisabled);
    return true;
}

bool FuzzSetScreenLockDisabled(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool disable = 0;
    systemAbility->SetScreenLockDisabled(disable, userId);

    userId = fdp.ConsumeIntegral<int32_t>();
    disable = fdp.ConsumeBool();
    systemAbility->SetScreenLockDisabled(disable, userId);
    return true;
}

bool FuzzSetScreenLockAuthState(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int authState = 1;
    int32_t userId = 100;
    std::string authToken = "test";
    systemAbility->SetScreenLockAuthState(authState, userId, authToken);
    systemAbility->GetScreenLockAuthState(userId, authState);

    userId = fdp.ConsumeIntegral<int32_t>();
    authState = fdp.ConsumeIntegral<int32_t>();
    authToken = fdp.ConsumeRandomLengthString(64);
    systemAbility->SetScreenLockAuthState(authState, userId, authToken);
    systemAbility->GetScreenLockAuthState(userId, authState);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    if (fdp.ConsumeBool()) {
        listener = new ScreenLockCallbackTest();
    }
    systemAbility->Lock(listener);
    return true;
}

bool FuzzRequestStrongAuth(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int reasonFlag = 1;
    int32_t userId = 100;
    systemAbility->RequestStrongAuth(reasonFlag, userId);

    userId = fdp.ConsumeIntegral<int32_t>();
    reasonFlag = fdp.ConsumeIntegral<int32_t>();
    systemAbility->RequestStrongAuth(reasonFlag, userId);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    if (fdp.ConsumeBool()) {
        listener = new ScreenLockCallbackTest();
    }
    systemAbility->UnlockScreen(listener);
    return true;
}

bool FuzzIsDeviceLocked(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int userId = 100;
    bool isDisabled = 0;
    systemAbility->IsDeviceLocked(userId, isDisabled);

    userId = fdp.ConsumeIntegral<int32_t>();
    isDisabled = fdp.ConsumeBool();
    systemAbility->IsDeviceLocked(userId, isDisabled);
    return true;
}

bool FuzzRegisterInnerListener(FuzzedDataProvider &fdp)
{
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

    userId = fdp.ConsumeIntegral<int32_t>();
    systemAbility->RegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::DEVICE_LOCK, InnerListenerIfTest1);

    systemAbility->RegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);
    systemAbility->UnRegisterInnerListener(userId, ListenType::STRONG_AUTH, InnerListenerIfTest1);
    return true;
}

bool FuzzSetScreenlocked(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    systemAbility->SetScreenlocked(false, userId);
    systemAbility->SetScreenlocked(true, userId);
    systemAbility->IsScreenLocked();

    userId = fdp.ConsumeIntegral<int32_t>();
    systemAbility->SetScreenlocked(false, userId);
    systemAbility->IsScreenLocked();
    systemAbility->SetScreenlocked(true, userId);
    return true;
}

bool FuzzStrongAuthChanged(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t reasonFlag = 0;
    systemAbility->StrongAuthChanged(userId, reasonFlag);

    userId = fdp.ConsumeIntegral<int32_t>();
    systemAbility->StrongAuthChanged(userId, reasonFlag);
    sptr<ScreenLockCallbackInterface> listener = nullptr;
    if (fdp.ConsumeBool()) {
        listener = new ScreenLockCallbackTest();
    }
    systemAbility->Unlock(listener);
    return true;
}

bool FuzzLock(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    systemAbility->Lock(userId);

    userId = fdp.ConsumeIntegral<int32_t>();
    systemAbility->Lock(userId);
    return true;
}

bool FuzzOnActiveUser(FuzzedDataProvider &fdp)
{
    auto systemAbility = ScreenLockSystemAbility::GetInstance();
    if (systemAbility == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t otherUserId = 102;
    systemAbility->OnActiveUser(userId, otherUserId);
    systemAbility->OnRemoveUser(otherUserId);

    userId = fdp.ConsumeIntegral<int32_t>();
    otherUserId = fdp.ConsumeIntegral<int32_t>();
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
    FuzzedDataProvider fdp(data, size);
    OHOS::FuzzIsLocked(fdp);
    OHOS::FuzzIsLockedWithUserId(fdp);
    OHOS::FuzzSendScreenLockEvent(fdp);
    OHOS::FuzzIsScreenLockDisabled(fdp);
    OHOS::FuzzSetScreenLockDisabled(fdp);
    OHOS::FuzzSetScreenLockAuthState(fdp);
    OHOS::FuzzRequestStrongAuth(fdp);
    OHOS::FuzzIsDeviceLocked(fdp);
    OHOS::FuzzRegisterInnerListener(fdp);
    OHOS::FuzzSetScreenlocked(fdp);
    OHOS::FuzzStrongAuthChanged(fdp);
    OHOS::FuzzLock(fdp);
    OHOS::FuzzOnActiveUser(fdp);
    return 0;
}