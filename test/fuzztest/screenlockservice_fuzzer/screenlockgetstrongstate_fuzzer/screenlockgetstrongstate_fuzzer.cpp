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

#include "screenlockgetstrongstate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#define private public
#define protected public
#include "screenlock_system_ability.h"
#include "innerlistenermanager.h"
#undef private
#undef protected

#include "screenlock_server_ipc_interface_code.h"
#include "screenlock_service_fuzz_utils.h"
#include "innerListener_fuzz_utils.h"
#include "screenlock_manager.h"
#include "screenlock_callback_interface.h"
#include "screenlock_common.h"
#include "system_ability_definition.h"
#include <random>
#include <string>
#include "sclock_log.h"
#include "commeventsubscriber.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
constexpr size_t LENGTH = 1;
constexpr size_t DEFAULT_USER = 100;
sptr<StrongAuthListener> StrongAuthListenerTest1 = new (std::nothrow) StrongAuthListenerTest(100);
sptr<DeviceLockedListener> DeviceLockedListenerTest1 = new (std::nothrow) DeviceLockedListenerTest(100);
sptr<InnerListenerIfTest> InnerListenerIfTest1 = new (std::nothrow) InnerListenerIfTest();
const std::string AUTH_PIN = "1";
const std::string HAS_CREDENTIAL = "1";
const std::string USER_CREDENTIAL_UPDATED_EVENT = "USER_CREDENTIAL_UPDATED_EVENT";
const std::string USER_CREDENTIAL_UPDATED_NONE = "USER_CREDENTIAL_UPDATED_NONE";

bool FuzzSubscribeEvent(const uint8_t *rawData, size_t size)
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

bool FuzzRegisterStrongAuthListener(const uint8_t *rawData, size_t size)
{
    SCLOCK_HILOGW("An11");
    if (size < LENGTH) {
        return true;
    }
    if (StrongAuthListenerTest1 == nullptr) {
        StrongAuthListenerTest1 = new (std::nothrow) StrongAuthListenerTest(DEFAULT_USER);
    }
    int32_t ret = ScreenLockManager::GetInstance()->RegisterStrongAuthListener(StrongAuthListenerTest1);
    ScreenLockManager::GetInstance()->UnRegisterStrongAuthListener(StrongAuthListenerTest1);
    
    StrongAuthListenerTest1 = nullptr;
    ScreenLockManager::GetInstance()->RegisterStrongAuthListener(StrongAuthListenerTest1);
    ScreenLockManager::GetInstance()->UnRegisterStrongAuthListener(StrongAuthListenerTest1);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzRegisterDeviceLockedListener(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    if (DeviceLockedListenerTest1 == nullptr) {
        DeviceLockedListenerTest1 = new (std::nothrow) DeviceLockedListenerTest(DEFAULT_USER);
    }
    int32_t ret = ScreenLockManager::GetInstance()->RegisterDeviceLockedListener(DeviceLockedListenerTest1);
    ScreenLockManager::GetInstance()->UnRegisterDeviceLockedListener(DeviceLockedListenerTest1);
    
    DeviceLockedListenerTest1 = nullptr;
    ScreenLockManager::GetInstance()->RegisterDeviceLockedListener(DeviceLockedListenerTest1);
    ScreenLockManager::GetInstance()->UnRegisterDeviceLockedListener(DeviceLockedListenerTest1);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzRegisterInnerListenerOne(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    ListenType state = static_cast<ListenType>(rawData[0] % 2);
    sptr<InnerListenerIf> InnerListenerOne = nullptr;
    InnerListener->RegisterInnerListener(userId, state, InnerListenerOne);
    InnerListener->UnRegisterInnerListener(state, InnerListenerOne);

    InnerListenerOne = InnerListenerIfTest1;
    InnerListener->RegisterInnerListener(userId, state, InnerListenerOne);
    InnerListener->UnRegisterInnerListener(state, InnerListenerOne);
    return true;
}

bool FuzzAddInnerListener(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    ListenType state = static_cast<ListenType>(rawData[0] % 2);
    InnerListener->RemoveInnerListener(state, InnerListenerIfTest1);
    InnerListener->AddInnerListener(userId, state, InnerListenerIfTest1);
    InnerListener->AddInnerListener(userId, state, InnerListenerIfTest1);
    InnerListener->RemoveInnerListener(state, InnerListenerIfTest1);
    return true;
}

bool FuzzOnStrongAuthChanged(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    ListenType state = static_cast<ListenType>(rawData[0] % 2);
    sptr<InnerListenerIf> InnerListenerStateChange = nullptr;
    InnerListener->AddDeathRecipient(state, InnerListenerStateChange);
    InnerListener->RemoveDeathRecipient(InnerListenerStateChange);
    return true;
}

bool FuzzHasListenerSet(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    InnerListener->HasListenerSet(userId, ListenType::DEVICE_LOCK);

    InnerListener->HasListenerSet(userId, ListenType::STRONG_AUTH);

    userId = rawData[0];
    InnerListener->HasListenerSet(userId, ListenType::DEVICE_LOCK);

    InnerListener->HasListenerSet(userId, ListenType::STRONG_AUTH);
    return true;
}

bool FuzzOnDeviceLockStateChanged(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t lockState = 1;
    InnerListener->OnDeviceLockStateChanged(userId, lockState);

    userId = rawData[0];
    InnerListener->OnDeviceLockStateChanged(userId, lockState);

    return true;
}

bool FuzzOnStateChanged(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    auto InnerListener = InnerListenerManager::GetInstance();
    if (InnerListener == nullptr) {
        return false;
    }

    int32_t userId = 100;
    int32_t lockState = 1;
    InnerListener->OnStateChanged(userId, lockState, ListenType::DEVICE_LOCK);

    InnerListener->OnStateChanged(userId, lockState, ListenType::STRONG_AUTH);

    userId = rawData[0];
    InnerListener->OnStateChanged(userId, lockState, ListenType::DEVICE_LOCK);

    InnerListener->OnStateChanged(userId, lockState, ListenType::STRONG_AUTH);

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
        static_cast<uint32_t>(ScreenLockServerIpcInterfaceCode::GET_STRONG_AUTHSTATE), data, size);
    ScreenLockSystemAbility::GetInstance()->ResetFfrtQueue();
    OHOS::FuzzSubscribeEvent(data, size);
    OHOS::FuzzRegisterStrongAuthListener(data, size);
    OHOS::FuzzRegisterDeviceLockedListener(data, size);
    OHOS::FuzzRegisterInnerListenerOne(data, size);
    OHOS::FuzzAddInnerListener(data, size);
    OHOS::FuzzOnStrongAuthChanged(data, size);
    OHOS::FuzzHasListenerSet(data, size);
    OHOS::FuzzOnDeviceLockStateChanged(data, size);
    OHOS::FuzzOnStateChanged(data, size);
    return 0;
}