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

#include "screenlockmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "message_parcel.h"
#include "screenlock_callback.h"
#include "screenlock_manager.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability.h"
#include "screenlock_system_ability_callback.h"
#include "commeventsubscriber.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr int32_t OFFSET = 4;
constexpr size_t LENGTH = 1;
constexpr size_t RANDNUM_ZERO = 0;
constexpr size_t RANDNUM_ONE = 1;
constexpr size_t RANDNUM_TWO = 2;
constexpr size_t DEFAULT_USER = 100;
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

uint32_t ConvertToUint32(const uint8_t *ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    uint32_t bigvar = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
    return bigvar;
}

bool FuzzScreenlockManager(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    EventListener eventListener;
    if (code == RANDNUM_ZERO) {
        return ScreenLockManager::GetInstance()->IsScreenLocked();
    }
    if (code == RANDNUM_ONE) {
        return ScreenLockManager::GetInstance()->GetSecure();
    }
    if (code == RANDNUM_TWO) {
        sptr<ScreenlockCallback> listener_ = new ScreenlockCallback(eventListener);
        int32_t ret = ScreenLockManager::GetInstance()->Lock(listener_);
        
        listener_ = nullptr;
        ScreenLockManager::GetInstance()->Lock(listener_);
        return ret == E_SCREENLOCK_OK;
    }
    return true;
}

bool UnlockFuzzTest(const uint8_t *rawData, size_t size)
{
    EventListener eventListener;
    sptr<ScreenlockCallback> listener_ = new ScreenlockCallback(eventListener);
    if (size < LENGTH) {
        return true;
    }
    int32_t count = 3;
    int32_t ret = ScreenLockManager::GetInstance()->Unlock(static_cast<Action>(rawData[0] % count), listener_);
    
    listener_ = nullptr;
    ScreenLockManager::GetInstance()->Unlock(static_cast<Action>(rawData[0] % count), listener_);
    return ret == E_SCREENLOCK_OK;
}

bool IsLockedFuzzTest(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    bool isLocked = static_cast<bool>(rawData[0] % 2);
    int32_t ret = ScreenLockManager::GetInstance()->IsLocked(isLocked);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockAppManager(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;
    EventListener eventListener;
    if (code == RANDNUM_ZERO) {
        sptr<ScreenlockSystemAbilityCallback> listener_ = new ScreenlockSystemAbilityCallback(eventListener);
        int32_t ret = ScreenLockManager::GetInstance()->OnSystemEvent(listener_);
        return ret == E_SCREENLOCK_OK;
    }
    if (code == RANDNUM_ONE) {
        int param = 0;
        std::string event(reinterpret_cast<const char *>(rawData), size);
        int32_t ret = ScreenLockManager::GetInstance()->SendScreenLockEvent(event, param);
        return ret == E_SCREENLOCK_OK;
    }
    return true;
}

bool FuzzScreenlockIsDisabled(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    bool isDisabled = static_cast<bool>(rawData[0] % 2);
    int32_t ret = ScreenLockManager::GetInstance()->IsScreenLockDisabled(userId, isDisabled);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockSetDisabled(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    bool isDisabled = static_cast<bool>(rawData[0] % 2);
    int32_t ret = ScreenLockManager::GetInstance()->SetScreenLockDisabled(isDisabled, userId);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockSetAuthState(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    int32_t authState = 2;
    std::string authToken = "test";
    int32_t ret = ScreenLockManager::GetInstance()->SetScreenLockAuthState(authState, userId, authToken);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockGetAuthState(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    int32_t authState = 2;
    int32_t ret = ScreenLockManager::GetInstance()->GetScreenLockAuthState(userId, authState);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockRequestStrongAuth(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    int reasonFlag = 1;
    int32_t ret = ScreenLockManager::GetInstance()->RequestStrongAuth(reasonFlag, userId);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzScreenlockGetStrongAuth(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    int reasonFlag = 1;
    int32_t ret = ScreenLockManager::GetInstance()->GetStrongAuth(userId, reasonFlag);
    return ret == E_SCREENLOCK_OK;
}

bool FuzzIsDeviceLocked(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = rawData[0];
    bool isDeviceLocked = static_cast<bool>(rawData[0] % 2);
    int32_t ret = ScreenLockManager::GetInstance()->IsDeviceLocked(userId, isDeviceLocked);
    if (userId != DEFAULT_USER) {
        return ret == E_SCREENLOCK_USER_ID_INVALID;
    } else {
        return ret == E_SCREENLOCK_OK;
    }
}

bool FuzzIsLockedWithUserId(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = rawData[0];
    bool isLocked = static_cast<bool>(rawData[0] % 2);
    int32_t ret = ScreenLockManager::GetInstance()->IsLockedWithUserId(userId, isLocked);
    if (userId != DEFAULT_USER) {
        return ret == E_SCREENLOCK_USER_ID_INVALID;
    } else {
        return ret == E_SCREENLOCK_OK;
    }
}

bool FuzzLock(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }
    int32_t userId = 100;
    int32_t ret = ScreenLockManager::GetInstance()->Lock(userId);

    userId = rawData[0];
    ret = ScreenLockManager::GetInstance()->Lock(userId);
    if (userId != DEFAULT_USER) {
        return ret == E_SCREENLOCK_USER_ID_INVALID;
    } else {
        return ret == E_SCREENLOCK_OK;
    }
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
    OHOS::FuzzScreenlockManager(data, size);
    OHOS::UnlockFuzzTest(data, size);
    OHOS::IsLockedFuzzTest(data, size);
    OHOS::FuzzScreenlockAppManager(data, size);
    OHOS::FuzzScreenlockIsDisabled(data, size);
    OHOS::FuzzScreenlockSetDisabled(data, size);
    OHOS::FuzzScreenlockSetAuthState(data, size);
    OHOS::FuzzScreenlockGetAuthState(data, size);
    OHOS::FuzzScreenlockRequestStrongAuth(data, size);
    OHOS::FuzzScreenlockGetStrongAuth(data, size);
    OHOS::FuzzIsDeviceLocked(data, size);
    OHOS::FuzzIsLockedWithUserId(data, size);
    OHOS::FuzzLock(data, size);
    return 0;
}