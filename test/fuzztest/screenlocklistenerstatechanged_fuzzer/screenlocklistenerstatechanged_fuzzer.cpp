/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "screenlocklistenerstatechanged_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#define private public
#define protected public
#include "screenlock_manager.h"
#undef private
#undef protected

#include "screenlock_common.h"
#include "sclock_log.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr int32_t THRESHOLD = 4;
constexpr size_t LENGTH = 1;
constexpr int32_t DEFAULT_USER = 100;

class StrongAuthListenerFuzzTest : public StrongAuthListener {
public:
    explicit StrongAuthListenerFuzzTest(int32_t userId) : StrongAuthListener(userId) {};
    virtual ~StrongAuthListenerFuzzTest() = default;
    void OnStrongAuthChanged(int32_t userId, int32_t authenticated) override
    {
        userId_ = userId;
        authenticated_ = authenticated;
    }
    int32_t userId_ = 0;
    int32_t authenticated_ = 0;
};

class DeviceLockedListenerFuzzTest : public DeviceLockedListener {
public:
    explicit DeviceLockedListenerFuzzTest(int32_t userId) : DeviceLockedListener(userId) {};
    virtual ~DeviceLockedListenerFuzzTest() = default;
    void OnDeviceLockStateChanged(int userId, bool isDeviceLocked) override
    {
        userId_ = userId;
        isDeviceLocked_ = isDeviceLocked;
    }
    int userId_ = 0;
    bool isDeviceLocked_ = false;
};

bool FuzzStrongAuthListenerOnStateChanged(const uint8_t *rawData, size_t size)
{
    if (size < THRESHOLD) {
        return true;
    }

    int32_t userId = static_cast<int32_t>(rawData[0]);
    int32_t state = static_cast<int32_t>(rawData[1]);

    sptr<StrongAuthListenerFuzzTest> listener = new (std::nothrow) StrongAuthListenerFuzzTest(DEFAULT_USER);
    if (listener == nullptr) {
        return false;
    }

    listener->OnStateChanged(userId, state);

    listener->OnStateChanged(DEFAULT_USER, 0);
    listener->OnStateChanged(DEFAULT_USER, 1);
    listener->OnStateChanged(DEFAULT_USER, -1);

    int32_t randomUserId = static_cast<int32_t>((rawData[0] << 8) | rawData[1]);
    int32_t randomState = static_cast<int32_t>((rawData[2] << 8) | rawData[3]);
    listener->OnStateChanged(randomUserId, randomState);

    listener->OnStateChanged(0, state);
    listener->OnStateChanged(-1, state);
    listener->OnStateChanged(INT32_MAX, state);
    listener->OnStateChanged(INT32_MIN, state);

    listener->OnStateChanged(userId, 0);
    listener->OnStateChanged(userId, 1);
    listener->OnStateChanged(userId, INT32_MAX);
    listener->OnStateChanged(userId, INT32_MIN);

    return true;
}

bool FuzzDeviceLockedListenerOnStateChanged(const uint8_t *rawData, size_t size)
{
    if (size < THRESHOLD) {
        return true;
    }

    int32_t userId = static_cast<int32_t>(rawData[0]);
    int32_t state = static_cast<int32_t>(rawData[1]);

    sptr<DeviceLockedListenerFuzzTest> listener = new (std::nothrow) DeviceLockedListenerFuzzTest(DEFAULT_USER);
    if (listener == nullptr) {
        return false;
    }

    listener->OnStateChanged(userId, state);

    listener->OnStateChanged(DEFAULT_USER, 0);
    listener->OnStateChanged(DEFAULT_USER, 1);
    listener->OnStateChanged(DEFAULT_USER, -1);
    listener->OnStateChanged(DEFAULT_USER, 2);

    int32_t randomUserId = static_cast<int32_t>((rawData[0] << 8) | rawData[1]);
    int32_t randomState = static_cast<int32_t>((rawData[2] << 8) | rawData[3]);
    listener->OnStateChanged(randomUserId, randomState);

    listener->OnStateChanged(0, state);
    listener->OnStateChanged(-1, state);
    listener->OnStateChanged(INT32_MAX, state);
    listener->OnStateChanged(INT32_MIN, state);

    listener->OnStateChanged(userId, 0);
    listener->OnStateChanged(userId, 1);
    listener->OnStateChanged(userId, INT32_MAX);
    listener->OnStateChanged(userId, INT32_MIN);

    return true;
}

bool FuzzScreenLockSaDeathRecipientOnRemoteDied(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    sptr<ScreenLockManager> manager = ScreenLockManager::GetInstance();
    if (manager == nullptr) {
        return false;
    }

    sptr<ScreenLockManager::ScreenLockSaDeathRecipient> deathRecipient =
        new (std::nothrow) ScreenLockManager::ScreenLockSaDeathRecipient();
    if (deathRecipient == nullptr) {
        return false;
    }

    wptr<IRemoteObject> nullObject = nullptr;
    deathRecipient->OnRemoteDied(nullObject);

    return true;
}

bool FuzzListenerGetUserId(const uint8_t *rawData, size_t size)
{
    if (size < LENGTH) {
        return true;
    }

    int32_t userId = static_cast<int32_t>(rawData[0]);

    sptr<StrongAuthListenerFuzzTest> strongListener = new (std::nothrow) StrongAuthListenerFuzzTest(userId);
    if (strongListener != nullptr) {
        int32_t ret = strongListener->GetUserId();
        (void)ret;
    }

    sptr<DeviceLockedListenerFuzzTest> deviceListener = new (std::nothrow) DeviceLockedListenerFuzzTest(userId);
    if (deviceListener != nullptr) {
        int32_t ret = deviceListener->GetUserId();
        (void)ret;
    }

    sptr<StrongAuthListenerFuzzTest> strongListenerDefault = new (std::nothrow) StrongAuthListenerFuzzTest(DEFAULT_USER);
    if (strongListenerDefault != nullptr) {
        int32_t ret = strongListenerDefault->GetUserId();
        (void)ret;
    }

    sptr<DeviceLockedListenerFuzzTest> deviceListenerDefault = new (std::nothrow) DeviceLockedListenerFuzzTest(DEFAULT_USER);
    if (deviceListenerDefault != nullptr) {
        int32_t ret = deviceListenerDefault->GetUserId();
        (void)ret;
    }

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
    OHOS::FuzzStrongAuthListenerOnStateChanged(data, size);
    OHOS::FuzzDeviceLockedListenerOnStateChanged(data, size);
    OHOS::FuzzScreenLockSaDeathRecipientOnRemoteDied(data, size);
    OHOS::FuzzListenerGetUserId(data, size);
    return 0;
}
