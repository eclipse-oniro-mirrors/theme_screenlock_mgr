/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef SERVICES_INCLUDE_SCLOCK_SERVICE_STUB_H
#define SERVICES_INCLUDE_SCLOCK_SERVICE_STUB_H

#include <cstdint>
#include <map>

#include "iremote_stub.h"
#include "screenlock_manager_interface.h"

namespace OHOS {
namespace ScreenLock {
class ScreenLockManagerStub : public IRemoteStub<ScreenLockManagerInterface> {
    using handleFunc = int32_t (ScreenLockManagerStub::*)(MessageParcel &, MessageParcel &);
    using HandleFuncMap = std::map<uint32_t, handleFunc>;

public:
    ScreenLockManagerStub();
    ~ScreenLockManagerStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    void InitHandleMap(void);
    int32_t OnIsLocked(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsScreenLocked(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetSecure(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnlock(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnlockScreen(MessageParcel &data, MessageParcel &reply);
    int32_t OnLock(MessageParcel &data, MessageParcel &reply);
    int32_t OnSendScreenLockEvent(MessageParcel &data, MessageParcel &reply);
    int32_t OnScreenLockOn(MessageParcel &data, MessageParcel &reply);
    int32_t OnLockScreen(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsScreenLockDisabled(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetScreenLockDisabled(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetScreenLockAuthState(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetScreenLockAuthState(MessageParcel &data, MessageParcel &reply);

    HandleFuncMap handleFuncMap;
};
} // namespace ScreenLock
} // namespace OHOS
#endif // SERVICES_INCLUDE_SCLOCK_SERVICE_STUB_H