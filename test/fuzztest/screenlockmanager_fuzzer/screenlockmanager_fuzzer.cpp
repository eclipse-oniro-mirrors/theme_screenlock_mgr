/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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


#include "screenlockmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "message_parcel.h"
#include "screenlock_app_manager.h"
#include "screenlock_callback.h"
#include "screenlock_manager.h"
#include "screenlock_manager_interface.h"
#include "screenlock_system_ability.h"

using namespace OHOS::ScreenLock;

namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr int32_t OFFSET = 4;
constexpr size_t RANDNUM_ZERO = 0;
constexpr size_t RANDNUM_ONE = 1;
constexpr size_t RANDNUM_TWO = 2;
constexpr size_t RANDNUM_THREE = 3;

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
    if (code == RANDNUM_ZERO) {
        return ScreenLockManager::GetInstance()->IsScreenLocked();
    }
    if (code == RANDNUM_ONE) {
        return ScreenLockManager::GetInstance()->GetSecure();
    }
    if (code == RANDNUM_TWO) {
        sptr<ScreenLockSystemAbilityInterface> listener_ = new ScreenLockSystemAbilityStub();
        int32_t ret = ScreenLockManager::GetInstance()->Unlock(Action::UNLOCK, listener_);
        return ret == E_SCREENLOCK_OK;
    }
    if (code == RANDNUM_THREE) {
        sptr<ScreenLockSystemAbilityInterface> listener_ = new ScreenLockSystemAbilityStub();
        int32_t ret = ScreenLockManager::GetInstance()->Lock(listener_);
        return ret == E_SCREENLOCK_OK;
    }
    return true;
}

bool FuzzScreenlockAppManager(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;
    if (code == RANDNUM_ZERO) {
        sptr<ScreenLockSystemAbilityInterface> listener_ = new ScreenLockSystemAbilityStub();
        int32_t ret = ScreenLockAppManager::GetInstance()->OnSystemEvent(listener_);
        return ret == E_SCREENLOCK_OK;
    }
    if (code == RANDNUM_ONE) {
        int param = 0;
        std::string event(reinterpret_cast<const char *>(rawData), size);
        int32_t ret = ScreenLockAppManager::GetInstance()->SendScreenLockEvent(event, param);
        return ret == E_SCREENLOCK_OK;
    }
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
    OHOS::FuzzScreenlockManager(data, size);
    OHOS::FuzzScreenlockAppManager(data, size);
    return 0;
}